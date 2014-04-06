/*

 MSE 2202 Project
 Language: Arduino
 
 
 */
 
 #include <Servo.h>
 #include <EEPROM.h>
 #include <uSTimer2.h>
 #include <CharliePlexM.h>
 
Servo servo_LeftMotor;
Servo servo_RightMotor;
Servo servo_FrontMotor;
Servo servo_BackMotor;

Servo servo_PlatformMotor;

void mydelay(long time) {
  long current = millis();
  	while (millis() - current < time) {};
}

boolean bt_Motors_Enabled = true;


//port pin constants

const int ci_Left_Motor = 11;
const int ci_Right_Motor = 9;
const int ci_Front_Motor = 8;
const int ci_Back_Motor = 10;

const int ci_Left_Encoder = 5;
const int ci_Right_Encoder = 3;
const int ci_Front_Encoder = 2;
const int ci_Back_Encoder = 4;

const int ci_Front_Ultrasonic_Ping = A5;   //input plug
const int ci_Front_Ultrasonic_Data = 7;   //output plug
const int ci_Left_Ultrasonic_Ping = A4;   //input plug
const int ci_Left_Ultrasonic_Data = 6;   //output plug

const int ci_Right_Ultrasonic_Ping = A1;   //input plug
const int ci_Right_Ultrasonic_Data = A0;   //output plug
const int ci_Back_Ultrasonic_Ping = A3;   //input plug
const int ci_Back_Ultrasonic_Data = A2;   //output plug

const int startbutton = 12;

unsigned long ul_Front_Echo_Time=0;
unsigned long ul_Right_Echo_Time=0;
unsigned long ul_Left_Echo_Time=0;
unsigned long ul_Back_Echo_Time=0;

unsigned long ul_Prev_Front_Echo_Time;
unsigned long ul_Prev_Right_Echo_Time;
unsigned long ul_Prev_Left_Echo_Time;
unsigned long ul_Prev_Back_Echo_Time;

unsigned int ui_Left_Motor_Speed=1500;
unsigned int ui_Right_Motor_Speed=1500;
unsigned int ui_Front_Motor_Speed=1500;
unsigned int ui_Back_Motor_Speed=1500;

unsigned long ul_init_LeftEncoder;
unsigned long ul_init_RightEncoder;
unsigned long ul_init_FrontEncoder;
unsigned long ul_init_BackEncoder;


unsigned long prev_time;

int location=1;
int destination=2;
int communication=0;
int final_destination=2;
int travel=1;                      //tracks the hallway we are in
int prev_distance=0;               //tracks the previous distance of the left
int go=1;

boolean reached = false;


void setup() 

{
  Serial.begin(9600);

 CharliePlexM::setEncoders(ci_Left_Encoder, ci_Right_Encoder, ci_Front_Encoder,ci_Back_Encoder);
 
 
ul_init_LeftEncoder = CharliePlexM::ul_LeftEncoder_Count;
ul_init_RightEncoder = CharliePlexM::ul_RightEncoder_Count;
ul_init_FrontEncoder = CharliePlexM::ul_Encoder3_Count;
ul_init_BackEncoder = CharliePlexM::ul_Encoder4_Count;
    
    
  // set up ultrasonic
  pinMode(ci_Front_Ultrasonic_Ping, OUTPUT);
  pinMode(ci_Front_Ultrasonic_Data, INPUT);
  pinMode(ci_Right_Ultrasonic_Ping, OUTPUT);
  pinMode(ci_Right_Ultrasonic_Data, INPUT);
  pinMode(ci_Back_Ultrasonic_Ping, OUTPUT);
  pinMode(ci_Back_Ultrasonic_Data, INPUT);
  pinMode(ci_Left_Ultrasonic_Ping, OUTPUT);
  pinMode(ci_Left_Ultrasonic_Data, INPUT);


  // set up drive motors
  pinMode(ci_Left_Motor, OUTPUT);
  servo_LeftMotor.attach(ci_Left_Motor);
  pinMode(ci_Right_Motor, OUTPUT);
  servo_RightMotor.attach(ci_Right_Motor);
  pinMode(ci_Front_Motor, OUTPUT);
  servo_FrontMotor.attach(ci_Front_Motor);
  pinMode(ci_Back_Motor, OUTPUT);
  servo_BackMotor.attach(ci_Back_Motor);

                            
  pinMode(ci_Left_Encoder, INPUT);
  pinMode(ci_Right_Encoder, INPUT);
  pinMode(ci_Back_Encoder, INPUT);
  pinMode(ci_Front_Encoder, INPUT);
 
 // Declare start push button 4 
  pinMode(startbutton, INPUT);
  digitalWrite(startbutton, HIGH);
  
  // Delay to allow time for the robot to start motion
/*  while (digitalRead(startbutton) == HIGH) {}
  prev_time = millis();
  while (millis() - prev_time < 3000) {};
  */
    
}



void loop()
{
  
   Ping();
   if(Serial.available())
     Translation();
   
 if(go==1)
 {
   if((location==1)&&(destination==2))
   {
     drive_forward();
     //after exiting the while loop, should stop the motor
   }
   else if((location==2)&&(destination==1))
   {
     drive_backward();     
   }
   
   else if((location==2)&&(destination==3))
   {
     if(travel==1) // we are at hallway 1
     {
       drive_forward();
       check_doors();  
     }
     else if(travel==2)
     {
       move_right();      //after ultral sonic find the wall, it changes travel from 2 to 3
     }
     else if(travel==3)
     {
       drive_right();
     }
   }
   
   else if((location==3)&&(destination==2))
   {
     if(travel==3) // we are at hallway 1
     {
       drive_left();
       check_wall();     //when it loses the wall, change travel to 2   
     }
     else if(travel==2)
     {
       move_left();      //after ultral sonic find the wall, it changes travel from 2 to 1
     }
     else if(travel==3)
     {
       drive_backward();
     }  
   }

   else if(location==destination)
   {
     if(destination!=final_destination)
     {
       destination=final_destination;
     }
   }
   
   // write the motor speed
   servo_FrontMotor.writeMicroseconds(ui_Front_Motor_Speed);
   servo_BackMotor.writeMicroseconds(ui_Back_Motor_Speed);
   servo_RightMotor.writeMicroseconds(ui_Right_Motor_Speed);
   servo_LeftMotor.writeMicroseconds(ui_Left_Motor_Speed);
 }
}

void drive_forward()
{
   if ((ul_Front_Echo_Time/58 < 13)&&(ul_Prev_Front_Echo_Time/58 < 13))
   {
     complete_stop();
   }
   else if ((ul_Left_Echo_Time/58 < 19)&&(ul_Prev_Left_Echo_Time/58 < 19))     //too close, move right
   {
     ui_Left_Motor_Speed=1500;
     ui_Right_Motor_Speed=1500;
     ui_Front_Motor_Speed=2000;
     ui_Back_Motor_Speed=2000;
   }
   else if ((ul_Left_Echo_Time/58 > 26)&&(ul_Prev_Left_Echo_Time/58 > 26))     //too far, move left
   {
     ui_Left_Motor_Speed=1500;
     ui_Right_Motor_Speed=1500;
     ui_Front_Motor_Speed=1000;
     ui_Back_Motor_Speed=1000;
   }
   else 
   {
     ui_Left_Motor_Speed=1950;
     ui_Right_Motor_Speed=2000;
     ui_Front_Motor_Speed=1500;
     ui_Back_Motor_Speed=1500; 
   }
   servo_FrontMotor.writeMicroseconds(ui_Front_Motor_Speed);
   servo_BackMotor.writeMicroseconds(ui_Back_Motor_Speed);
   servo_RightMotor.writeMicroseconds(ui_Right_Motor_Speed);
   servo_LeftMotor.writeMicroseconds(ui_Left_Motor_Speed);
}

void drive_backward()
{
   if (ul_Back_Echo_Time/58 < 13)
   {
     complete_stop();
   }
   else if (ul_Left_Echo_Time/58 < 20)     //too close, move right
   {
     ui_Left_Motor_Speed=1500;
     ui_Right_Motor_Speed=1500;
     ui_Front_Motor_Speed=2000;
     ui_Back_Motor_Speed=2000;
   }
   else if (ul_Left_Echo_Time/58 > 25)     //too far, move left
   {
     ui_Left_Motor_Speed=1500;
     ui_Right_Motor_Speed=1500;
     ui_Front_Motor_Speed=1000;
     ui_Back_Motor_Speed=1000;
   }
   else 
   {
     ui_Left_Motor_Speed=1000;
     ui_Right_Motor_Speed=1000;
     ui_Front_Motor_Speed=1500;
     ui_Back_Motor_Speed=1500; 
   }
}

void drive_right()
{
   if (ul_Right_Echo_Time/58 < 13)
   {
     complete_stop();
   }
   else if (ul_Front_Echo_Time/58 < 21)     //too close, move right
   {
     ui_Left_Motor_Speed=1000;
     ui_Right_Motor_Speed=1000;
     ui_Front_Motor_Speed=1500;
     ui_Back_Motor_Speed=1500;
   }
   else if (ul_Front_Echo_Time/58 > 24)     //too far, move left
   {
     ui_Left_Motor_Speed=2000;
     ui_Right_Motor_Speed=2000;
     ui_Front_Motor_Speed=1500;
     ui_Back_Motor_Speed=1500;
   }
   else 
   {
     ui_Left_Motor_Speed=1500;
     ui_Right_Motor_Speed=1500;
     ui_Front_Motor_Speed=2000;
     ui_Back_Motor_Speed=2000; 
   }
}

void drive_left()
{
   if (ul_Left_Echo_Time/58 < 13)
   {
     complete_stop();
   }
   else if (ul_Front_Echo_Time/58 < 21)     //too close, move right
   {
     ui_Left_Motor_Speed=1000;
     ui_Right_Motor_Speed=1000;
     ui_Front_Motor_Speed=1500;
     ui_Back_Motor_Speed=1500;
   }
   else if (ul_Front_Echo_Time/58 > 24)     //too far, move left
   {
     ui_Left_Motor_Speed=2000;
     ui_Right_Motor_Speed=2000;
     ui_Front_Motor_Speed=1500;
     ui_Back_Motor_Speed=1500;
   }
   else
   {
     ui_Left_Motor_Speed=1500;
     ui_Right_Motor_Speed=1500;
     ui_Front_Motor_Speed=1000;
     ui_Back_Motor_Speed=1000; 
   }
}

void move_right()
{
   ui_Left_Motor_Speed=1500;
   ui_Right_Motor_Speed=1500;
   ui_Front_Motor_Speed=2000;
   ui_Back_Motor_Speed=2000;
   
   if(ul_Front_Echo_Time/58<30)
   travel=3;
}

void move_left()
{
   ui_Left_Motor_Speed=1500;
   ui_Right_Motor_Speed=1500;
   ui_Front_Motor_Speed=1000;
   ui_Back_Motor_Speed=1000;
   
   if(ul_Left_Echo_Time/58<25)
   travel=1;
}

void check_wall()
{
  if(ul_Front_Echo_Time/58>30)
  travel=2;
}

void check_doors()
{
  if((prev_distance-ul_Left_Echo_Time)>232)       //232=4*58, 4cm
    travel=2;
  prev_distance=ul_Left_Echo_Time/58;
}


void Translation()
{
  communication=Serial.read(); 
  if(communication==1)           // top sends signal to stop
  {
    location=destination;
    complete_stop();
    go=0;
  }
  else if(communication==11)     // top sends signal for the change in destination
  { 
    final_destination=1;
    if(location==3)
    destination=2;  
    else
    destination=1;
  }
  else if(communication==12)
  {
    destination=2;
  }
  else if(communication==13)
  {
    final_destination=3;
    if(location==1)
    destination=2;  
    else
    destination=3;
  }
  else if(communication==66)
    command_mid_to_out();  
  else if(communication==44)
    command_mid_to_in();
  else if(communication==88)
    command_in_to_out();
  else if(communication==100)     //move from location to destination
    go=1;
  communication=0;
}

void command_mid_to_out()
{
  if(location==3)
  {
    command_right(225);
    command_forward(100);       //???????????????????
    mydelay(5000);
    command_backward(100);
    //drive right for ...
    //drive forward for ...
    //mydelay(5000);
    //drive backward for...
  }
  else
  {
    command_forward(225);
    command_left(100);       //???????????????????
    mydelay(5000);
    command_right(100);
    //drive forward for ...
    //drive left for ...
    //mydelay(5000);
    //drive right for ...
  }
}

void command_mid_to_in()
{
  if(location==3)
  {
    command_left(225);
    command_forward(100);       //???????????????????
    mydelay(5000);
    command_backward(100);
    //drive left for ...
    //drive forward for ...
    //mydelay(5000);
    //drive back for...
  }
  else
  {
    command_backward(225);
    command_left(100);       //???????????????????
    mydelay(5000);
    command_right(100);    
    //drive back for ...
    //drive left for ...
    //mydelay(5000);
    //drive right for ...
  }
}

void command_in_to_out()
{
  if(location==3)
  {
    command_right(450);
    command_forward(100);       //???????????????????
    mydelay(5000);
    command_backward(100);
    //drive right for ...
    //drive forward for ...
    //mydelay(5000);
    //drive back for..
  }
  else
  {command_backward(450);
    command_left(100);       //???????????????????
    mydelay(5000);
    command_right(100);
    //drive back for ...
    //drive left for ...
    //mydelay(5000);
    //drive right for...
  }
}

void command_forward(long ticks)
{
//asdasdfasdfsdf

// make it move forward by (ticks) ticks
  ui_Left_Motor_Speed = 2000;
  ui_Right_Motor_Speed = 2000;     
 
  unsigned long ul_initRightEncoder = CharliePlexM::ul_RightEncoder_Count;
 
  while (CharliePlexM::ul_RightEncoder_Count - ul_initRightEncoder < ticks) {
   

        servo_RightMotor.writeMicroseconds(ui_Right_Motor_Speed);
        servo_LeftMotor.writeMicroseconds(ui_Left_Motor_Speed);
        servo_FrontMotor.writeMicroseconds(1500);
        servo_BackMotor.writeMicroseconds(1500);    
 }
      
     

  
  //Stop the motor at this stage
        servo_FrontMotor.writeMicroseconds(1500);
        servo_BackMotor.writeMicroseconds(1500);
        servo_RightMotor.writeMicroseconds(1500);
        servo_LeftMotor.writeMicroseconds(1500);
  
//  Serial.print("Right Motor Speed: \t");
//  Serial.println(ui_Right_Motor_Speed);
//  Serial.print("Left Motor Speed: \t");
//  Serial.println(ui_Left_Motor_Speed);
// 
//


}


void command_backward(long ticks)
{
//asdasdfasdfsdf

// make it move forward by (ticks) ticks
  ui_Left_Motor_Speed = 1000;
  ui_Right_Motor_Speed = 1000;     
  
  unsigned long ul_initRightEncoder = CharliePlexM::ul_RightEncoder_Count;
 
  while (CharliePlexM::ul_RightEncoder_Count - ul_initRightEncoder < ticks) {
    
   

        servo_RightMotor.writeMicroseconds(ui_Right_Motor_Speed);
        servo_LeftMotor.writeMicroseconds(ui_Left_Motor_Speed);
        servo_FrontMotor.writeMicroseconds(1500);
        servo_BackMotor.writeMicroseconds(1500);    
 }
      
     

  
  //Stop the motor at this stage
        servo_FrontMotor.writeMicroseconds(1500);
        servo_BackMotor.writeMicroseconds(1500);
        servo_RightMotor.writeMicroseconds(1500);
        servo_LeftMotor.writeMicroseconds(1500);
  
//  Serial.print("Right Motor Speed: \t");
//  Serial.println(ui_Right_Motor_Speed);
//  Serial.print("Left Motor Speed: \t");
//  Serial.println(ui_Left_Motor_Speed);
// 
//


}

void command_right(long ticks)
{

// make it move forward by (ticks) ticks
  ui_Front_Motor_Speed = 2000;
  ui_Back_Motor_Speed = 2000;     
 
 unsigned long ul_initFrontEncoder = CharliePlexM::ul_Encoder3_Count;
 
   // Check the front encoder increment
  while (CharliePlexM::ul_Encoder3_Count - ul_initFrontEncoder < ticks) {
    
   

        servo_RightMotor.writeMicroseconds(1500);
        servo_LeftMotor.writeMicroseconds(1500);
        servo_FrontMotor.writeMicroseconds(ui_Front_Motor_Speed);
        servo_BackMotor.writeMicroseconds(ui_Back_Motor_Speed);    
 }
      
     

  
  //Stop the motor at this stage
        servo_FrontMotor.writeMicroseconds(1500);
        servo_BackMotor.writeMicroseconds(1500);
        servo_RightMotor.writeMicroseconds(1500);
        servo_LeftMotor.writeMicroseconds(1500);
  
//  Serial.print("Right Motor Speed: \t");
//  Serial.println(ui_Right_Motor_Speed);
//  Serial.print("Left Motor Speed: \t");
//  Serial.println(ui_Left_Motor_Speed);
// 
//


}

void command_left(long ticks)
{

// make it move forward by (ticks) ticks
  ui_Front_Motor_Speed = 1000;
  ui_Back_Motor_Speed = 1000;     
 
 unsigned long ul_initFrontEncoder = CharliePlexM::ul_Encoder3_Count;
 
   // Check the front encoder increment
  while (CharliePlexM::ul_Encoder3_Count - ul_initFrontEncoder < ticks) {

        servo_RightMotor.writeMicroseconds(1500);
        servo_LeftMotor.writeMicroseconds(1500);
        servo_FrontMotor.writeMicroseconds(ui_Front_Motor_Speed);
        servo_BackMotor.writeMicroseconds(ui_Back_Motor_Speed);    
 }
      
     

  
  //Stop the motor at this stage
        servo_FrontMotor.writeMicroseconds(1500);
        servo_BackMotor.writeMicroseconds(1500);
        servo_RightMotor.writeMicroseconds(1500);
        servo_LeftMotor.writeMicroseconds(1500);
  
//  Serial.print("Right Motor Speed: \t");
//  Serial.println(ui_Right_Motor_Speed);
//  Serial.print("Left Motor Speed: \t");
//  Serial.println(ui_Left_Motor_Speed);
// 
//


}



void complete_stop()
{
    ui_Left_Motor_Speed=1500;
    ui_Right_Motor_Speed=1500;
    ui_Front_Motor_Speed=1500;
    ui_Back_Motor_Speed=1500;
  
   servo_FrontMotor.writeMicroseconds(ui_Front_Motor_Speed);
   servo_BackMotor.writeMicroseconds(ui_Back_Motor_Speed);
   servo_RightMotor.writeMicroseconds(ui_Right_Motor_Speed);
   servo_LeftMotor.writeMicroseconds(ui_Left_Motor_Speed);
}


void Ping()
{
  ul_Prev_Front_Echo_Time=ul_Front_Echo_Time;
  ul_Prev_Right_Echo_Time=ul_Right_Echo_Time;
  ul_Prev_Left_Echo_Time=ul_Left_Echo_Time;
  ul_Prev_Back_Echo_Time=ul_Left_Echo_Time;
  //Ping Ultrasonic
  //Send the Ultrasonic Range Finder a 10 microsecond pulse per tech spec
  digitalWrite(ci_Front_Ultrasonic_Ping, HIGH);
  delayMicroseconds(10);  //The 10 microsecond pause where the pulse in "high"
  digitalWrite(ci_Front_Ultrasonic_Ping, LOW);
  //use command pulseIn to listen to Ultrasonic_Data pin to record the
  //time that it takes from when the Pin goes HIGH until it goes LOW 
  ul_Front_Echo_Time = pulseIn(ci_Front_Ultrasonic_Data, HIGH, 5000);
  
  
  digitalWrite(ci_Right_Ultrasonic_Ping, HIGH); 
  delayMicroseconds(10);
  digitalWrite(ci_Right_Ultrasonic_Ping, LOW);
  ul_Right_Echo_Time = pulseIn(ci_Right_Ultrasonic_Data, HIGH, 5000);
  
  
  digitalWrite(ci_Left_Ultrasonic_Ping, HIGH); 
  delayMicroseconds(10);
  digitalWrite(ci_Left_Ultrasonic_Ping, LOW);
  ul_Left_Echo_Time = pulseIn(ci_Left_Ultrasonic_Data, HIGH, 5000);
  
  digitalWrite(ci_Back_Ultrasonic_Ping, HIGH); 
  delayMicroseconds(10);
  digitalWrite(ci_Back_Ultrasonic_Ping, LOW);
  ul_Back_Echo_Time = pulseIn(ci_Back_Ultrasonic_Data, HIGH, 5000);
  

  // Print Sensor Readings
  //#ifdef DEBUG_ULTRASONIC
  /*
  Serial.print("FRONT");
  Serial.print("Time (microseconds): ");
  Serial.print(ul_Front_Echo_Time, DEC);
  Serial.print(", Inches: ");
  Serial.print(ul_Front_Echo_Time/148); //divide time by 148 to get distance in inches
  Serial.print(", cm: ");
  Serial.println(ul_Front_Echo_Time/58); //divide time by 58 to get distance in cm 

  Serial.print("RIGHT");
  Serial.print("Time (microseconds): ");
  Serial.print(ul_Right_Echo_Time, DEC);
  Serial.print(", Inches: ");
  Serial.print(ul_Right_Echo_Time/148); //divide time by 148 to get distance in inches
  Serial.print(", cm: ");
  Serial.println(ul_Right_Echo_Time/58);
  
  Serial.print("LEFT");
  Serial.print("Time (microseconds): ");
  Serial.print(ul_Left_Echo_Time, DEC);
  Serial.print(", Inches: ");
  Serial.print(ul_Left_Echo_Time/148); //divide time by 148 to get distance in inches
  Serial.print(", cm: ");
  Serial.println(ul_Left_Echo_Time/58);
  
  Serial.print("BACK");
  Serial.print("Time (microseconds): ");
  Serial.print(ul_Back_Echo_Time, DEC);
  Serial.print(", Inches: ");
  Serial.print(ul_Back_Echo_Time/148); //divide time by 148 to get distance in inches
  Serial.print(", cm: ");
  Serial.println(ul_Back_Echo_Time/58);
  
  Serial.println();
  Serial.println();
  */
//#endif
}


  //Ping();
/*
   prev_time=millis();
   while((millis()-prev_time)<2000)
   {
   servo_LeftMotor.writeMicroseconds(2000);
   servo_RightMotor.writeMicroseconds(2000);
   servo_FrontMotor.writeMicroseconds(1500);
   servo_BackMotor.writeMicroseconds(1500);
   }
   
   prev_time=millis();
   while((millis()-prev_time)<2000)
   {
   servo_LeftMotor.writeMicroseconds(1500);
   servo_RightMotor.writeMicroseconds(1500);
   servo_FrontMotor.writeMicroseconds(2000);
   servo_BackMotor.writeMicroseconds(2000);
   }
   
   prev_time=millis();
   while((millis()-prev_time)<2000)
   {
   servo_LeftMotor.writeMicroseconds(500);
   servo_RightMotor.writeMicroseconds(500);
   servo_FrontMotor.writeMicroseconds(1500);
   servo_BackMotor.writeMicroseconds(1500);
   }
   
   prev_time=millis();
   while((millis()-prev_time)<2000)
   {
   servo_LeftMotor.writeMicroseconds(1500);
   servo_RightMotor.writeMicroseconds(1500);
   servo_FrontMotor.writeMicroseconds(1200);
   servo_BackMotor.writeMicroseconds(1200);
   }

   
  */
   
 
 


  
  // RUNNING THE MOTOR
  /*
   long prevtime;
   prevtime=millis();
   while(1)
   {
   if((millis()-prevtime)<500)
   { servo_LeftMotor.writeMicroseconds(2000);
    servo_RightMotor.writeMicroseconds(2000);
   }
   else if((millis()-prevtime)<2000)
   {
    servo_FrontMotor.writeMicroseconds(2000);
    servo_BackMotor.writeMicroseconds(2000);
   }
   else
   prevtime=millis();
   }
  */



