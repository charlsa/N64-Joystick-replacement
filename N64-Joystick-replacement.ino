/* Defining pins for the board */
#define analogXpin A6  // Pin 14 on the board, corresponed to A6 on the LaunchPad
#define analogYpin A0  // Pin 2 on the board
#define Xout0 7
#define Xout1 6
#define Yout0 9
#define Yout1 8
#define Button 10

#define maxSteps 127

// Has to be calibrated for etch Joystick...
#define Xmax 505
#define Xmin -418
#define Ymax 505
#define Ymin -517
#define deadZoneX 169
#define deadZoneY 260

// Global Variabels
int currentX = 0; int currentY = 0;
int stateX = 0; int stateY = 0;

void setup() {
  // initialize serial communications at 9600 bps:
  //Serial.begin(9600); // Only used for debug
  // Output to interface existing N64
  pinMode(Xout0, OUTPUT);
  pinMode(Xout1, OUTPUT);
  pinMode(Yout0, OUTPUT);
  pinMode(Yout1, OUTPUT);
}

/* Functions... */
int readJoystick(int pin, int offset) {
  // Reading the joystick value, this function is alos including an avraging filter by 5
  // *may change if the response time of the joystick gets slow...
  int i = 0, value = 0;
  while (i < 5) {
    value += (analogRead(pin) - offset);
   // delay(1);
    i++;
  }
  return value / 5;
}

void moveX(int state, int LSB, int MSB) {
  switch (state) {
    case 0:
      digitalWrite(LSB, HIGH);
      break;
    case 1:
      digitalWrite(MSB, LOW);
      break;
    case 2:
      digitalWrite(LSB, LOW);
      break;
    case 3:
      digitalWrite(MSB, HIGH);
      break;
  }
}

void makeXEqual(int target) {
  // Counting up to joystick value
  if (currentX == target) {
    return;
  }

  if (currentX > target) {
    if (currentX != -maxSteps) {
      if (stateX <= 0) {
        stateX = 3;
      }
      else {
        stateX--;
      }
      currentX--;
    }
    else {
      return;
    }
  }
  else if (currentX < target) {
    if (currentX != maxSteps) {
      if (stateX >= 3) {
        stateX = 0;
      }
      else {
        stateX++;
      }
      currentX++;
    }
    else {
 //     return;
    }
  }
  moveX(stateX, Xout0, Xout1); // Input state 0-3 Digital pin 0 = LSB
}

void move2centerX() {
  if (currentX > 0) {
    if (stateX == 0) {
      stateX = 3;
    }
    else {
      stateX--;
    }
    currentX--;
  }
  else if (currentX < 0) {
    if (stateX == 3) {  
      stateX = 0;
    }
    else {
      stateX++;
    }
    currentX++;
  }
  moveX(stateX, Xout0, Xout1); // Input state 0-3 Digital pin 0 = LSB
}

void moveY(int state, int LSB, int MSB) {

  switch (state) {
    case 0:
      digitalWrite(LSB, HIGH);
      break;
    case 1:
      digitalWrite(MSB, LOW);
      break;
    case 2:
      digitalWrite(LSB, LOW);
      break;
    case 3:
      digitalWrite(MSB, HIGH);
      break;
  }
}

void makeYEqual(int target) {
  if (currentY == target) {
    return;
  }
  if (currentY > target) {
    if (currentY != -maxSteps) {
      if (stateY <= 0) {
        stateY = 3;
      }
      else {
        stateY--;
      }
      currentY--;
    }
  }
  else if (currentY < target) {
    if (currentY != maxSteps) {
      if (stateY >= 3) {
        stateY = 0;
      }
      else {
        stateY++;
      }
      currentY++;
    }
  }
  moveY(stateY, Yout0, Yout1);
}

void move2centerY(int target) {
  if (currentY > 0) {
    if (stateY == 0) {
      stateY = 3;
    }
    else {
      stateY--;
    }
    currentY--;
  }
  else if (currentY < 0) {
    if (stateY == 3) {
      stateY = 0;
    }
    else {
      stateY++;
    }
    currentY++;
  }
  moveY(stateY, Yout0, Yout1);
}

void loop() {
  // Initial values for digital outputs
  digitalWrite(Xout0, HIGH); digitalWrite(Xout1, HIGH); digitalWrite(Yout0, HIGH); digitalWrite(Yout1, HIGH);
  delay(10);
	
  // Variabels
  int offsetX = 0,  offsetY = 0;           // offset values from the joystick
  int newX = 0, newY = 0, stepX = 0, stepY = 0;

  // defined max, min & deadzone values, this are based on test
  int deadzoneX = 0, deadzoneY = 0;
  deadzoneX = map(deadZoneX, Xmin, Xmax, -maxSteps, maxSteps);
  deadzoneY = map(deadZoneY, Ymin, Ymax, -maxSteps, maxSteps);
	
  // Read offset for X and Y
  int i = 0;
  while (i < 10) {
    offsetX += readJoystick(analogXpin, 0);
    offsetY += readJoystick(analogYpin, 0);
    i++;
  }
  offsetY = offsetY / 10; offsetX = offsetX / 10;

  while (1) {
    //delay(5); // to be deleted... or adjusted...

    // Map read value to +/-127 i.e. no. of steps
    newX = readJoystick(analogXpin, offsetX);
    stepX = map(newX, Xmin, Xmax, -maxSteps, maxSteps);
    newY = readJoystick(analogYpin, offsetY);
    stepY = map(newY, Ymin, Ymax, -maxSteps, maxSteps);

    // Filter out values within the deadzone...
    if (abs(stepX) < deadzoneX) {
      stepX = 0;
    }
    if (abs(stepY) < deadzoneY) {
      stepY = 0;
    }

   
    if (stepX > 0) {
      makeXEqual(stepX);
    }
    else if (stepX < 0) {
      makeXEqual(stepX);
    }
    else if (stepX == 0 && currentX != 0) {
      move2centerX();
    }

    if (stepY > 0) {
      makeYEqual(stepY);
    }
    else if (stepY < 0) {
      makeYEqual(stepY);
    }
    else if (stepY == 0 && currentY != 0) {
      move2centerY(stepY);
    }
  }
}
