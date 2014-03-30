/*

 MSE 2202 Project
 Language: Arduino
 
*/
 
 #include <Servo.h>
 #include <EEPROM.h>
 #include <uSTimer2.h>
 
Servo servo_Grab_Motor;
Servo servo_Platform_Motor;




boolean bt_Motors_Enabled = true;


//port pin constants


const int ci_Grab_Motor = 9;
const int ci_Platform_Motor = 8;


const int ci_Left_Line_Tracker = A0;   //input plug
const int ci_Right_Line_Tracker = A1;   //output plug
const int ci_Left_Light_Sensor = A2;   //input plug
const int ci_Right_Light_Sensor = A3;   //output plug
const int ci_Switch = A4;   //output plug


unsigned int ui_Left_Line_Tracker_Data;
unsigned int ui_Right_Line_Tracker_Data;


unsigned int ui_Left_Light_Sensor_Data;
unsigned int ui_Right_Light_Sensor_Data;

unsigned int ui_Switch_Data;


unsigned int prev_time;


int location=1;
int destination=2;

int arm_angle=0;
bool im_here=0;   //when see the light
bool letter_exist=0;   //if there is letter in the mail box
bool holding_letter=0;
int room_number=0;
int counter=1;

void setup() {
  Serial.begin(9600);
  
  // set up ultrasonic


  // set up drive motors
  pinMode(ci_Grab_Motor, OUTPUT);
  servo_Grab_Motor.attach(ci_Grab_Motor);
  pinMode(ci_Platform_Motor, OUTPUT);
  servo_Platform_Motor.attach(ci_Platform_Motor);

                    
  pinMode(ci_Left_Line_Tracker, INPUT);
  pinMode(ci_Right_Line_Tracker, INPUT);                  

  pinMode(ci_Switch, INPUT);
  
  pinMode(ci_Left_Light_Sensor, INPUT);
  pinMode(ci_Right_Light_Sensor, INPUT);    
}


void loop()
{
  prev_time=millis();
  
  
  //position arm to be at initial position  asdjflkwejrkl???
  //while driving straight
  if(im_here==0)
  {
    //force the robot to move for couple seconds so it doesnt read the lights again, have to go pass the right led
    // move_forward_for_2_second();
    read_led();
  }
  
  if(im_here==1)
  {
    //stop the motor
    rotate_left();
    //move to adjust the position
    if(holding_letter==1)
    dispense_letter();
    if(letter_exist==1)
    retrieve_letter();
    rotate_right();
  }
  
 
  //depends on what room it's going, rotate the arm to the correct position
  
  
  
  
  
  
  
  //keep going
  

  /*
  if(ui_Switch_Data==0)
    servo_Platform_Motor.writeMicroseconds(1350);
  else
    servo_Platform_Motor.writeMicroseconds(1500);
  */
  //servo_Grab_Motor.writeMicroseconds(1200);

  //servo_Platform_Motor.writeMicroseconds(1200);   //1200 is counterclockwise,  1800 is clockwise  (view from the top)
  //servo_Grab_Motor.writeMicroseconds(1200);   //1200 is pulling in, and 1800 is spitting out 

/*  
  Serial.print("Trackers: Left = ");
  Serial.print(ui_Left_Line_Tracker_Data,DEC);
  Serial.print(", Right = ");
  Serial.println(ui_Right_Line_Tracker_Data,DEC);
  
  Serial.print("Light Sensors: Left = ");
  Serial.print(ui_Left_Light_Sensor_Data,DEC);
  Serial.print(", Right = ");
  Serial.println(ui_Right_Light_Sensor_Data,DEC);
  
  Serial.print("Switch ");
  Serial.println(ui_Switch_Data,DEC);
*/
}


void read_led()
{
  ui_Left_Light_Sensor_Data=analogRead(ci_Left_Light_Sensor);
  ui_Right_Light_Sensor_Data=analogRead(ci_Right_Light_Sensor);
  if(ui_Left_Light_Sensor_Data<30);
  {
    if(counter==1)
    {
    im_here=1;
    //call motor stop;
    Serial.write(1); //1 means stop the motor
    }
    counter--;
  }
  
  //wait there for 5 seconds, if the right light is on, three is a letter
  if((ui_Left_Light_Sensor_Data<30)&&(ui_Right_Light_Sensor_Data<30))
    letter_exist=1;
  else
    update_destination();
  //if the robot moves too fast for it to stop, make the motor go slow once the right sensor sees the left light
}

void rotate_left()
{
  servo_Platform_Motor.writeMicroseconds(1350);
  delay(100);
  ui_Switch_Data = analogRead(ci_Switch);
  while(ui_Switch_Data==0)
  {
    servo_Platform_Motor.writeMicroseconds(1350);
    ui_Switch_Data = analogRead(ci_Switch);          //have to read the switch every time
  }
  servo_Platform_Motor.writeMicroseconds(1500);
  arm_angle=arm_angle-90;
}

void rotate_right()
{
  servo_Platform_Motor.writeMicroseconds(1650);
  delay(100);
  ui_Switch_Data = analogRead(ci_Switch);
  while(ui_Switch_Data==0)
  {
    servo_Platform_Motor.writeMicroseconds(1650);
    ui_Switch_Data = analogRead(ci_Switch);          //have to read the switch every time
  }
  servo_Platform_Motor.writeMicroseconds(1500);
  arm_angle=arm_angle+90;
}

void retrieve_letter()
{
 servo_Grab_Motor.writeMicroseconds(1200);  //pulling in 
 delay(2000);
 servo_Grab_Motor.writeMicroseconds(1500);  
 im_here=0; // or something else that makes it keep going 
 read_letter();
}

void dispense_letter()
{
 servo_Grab_Motor.writeMicroseconds(1800);  //pushing out
 delay(2000);
 servo_Grab_Motor.writeMicroseconds(1500);
 im_here=0; // or something else that makes it keep going 
}

void read_letter()
{
  ui_Left_Line_Tracker_Data = analogRead(ci_Left_Line_Tracker);
  ui_Right_Line_Tracker_Data = analogRead(ci_Right_Line_Tracker);
  if(ui_Left_Line_Tracker_Data>400)&&(ui_Right_Line_Tracker_Data>400))   //black black  11=room 4
  room_number=4;
  if(ui_Left_Line_Tracker_Data>400)&&(ui_Right_Line_Tracker_Data<400))   //black white  10=room 3
  room_number=3;
  if(ui_Left_Line_Tracker_Data<400)&&(ui_Right_Line_Tracker_Data>400))   //white black  01=room 2
  room_number=2;
  if(ui_Left_Line_Tracker_Data>400)&&(ui_Right_Line_Tracker_Data>400))   //white white  00=room 1
  room_number=1;
  destination=room_number;
  counter=abs(destination-location);
  Serial.write(room_number+10);
}

void update_destination()
{
  if(destination<3)
  destination+=destination;
  else
  destination=1;
}
void turning_right_reset()
{
  rotate_right();
  arm_angle=0;
}

void turning_left_reset()
{
  rotate_left();
  arm_angle=0;
}






