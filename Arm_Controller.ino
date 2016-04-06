#include <Wire.h>             // Include the required Wire library for I2C
#include <Servo.h>            // Include the required Servo Library to servo control

// define servo objects 

Servo myServo;
Servo claw_Servo1;
Servo claw_Servo2;
Servo claw_Servo3;

// Set initial positions for the claw

int claw_pos = 1400;
int claw_servoTrim1=100;
int claw_servoTrim2= 0;
int claw_servoTrim3=50;

bool claw_openClaw = false;
int claw_openPos = 1500;
int claw_closePos = 2250;
int claw_clawMoveDist = 0;
int claw_clawDelay = 25;

// store the pin numbers for the servo signal wire in a variable 

int claw_pin_1 = 51;
int claw_pin_2 = 52;
int claw_pin_3 = 53;
int wrist_servo_pin = 50;

// define the length of the arm and fore-arm used in the kinimatic equations

double L1=10;
double L2=10;

double scale = 10;

// initial arm positions

float x = .0001;
float y = 10;
float z = 10;

float x_last = .0001;
float y_last = 6;
float z_last = .0001;

// initial steps taken by the stepper motors to be at the inital position

// 1 - stepper motor controlling lower joint or the sholder
// 2 - stepper motor controlling upper joint or the elbow 
// 3 - stepper motor controlling the piviot of the table

int16_t steps1 = 278;
int16_t steps2 = 131;
int16_t steps3 = 0;

// set up desired velocity of the arm

int v = 20; // inches per second

float pi = 3.141592654;

double stepTime = 0;
double dx = 0;
double dz = 0;
double dy = 0;

////////////////////////////////////////////////////////////////////////////////////
// Diameters for the pullyes attached to the stepper motors 
// these are also used for calibrating 
////////////////////////////////////////////////////////////////////////////////////

float LG1=2.4125;     
float SG1=.434;

float LG2=.9795;     
float SG2=.375;

float LG3=5.167;      
float SG3=1.0655;

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

// rpms for each stepper motor 

float rpm1=10;
float rpm2=5;
float rpm3=20;

// define variables to store the angle each stepper motor needs to move to

double theta1=0;
double theta2=0;
double theta3=0;

// define variables to store the angle each stepper motors change in angle

double deltaTheta1 = 0;
double deltaTheta2 = 0;
double deltaTheta3 = 0;

// define values for servo trim

int servoTarget = 0;
int servoPos = 2320;
int servoTrim = 0;
bool servoStop = false;
int openClaw;

void setup() {
  // Start the I2C Bus as Master
  Wire.begin();  // start I2C communitation
  pinMode(13,OUTPUT);
  
  Serial.begin(9600); // start serial 
  // Serial.println("8");
  
  //set up objects for servos and move to the innirial position 
  myServo.attach(wrist_servo_pin);
  claw_Servo1.attach(claw_pin_1);
  claw_Servo2.attach(claw_pin_2);
  claw_Servo3.attach(claw_pin_3);
  claw_Servo1.writeMicroseconds(claw_pos+claw_servoTrim1);
  claw_Servo2.writeMicroseconds(claw_pos+claw_servoTrim2);
  claw_Servo3.writeMicroseconds(claw_pos+claw_servoTrim3);
  myServo.writeMicroseconds(claw_pos);
}
void loop() {
  //delay(3000);
  Serial.println("Ready for x:y:z:Claw ");  
  //Serial.println("ready");  
  Serial.println("");    
  while (Serial.available() == 0){} // wait untill data is sent from the serial connection then it moves on
  String temp = Serial.readString();
  Serial.println(temp);   

  // Parse data
  
  x = getValue(temp,':',0).toFloat();//toInt();
  y = getValue(temp,':',1).toFloat();//toInt();
  z = getValue(temp,':',2).toFloat();//toInt();
  if (getValue(temp,':',3).toFloat()==1){
    claw_openClaw = true;
  }else{
    claw_openClaw = false;
  }

  // correct divide by zero error
  if(y==0)y=.0001;
  if(z==0)z=.0001;


  // calculate the time required to move 
  stepTime = sqrt(pow(x-x_last,2)+pow(y-y_last,2)+pow(z-z_last,2))/v;
  
  theta1=Angle1(x,y,z);
  theta2=Angle2(x,y,z);
  theta3=Angle3(x,y,z);
  
  deltaTheta1=Angle1(x,y,z)-Angle1(x_last,y_last,z_last);
  deltaTheta2=Angle2(x,y,z)-Angle2(x_last,y_last,z_last);
  deltaTheta3=Angle3(x,y,z)-Angle3(x_last,y_last,z_last);

  rpm1=10;
  rpm2=5;
  rpm3=20;
  
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//****************************I2C Comms*******************************
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

  // calcualte the angle the sepper needs to move to
  // convert the angle to a number of stepps 
  
  int16_t bigNum3 = round(Angle3(x,y,z)*200/(2*pi)*LG3/SG3)-steps3;//angle*steps per rev/2 pi
  steps3 = steps3 + bigNum3;
  int16_t speed3 = rpm3;
  byte myArray3[3];

  //break down the 16 bit int into two 8 bit ints

  myArray3[2] = (speed3 >> 8) & 0xFF;
  myArray3[3] = speed3 & 0xFF;
  
  myArray3[0] = (bigNum3 >> 8) & 0xFF;
  myArray3[1] = bigNum3 & 0xFF;

  // send the data 

  Wire.beginTransmission(10);//sends instructions to the table stepper controller with speed and number of steps
  Wire.write(myArray3, 4);
  Wire.endTransmission();  
  
  int16_t bigNum2 = round(Angle2(x,y,z)*200/(2*pi)*LG2/SG2)-steps2;//angle*steps per rev/2 pi;
  steps2 = steps2 + bigNum2;
  int16_t speed2 = rpm2;
  byte myArray2[3];

  myArray2[2] = (speed2 >> 8) & 0xFF;
  myArray2[3] = speed2 & 0xFF;
  
  myArray2[0] = (bigNum2 >> 8) & 0xFF;
  myArray2[1] = bigNum2 & 0xFF;

  Wire.beginTransmission(9);//sends instructions to the elbow stepper controller with speed and number of steps
  Wire.write(myArray2, 4);
  Wire.endTransmission();  

  int16_t bigNum1 = round(Angle1(x,y,z)*200/(2*pi)*LG1/SG1)-steps1;//angle*steps per rev/2 pi
  steps1 = steps1 + bigNum1;
  int16_t speed1 = rpm1;//abs(bigNum)*rate;
  byte myArray1[3];
  
  myArray1[2] = (speed1 >> 8) & 0xFF;
  myArray1[3] = speed1 & 0xFF;
  
  myArray1[0] = (bigNum1 >> 8) & 0xFF;
  myArray1[1] = bigNum1 & 0xFF;
    
  Wire.beginTransmission(8);//sends instructions to the Sholder stepper controller with speed and number of steps
  Wire.write(myArray1, 4);
  Wire.endTransmission();  

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//****************************Servo Lim*******************************
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
  
  //Serial.println(Angle2(x,y,z)*180/pi);

  // find the angle the servo needs to move to
  servoTarget = abs(angleToServo(round(servoAngle2(x,y,z)*180/pi)));
  servoStop = false;

// move the servo controlling the claw
// increment the servos position by 20s so the servo dosent move to fast
  
  while(servoStop==false){
    if ((servoPos-servoTarget)>0){
      servoPos-=20;
    }
    else{
      servoPos+=20;
    }
    myServo.writeMicroseconds(servoPos);
    if (abs(servoPos-servoTarget)<30) servoStop=true;
    delay(15);
  }
  servoPos=servoTarget;
  myServo.writeMicroseconds(servoPos);

//
  
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//****************************Open or Close Claw*******************************
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

// wait untill the arm has compleated the move.
delay(stepTime*1000);

// if the claw needs to be open, increment the servos position to the open opsition or the closed position 

if (claw_openClaw) {
  claw_clawMoveDist = claw_openPos-claw_pos;
  while (abs(claw_clawMoveDist)>30){
    delay(claw_clawDelay);
  claw_clawMoveDist = claw_openPos-claw_pos;
    if (claw_clawMoveDist>0){
      claw_pos+=10;
    }
    else{
      claw_pos-=10;
    }
  claw_Servo1.writeMicroseconds(claw_pos+claw_servoTrim1);
  claw_Servo2.writeMicroseconds(claw_pos+claw_servoTrim2);
  claw_Servo3.writeMicroseconds(claw_pos+claw_servoTrim3);
  }
  claw_pos=claw_openPos;
}else{
  claw_clawMoveDist = claw_pos-claw_closePos;
  while (abs(claw_clawMoveDist)>30){
    delay(claw_clawDelay);
   claw_clawMoveDist = claw_closePos-claw_pos;
    if (claw_clawMoveDist>0){
      claw_pos+=10;
    }
    else{
      claw_pos-=10;
    }
    
  claw_Servo1.writeMicroseconds(claw_pos+claw_servoTrim1);
  claw_Servo2.writeMicroseconds(claw_pos+claw_servoTrim2);
  claw_Servo3.writeMicroseconds(claw_pos+claw_servoTrim3);
  }
  claw_pos=claw_closePos;
} 
  claw_Servo1.writeMicroseconds(claw_pos+claw_servoTrim1);
  claw_Servo2.writeMicroseconds(claw_pos+claw_servoTrim2);
  claw_Servo3.writeMicroseconds(claw_pos+claw_servoTrim3);


// print values to the serial for debugging 
//
//  Serial.println("Servo:");
//  Serial.print("servoPos: ");Serial.println(servoPos);
//  Serial.print("servoTarget: ");Serial.println(servoTarget);
//  Serial.println(" ");
//
//  Serial.println("Moving to:");
//  Serial.print("x: ");Serial.println(x);
//  Serial.print("y: ");Serial.println(y);
//  Serial.print("z: ");Serial.println(z);
//
//  Serial.println("From:");
//  Serial.print("x: ");Serial.println(x_last);
//  Serial.print("y: ");Serial.println(y_last);
//  Serial.print("z: ");Serial.println(z_last);
//  Serial.println(" ");
//
//  Serial.println("Thetas:");
//  Serial.print("1: ");Serial.println(theta1*180/pi);
//  Serial.print("2: ");Serial.println(theta2*180/pi);
//  Serial.print("3: ");Serial.println(theta3*180/pi);
//  Serial.println(" ");
//
//  
//  Serial.println("D Thetas:");
//  Serial.print("1: ");Serial.println(deltaTheta1);
//  Serial.print("2: ");Serial.println(deltaTheta2);
//  Serial.print("3: ");Serial.println(deltaTheta3);
//  Serial.println(" ");

  
//  Serial.println("RPMs:");
//  Serial.print("1: ");Serial.println(rpm1);
//  Serial.print("2: ");Serial.println(rpm2);
//  Serial.print("3: ");Serial.println(rpm3);
//  Serial.println(" ");
//
//  Serial.println("steps");
//  Serial.print("1: ");Serial.println(steps1);
//  Serial.print("2: ");Serial.println(steps2);
//  Serial.print("3: ");Serial.println(steps3);
//  Serial.println(" ");
//  
//  Serial.println("time");
//  Serial.println(stepTime);
//  Serial.println(" ");
//
//  Serial.println("Angle");
//  Serial.print("Sholder: ");
//  Serial.println(Angle1(x,y,z)*180./pi);
//  Serial.print("Elbow:   ");
//  Serial.println(Angle2(x,y,z)*180./pi);
//  Serial.print("Table:   ");
//  Serial.println(Angle3(x,y,z)*180./pi);
//  
//  Serial.println(" ");
//  Serial.println(" ");

// store the curent x,y,z value as the last x,y,z
  
  x_last=x;
  y_last=y;
  z_last=z;
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//******Functions used to convert the desired x,y,z to angles for the steppers********//
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

double Angle1(double x,double y,double z){ //angle between the chess board plane and the first part of the arm
  double Angle1_a=atan((L1*(pow(L1,2)*sqrt(pow(x,2)+pow(y,2))*pow(z,2)-pow(L2,2)*sqrt(pow(x,2)+pow(y,2))*pow(z,2)+pow(x,2)*sqrt(pow(x,2)+pow(y,2))*pow(z,2)+pow(y,2)*sqrt(pow(x,2)+pow(y,2))*pow(z,2)+sqrt(pow(x,2)+pow(y,2))*pow(z,4)+pow(x,2)*sqrt(-pow(z,2)*(pow(L1,4)+pow((-pow(L2,2)+pow(x,2)+pow(y,2)+pow(z,2)),2)-2*pow(L1,2)*(pow(L2,2)+pow(x,2)+pow(y,2)+pow(z,2))))+pow(y,2)*sqrt(-pow(z,2)*(pow(L1,4)+pow((-pow(L2,2)+pow(x,2)+pow(y,2)+pow(z,2)),2)-2*pow(L1,2)*(pow(L2,2)+pow(x,2)+pow(y,2)+pow(z,2))))))/(z*(pow(L1,3)*(pow(x,2)+pow(y,2))+L1*(pow(x,2) + pow(y,2))*(-pow(L2,2)+pow(x,2)+pow(y,2)+pow(z,2))-L1*sqrt(pow(x,2)+pow(y,2))*sqrt(-pow(z,2)*(pow(L1,4)+pow((-pow(L2,2)+pow(x,2)+pow(y,2)+pow(z,2)),2)-2*pow(L1,2)*(pow(L2,2)+pow(x,2)+pow(y,2)+pow(z,2)))))));
  if (Angle1_a<0){
    return Angle1_a+3.141;
  }
  else {
    return Angle1_a;
  }   
}

double Angle2(double x,double y,double z){ //angle between the first part of the arm and the seccond part of the arm
  double Angle2_a=atan((z*(-(pow(L1,3)*(pow(x,2) + pow(y,2))) + L1*sqrt(pow(x,2) + pow(y,2))*sqrt((-pow(z,2))*(pow(L1,4) - 2*pow(L1,2)*(pow(L2,2) + pow(x,2) + pow(y,2) + pow(z,2))+pow((-pow(L2,2) + pow(x,2) + pow(y,2) + pow(z,2)),2))) + L1*(pow(x,2) + pow(y,2))*(pow(L2,2) + pow(x,2) + pow(y,2) + pow(z,2))))/(L1*(pow(L1,2)*pow(z,2)*sqrt(pow(x,2) + pow(y,2)) + pow(x,2)*sqrt((-pow(z,2))*(pow(L1,4) - 2*pow(L1,2)*(pow(L2,2) + pow(x,2) + pow(y,2) + pow(z,2)) +pow((-pow(L2,2) + pow(x,2) + pow(y,2) + pow(z,2)),2))) + pow(y,2)*sqrt((-pow(z,2))*(pow(L1,4) - 2*pow(L1,2)*(pow(L2,2) + pow(x,2) + pow(y,2) + pow(z,2)) + pow((-pow(L2,2) + pow(x,2) + pow(y,2) + pow(z,2)),2))) - pow(L2,2)*pow(z,2)*sqrt(pow(x,2) + pow(y,2)) + pow(z,4)*(-sqrt(pow(x,2) + pow(y,2))) - pow(x,2)*pow(z,2)*sqrt(pow(x,2) + pow(y,2)) - pow(y,2)*pow(z,2)*sqrt(pow(x,2) + pow(y,2)))));
  if (Angle2_a>0){
    return  1.5708 - Angle1(x,y,z) + Angle2_a;
  }
  else {
    return 1.5708 - Angle1(x,y,z) + Angle2_a + 3.141;
  }    
}
double servoAngle2(double x,double y,double z){ //angle between the first part of the arm and the seccond part of the arm
  double Angle2_a=atan((z*(-(pow(L1,3)*(pow(x,2) + pow(y,2))) + L1*sqrt(pow(x,2) + pow(y,2))*sqrt((-pow(z,2))*(pow(L1,4) - 2*pow(L1,2)*(pow(L2,2) + pow(x,2) + pow(y,2) + pow(z,2))+pow((-pow(L2,2) + pow(x,2) + pow(y,2) + pow(z,2)),2))) + L1*(pow(x,2) + pow(y,2))*(pow(L2,2) + pow(x,2) + pow(y,2) + pow(z,2))))/(L1*(pow(L1,2)*pow(z,2)*sqrt(pow(x,2) + pow(y,2)) + pow(x,2)*sqrt((-pow(z,2))*(pow(L1,4) - 2*pow(L1,2)*(pow(L2,2) + pow(x,2) + pow(y,2) + pow(z,2)) +pow((-pow(L2,2) + pow(x,2) + pow(y,2) + pow(z,2)),2))) + pow(y,2)*sqrt((-pow(z,2))*(pow(L1,4) - 2*pow(L1,2)*(pow(L2,2) + pow(x,2) + pow(y,2) + pow(z,2)) + pow((-pow(L2,2) + pow(x,2) + pow(y,2) + pow(z,2)),2))) - pow(L2,2)*pow(z,2)*sqrt(pow(x,2) + pow(y,2)) + pow(z,4)*(-sqrt(pow(x,2) + pow(y,2))) - pow(x,2)*pow(z,2)*sqrt(pow(x,2) + pow(y,2)) - pow(y,2)*pow(z,2)*sqrt(pow(x,2) + pow(y,2)))));
  if (Angle2_a>0){
    return Angle2_a+ pi/2.;
  }
  else {
    return Angle2_a + pi+ pi/2.;
  }    
}

double Angle3(double x,double y,double z){ // angle of the twist for the robot arm. 0 at the center
  return atan(x/y);
}

//funcion used for parsing 

String getValue(String data, char separator, int index)
{
 int found = 0;
  int strIndex[] = {
0, -1  };
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
  if(data.charAt(i)==separator || i==maxIndex){
  found++;
  strIndex[0] = strIndex[1]+1;
  strIndex[1] = (i == maxIndex) ? i+1 : i;
  }
 }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// function for converting an angle in degrees to milliseconds

int angleToServo(int a){
  return map(a,90,180,1400,2320);
}
