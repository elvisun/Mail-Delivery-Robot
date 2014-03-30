#include <iostream>
using namespace std;

//Pseudocode for Chassis Control:

// The rooms are numbered 1, 2, and 3

/* 
Serial Communication:
To upper micro:
'A' = Are you there?
'O' = Is there outgoing mail?
'I' = Drop to Incoming Mail
'R' = Reached destination room?

From upper micro:
'Y' = Yes
'n' = No
'W' = Wait please...
'1', '2', or '3' = Destination room number

*/

int previousroom = 1; // -1 stands for nothing
int destinationroom = 1; // Always start at room 1
int visited[4] = {0,1,0,0};
boolean connectionExists = false;

void mydelay(long time = 1000);
void establishContact(char character = 'A');
void moverobot();
void robot1to2();
void robot1to3();
void robot2to1();
void robot3to1();


void setup() {
	Serial.begin(9600);
	
	establishContact();


}




void loop() {

	// check outbox mail, from upper micro
	// mailexists = status of mail (true or false)
	boolean mailexists = false;
      Serial.write("L");
              
	if (connectionExists) {
		char reply = 'n';
		do {	
			Serial.write('O');
                        while (Serial.available() <= 0) {}
                        
			char reply = Serial.read();
		
			if (reply == 'Y') {
				mailexists = true;
			}
			else {
				mailexists = false;
				mydelay(100);
			}
		} while (reply == 'W');
	}

	
	if (mailexists) {
                while (Serial.available() < 0) {}
		char reply = Serial.read();
		if (reply == '1') 
			destinationroom = 1;
		else if (reply == '2') 
			destinationroom = 2;
		else if (reply == '3') 
			destinationroom = 3;
	}


	if (destinationroom != previousroom && mailexists) {
		moverobot();	
		Serial.write('I');
        }
       	else {
		for (int i = 1; i < 4; i++) {
                  
			if (visited[i] == 0) {
				destinationroom = i;
				moverobot();
                                Serial.print("Visit room # ");
                                 Serial.println(i);
				i = 4; // Exits the loop
			}
		}
	}
}


// This function orders the robot to go from previousroom to destinationroom
void moverobot() {
	        Serial.println("Move Robot");
		if (previousroom == 2) {

			if (destinationroom == 1) {
				robot2to1();
				visited[2] = true;
			}
			else if (destinationroom == 3) {
				robot2to1();
				robot1to3();
				visited[3] = true;
			}
		}
		else if (previousroom == 1) {
			if (destinationroom == 2) {
				robot1to2();
				visited[2] = true;
			}
			else if (destinationroom == 3) {
				robot1to3();
				visited[3] = true;
			}
		}	
		else if (previousroom == 3) {

			if (destinationroom == 1) {
				robot3to1();
				visited[1] = true;
			}
			else if (destinationroom == 2) {
				robot3to1();
				robot1to2();
				visited[2] = true;
			}
		}	
		
		
	
	previousroom = destinationroom;
        visited[destinationroom] = 1;

}

void establishContact(char character) {
	int counter = 0;
        
	do {
		Serial.print(character);
		mydelay(300);
		counter++;
	} while (Serial.available() <= 0 && counter < 33);
	
	if (Serial.available() > 0) {
		connectionExists = true;
	}
	else {
		connectionExists = false;
	}
}

void mydelay(long time) {
  long current = millis();
  	while (millis() - current < time) {};
}

void robot2to1() {
  Serial.println("Moving from 2 to 1...");  
	


}

void robot1to3() {
  Serial.println("Moving from 1 to 3...");    
  
  
  
  
  
  
}


void robot1to2() {
  Serial.println("Moving from 1 to 2...");  
  
  
  
  
  
}


void robot3to1() {
  
  Serial.println("Moving from 3 to 1...");    
  
  
  
  
}
