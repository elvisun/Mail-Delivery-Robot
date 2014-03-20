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

const int startbutton = 12;

unsigned long ul_Front_Echo_Time;
unsigned long ul_Right_Echo_Time;

unsigned long prev_time;


void setup() {
  Serial.begin(9600);
  
  CharliePlexM::setBtn(13,13,13,13,13);     // Required to have Encoders working :(
  CharliePlexM::setEncoders(ci_Front_Encoder,ci_Back_Encoder);
  
  
  // set up ultrasonic
  pinMode(ci_Front_Ultrasonic_Ping, OUTPUT);
  pinMode(ci_Front_Ultrasonic_Data, INPUT);
  pinMode(ci_Right_Ultrasonic_Ping, OUTPUT);
  pinMode(ci_Right_Ultrasonic_Data, INPUT);



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

boolean reached = false;
int currentmicros = 1500;
void loop()
{

  
 //if (!reached && CharliePlexM::ul_RightEncoder_Count < 7000) {
    currentmicros = 1800;
  servo_RightMotor.writeMicroseconds(currentmicros);
  servo_LeftMotor.writeMicroseconds(currentmicros);
 // }
  
  // Back encoder not working :(
  
  Serial.println(" ");
      Serial.print("Encoder 1 =  ");
      Serial.println(CharliePlexM::ul_LeftEncoder_Count);
      Serial.print("Encoder 2 =  ");
      Serial.println(CharliePlexM::ul_RightEncoder_Count);
   
   
   Ping();
  while (ul_Front_Echo_Time/58 < 7) {
    Ping();
       servo_FrontMotor.writeMicroseconds(1500);
        servo_BackMotor.writeMicroseconds(1500);
        servo_RightMotor.writeMicroseconds(1500);
        servo_LeftMotor.writeMicroseconds(1500);
  }
   
   
  if (ul_Right_Echo_Time/58 > 15) {
        servo_FrontMotor.writeMicroseconds(1200);
        servo_BackMotor.writeMicroseconds(1200);
        Ping();
    }

    if (ul_Right_Echo_Time/58 < 5) {
        servo_FrontMotor.writeMicroseconds(1800);
        servo_BackMotor.writeMicroseconds(1800);
        Ping();
    }
    else {
        servo_FrontMotor.writeMicroseconds(1500);
        servo_BackMotor.writeMicroseconds(1500);
  }
  
  
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
//#endif
}  




