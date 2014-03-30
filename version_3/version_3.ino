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
const int ci_Right_Ultrasonic_Ping = A4;   //input plug
const int ci_Right_Ultrasonic_Data = 6;   //output plug

const int ci_Right_Ultrasonic_Ping = A1;   //input plug
const int ci_Right_Ultrasonic_Data = A0;   //output plug
const int ci_Back_Ultrasonic_Ping = A3;   //input plug
const int ci_Back_Ultrasonic_Data = A2;   //output plug

const int startbutton = 12;

unsigned long ul_Front_Echo_Time;
unsigned long ul_Right_Echo_Time;
unsigned long ul_Left_Echo_Time;
unsigned long ul_Back_Echo_Time;

unsigned int ui_Left_Motor_Speed=1500;
unsigned int ui_Right_Motor_Speed=1500;
unsigned int ui_Front_Motor_Speed=1500;
unsigned int ui_Back_Motor_Speed=1500;


unsigned long prev_time;

int location=1;
int destination=2;
int communication=0;
int final_destination=2;
int travel=1;

boolean reached = false;
int currentmicros = 1500;


void setup() 

{
  Serial.begin(9600);
  
  CharliePlexM::setBtn(13,13,13,13,13);     // Required to have Encoders working :(
  CharliePlexM::setEncoders(ci_Front_Encoder,ci_Back_Encoder);
  
  
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
  while (digitalRead(startbutton) == HIGH) {}
  prev_time = millis();
  while (millis() - prev_time < 3000) {};
    
}



void loop()
{
  
  boolean calibrated = false;
  long timer = millis();
  long leftmicros = 1800;
  long rightmicros = 1800;  
  while (!reached && CharliePlexM::ul_RightEncoder_Count < 7000) {
    

  
  // Back encoder not working :(
  
  Serial.println(" ");
      Serial.print("Encoder 1 =  ");
      Serial.println(CharliePlexM::ul_LeftEncoder_Count);
      Serial.print("Encoder 2 =  ");
      Serial.println(CharliePlexM::ul_RightEncoder_Count);
   
   if (millis() - timer > 1000) {
       timer = millis();
       calibrated = false;
   }
   
   initLeftEncoder = CharliePlexM::u1_LeftEncoder_Count;
   initRightEncoder = CharliePlexM::ul_RightEncoder_Count;

   Ping();
   
   if(Serial.available())
     Translation();
   
   if(location==1)&&(destination==2)
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
     else if(traval==3)
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
     else if(traval==3)
     {
       drive_backwards();
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
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
  if (ul_Front_Echo_Time/58 < 13) {
        servo_FrontMotor.writeMicroseconds(1500);
        servo_BackMotor.writeMicroseconds(1500);
        servo_RightMotor.writeMicroseconds(1500);
        servo_LeftMotor.writeMicroseconds(1500);
  }
  if (ul_Right_Echo_Time/58 < 21) {
        servo_FrontMotor.writeMicroseconds(1800);
        servo_BackMotor.writeMicroseconds(1800);
        servo_RightMotor.writeMicroseconds(1500);
        servo_LeftMotor.writeMicroseconds(1500);
        Ping();
  }
  else if (ul_Right_Echo_Time/58 > 24) {
        servo_FrontMotor.writeMicroseconds(1200);
        servo_BackMotor.writeMicroseconds(1200);
        servo_RightMotor.writeMicroseconds(1500);
        servo_LeftMotor.writeMicroseconds(1500);
  }
  else {
        
        if ((CharliePlexM::u1_LeftEncoder_Count - initLeftEncoder) > (CharliePlexM::ul_RightEncoder_Count - initRightEncoder) + 20) {
          if (!calibrated) {   
             rightmicros += 20; 
             leftmicros -= 20;
             calibrated = true;
               Serial.println(" ");
      Serial.print("Micros 1 =  ");
      Serial.println(CharliePlexM::ul_LeftEncoder_Count);
      Serial.print("Micros 2 =  ");
      Serial.println(CharliePlexM::ul_RightEncoder_Count);
          }
        } 
        
        else if ((CharliePlexM::u1_LeftEncoder_Count - initLeftEncoder) / (millis() - timer) + 20 < (CharliePlexM::ul_RightEncoder_Count - initRightEncoder)/(millis() - timer)) {
          if (!calibrated) {   
             rightmicros -= 20; 
             leftmicros += 20;
             calibrated = true;
          }
        } 
        
        servo_RightMotor.writeMicroseconds(rightmicros);
        servo_LeftMotor.writeMicroseconds(lefttmicros);
    }
  
  }
  
   servo_FrontMotor.writeMicroseconds(ui_Front_Motor_Speed);
   servo_BackMotor.writeMicroseconds(ui_Back_Motor_Speed);
   servo_RightMotor.writeMicroseconds(ui_Right_Motor_Speed);
   servo_LeftMotor.writeMicroseconds(ui_Left_Motor_Speed);
   
  /*
   if (CharliePlexM::ul_RightEncoder_Count > 500) {
     if (!reached) {
       currentmicros = 1500;
       servo_FrontMotor.writeMicroseconds(currentmicros);
       servo_BackMotor.writeMicroseconds(currentmicros);
       prev_time=millis();
       while((millis()-prev_time)<2000) {};
       reached = true;
     }
       currentmicros = 1500;
       servo_FrontMotor.writeMicroseconds(currentmicros);
       servo_BackMotor.writeMicroseconds(currentmicros);
   }
  
  
if (CharliePlexM::ul_RightEncoder_Count > 1000) {
         servo_FrontMotor.writeMicroseconds(1500);
       servo_BackMotor.writeMicroseconds(1500);
  while (true) {}
}


  //Ping();
/*
   prev_time=millis();
   while((millis()-prev_time)<2000)
   {
   servo_LeftMotor.writeMicroseconds(1800);
   servo_RightMotor.writeMicroseconds(1800);
   servo_FrontMotor.writeMicroseconds(1500);
   servo_BackMotor.writeMicroseconds(1500);
   }
   
   prev_time=millis();
   while((millis()-prev_time)<2000)
   {
   servo_LeftMotor.writeMicroseconds(1500);
   servo_RightMotor.writeMicroseconds(1500);
   servo_FrontMotor.writeMicroseconds(1800);
   servo_BackMotor.writeMicroseconds(1800);
   }
   
   prev_time=millis();
   while((millis()-prev_time)<2000)
   {
   servo_LeftMotor.writeMicroseconds(1200);
   servo_RightMotor.writeMicroseconds(1200);
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
   
 
 
}

  
  // RUNNING THE MOTOR
  /*
   long prevtime;
   prevtime=millis();
   while(1)
   {
   if((millis()-prevtime)<1000)
   { servo_LeftMotor.writeMicroseconds(1800);
    servo_RightMotor.writeMicroseconds(1800);
   }
   else if((millis()-prevtime)<2000)
   {
    servo_FrontMotor.writeMicroseconds(1800);
    servo_BackMotor.writeMicroseconds(1800);
   }
   else
   prevtime=millis();
   }
  */
void Translation()
{
  communication=Serial.read(); 
  if(communication==1)           // top sends signal to stop
  {
    location=destination;
    complete_stop();
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
  communication=0;  
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
  //Ping Ultrasonic
  //Send the Ultrasonic Range Finder a 10 microsecond pulse per tech spec
  digitalWrite(ci_Front_Ultrasonic_Ping, HIGH);
  delayMicroseconds(10);  //The 10 microsecond pause where the pulse in "high"
  digitalWrite(ci_Front_Ultrasonic_Ping, LOW);
  //use command pulseIn to listen to Ultrasonic_Data pin to record the
  //time that it takes from when the Pin goes HIGH until it goes LOW 
  ul_Front_Echo_Time = pulseIn(ci_Front_Ultrasonic_Data, HIGH, 10000);
  
  
  digitalWrite(ci_Right_Ultrasonic_Ping, HIGH); 
  delayMicroseconds(10);
  digitalWrite(ci_Right_Ultrasonic_Ping, LOW);
  ul_Right_Echo_Time = pulseIn(ci_Right_Ultrasonic_Data, HIGH, 10000);
  
  digitalWrite(ci_Left_Ultrasonic_Ping, HIGH); 
  delayMicroseconds(10);
  digitalWrite(ci_Left_Ultrasonic_Ping, LOW);
  ul_Left_Echo_Time = pulseIn(ci_Left_Ultrasonic_Data, HIGH, 10000);
  
  digitalWrite(ci_Back_Ultrasonic_Ping, HIGH); 
  delayMicroseconds(10);
  digitalWrite(ci_Back_Ultrasonic_Ping, LOW);
  ul_Back_Echo_Time = pulseIn(ci_Back_Ultrasonic_Data, HIGH, 10000);
  

  // Print Sensor Readings
  //#ifdef DEBUG_ULTRASONIC
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
//#endif
}  




