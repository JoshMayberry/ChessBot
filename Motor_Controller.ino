// Wire Slave Receiver
#include <Wire.h>
#include <Stepper.h>
int angle = 0;
int in1Pin = 4;
int in2Pin = 5;
int in3Pin = 7;
int in4Pin = 8;
int stepCount = 0;
bool stepForward=false;
bool stepBackward=false;
int16_t steps;
int16_t v;
byte a,b,va,vb;
Stepper motor(200, in1Pin, in2Pin, in3Pin, in4Pin);  


int x=0;
int i=0;
void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  pinMode(13,OUTPUT);
  // set up pin mode for motor controll 
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(6, OUTPUT);

  // this line is for Leonardo's, it delays the serial interface
  // until the terminal window is opened
  motor.setSpeed(150);

  // pulse width to stepper
  analogWrite(3,200);
  analogWrite(6,200);
 // motor.step(200);

  
  Serial.begin(9600);
  Serial.println("Slave Ready");
}

void loop() {
    if(i==1){  //run once
      motor.step(steps);
      motor.setSpeed(v);
      //motor.setSpeed(150);
      stepCount = stepCount+steps;//record position 
      i = 0;
      Serial.println(steps);
      Serial.println(v);
    }

    
  delay(10);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {    //3 bits will be recieved. 2 for how many stepps in the positive or negative direction and the 3rd for how fast to move there 
  a = Wire.read();                  // bits are recieved one at a time
  b = Wire.read();                  // so "a" is the first "b" is the second ext...
  va = Wire.read();
  vb = Wire.read();
  steps = a;
  steps = (steps << 8) | b;
  v = va;
  v = (v << 8) | vb;
  i=1;
}
