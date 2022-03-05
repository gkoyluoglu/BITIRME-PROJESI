// ServoControl
// ELE 495 BitirimeProjesi
// By Ahmet Gurkan Koyluoglu and Tugce Bayindir

// Include the Servo library
#include <PID_v1.h>
#include <Servo.h>
#include <SoftwareSerial.h>
//Panel
#define PIN_TR 1
#define PIN_TL 2
#define PIN_S  A5
#define PIN_BL 3
#define PIN_BR 4
#define SETTLE_TIME 35
#define STATE_START 0
#define WAIT_TIME 1000
int x_pos;
int y_pos;
struct P{
  double x;
  double y;
};
P p;
unsigned long panel_time;
int state;
int q, a;
//Servo pin
int servo1Pin = 8;
int servo2Pin = 11;
#define SERVO1_START_DEGREE  95
#define SERVO2_START_DEGREE  90
//PID for Servo 1
double p1 = 0.14;
double i1 = 0.004;
double d1 = 0.07;
double input1;
double output1;
double setpoint1 = 198;//198
PID s1Controller(&input1 ,&output1,&setpoint1 , p1,i1,d1,DIRECT);

//PID for Servo 2
double p2 = 0.14;
double i2 = 0.004;
double d2 = 0.07;
double input2;
double output2;
double setpoint2 = 165.8;//165.8
PID s2Controller(&input2 ,&output2,&setpoint2 , p2,i2,d2,REVERSE);

//Bluetooh Haberlesme
SoftwareSerial bluetoothModulu(10, 12); 
double x ;
double y ;
double degx;
double degy;

// Creat a servo object
Servo servo1 , servo2;

void setup() {
  Serial.begin(9600);
  //Configure Panel
  pinMode(PIN_TR, OUTPUT);
  pinMode(PIN_TL, OUTPUT);
  pinMode(PIN_BL, OUTPUT);
  pinMode(PIN_BR, OUTPUT);
  digitalWrite(PIN_TR, LOW);
  digitalWrite(PIN_TL, LOW);
  digitalWrite(PIN_BL, LOW);
  digitalWrite(PIN_BR, LOW);
  pinMode(PIN_S, INPUT);
  state = STATE_START;
  // Attach Servos to pin
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  output1 = 95;
  output2 = 95;
  servo1.write(90);
  servo1.write(90);
  //PID
  s1Controller.SetMode(AUTOMATIC);
  s1Controller.SetOutputLimits(SERVO1_START_DEGREE-40, SERVO1_START_DEGREE+40);
  s2Controller.SetMode(AUTOMATIC);
  s2Controller.SetOutputLimits(SERVO2_START_DEGREE-40, SERVO2_START_DEGREE+40);
  //Bluetooh
  bluetoothModulu.begin(9600); 
}
void readData() {
  int temp;
  switch (state)
  {
    case 0: // Wait for button press
      if (analogRead(PIN_S) != 0) {
        state = 1;
      } else {
        state = 0; // Otherwise, do nothing
      }
      break;

    case 1: // Set up X read
      //~~~~~~~~~~~~~~~~
      // Set up X read
      //~~~~~~~~~~~~~~~~
      // Set TR and BR high
      digitalWrite(PIN_TR, HIGH);
      digitalWrite(PIN_BR, HIGH);
      // Set TL and BL low
      digitalWrite(PIN_TL, LOW);
      digitalWrite(PIN_BL, LOW);

      //~~~~~~~~~~~~~~~~
      // Move to next state
      //~~~~~~~~~~~~~~~~
      // Record the time
      panel_time = millis();
      // Switch to waiting for the panel to settle
      state = 2;
      break;

    case 2: // Wait for panel to settle
      // How long has it been since the panel changed configuration?
      temp = millis() - panel_time;

      // Has it been long enough?
      if (temp >= SETTLE_TIME)
      { // If it has...

        // Switch to the Read X state
        state = 3;
      } // Otherwise, do nothing.
      break;

    case 3: // Read the X position
      // Read the sensor voltage (X position)
      x_pos = analogRead(PIN_S);
      p.x=getXValue(x_pos);
      // Switch to the set up Y state
      state = 4;
      break;

    case 4: // Set up Y read
      //~~~~~~~~~~~~~~~~
      // Set up Y read
      //~~~~~~~~~~~~~~~~
      // Set TR and TL high
      digitalWrite(PIN_TR, HIGH);
      digitalWrite(PIN_TL, HIGH);
      // Set BL and BR low
      digitalWrite(PIN_BL, LOW);
      digitalWrite(PIN_BR, LOW);

      //~~~~~~~~~~~~~~~~
      // Move to next state
      //~~~~~~~~~~~~~~~~
      // Record the time
      panel_time = millis();
      // Switch to waiting for the panel to settle
      state = 5;
      break;

    case 5: // Wait for panel to settle
      // How long has it been since the panel changed configuration?
      temp = millis() - panel_time;

      // Has it been long enough?
      if (temp >= SETTLE_TIME)
      { // If it has...

        // Switch to the Read Y state
        state = 6;
      } // Otherwise, do nothing.
      break;

    case 6: // Read the Y position
      // Read the sensor voltage (Y position)
      y_pos = analogRead(PIN_S);
      p.y=getYValue(y_pos);
      // Switch to the Output coords state
      state = 7;
      break;

    case 7: // Output coords to Serial
      // Write the coordinates to Serial
      
      input1 = getXMM(x_pos);
      input2 = getYMM(y_pos);
      Serial.print(p.x);
      Serial.print("\t");
      Serial.println(p.y);
      s1Controller.Compute();
      s2Controller.Compute();
      // Switch to the Turn off the panel state
      state = 8;
      break;

    case 8: // Turn off the panel
      // Turn off the panel
      //moveServo2(output2);
      //moveServo1(output1);
      servo1.write(output1);
      servo2.write(output2);
      Serial.print(output1);
      Serial.print(" ");
      Serial.print(" ");
      Serial.println(output2);
      state = 9;
      break;
    case 9:
      bluetoothModulu.write((int)p.x);
      bluetoothModulu.write((int)p.y);
      temp = millis() - panel_time;

      state=0;
      break;
    default: // Error
      Serial.println("Something has gone terribly wrong.");
      break;
  }
}
double getYValue(int x) {
  if (x < 280)
    return -16, 15;
  else if ( x > 730)
    return 16.15;
  else
    return ((x - 280) * 0.0717) - 16.15;
}
double getXValue(int y) {
  if (y < 280)
    return -19.8;
  else if ( y > 730)
    return 19.8;
  else
    return ((y - 280) * 0.088) - 19.8;
}
double getXMM(int y) {
  if (y < 280)
    return 0;
  else if ( y > 730)
    return 396;
  else
    return ((y - 280) * 0.88);
}
double getYMM(int x) {
  if (x < 280)
    return 0;
  else if ( x > 730)
    return 323;
  else
    return ((x - 280) * 0.717);
}
void loop() {
  readData();
}
