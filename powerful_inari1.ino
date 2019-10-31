#include <Servo.h>             //Servo library
 
Servo servo_test;    		//initialize a servo object for the connected servo  
                    

const byte numChars = 32;
char receivedChars[numChars];

boolean newData = false;

void setup() {
    Serial.begin(9600);
    Serial.println("<Arduino is ready>");
  	servo_test.attach(9); 		 // attach the signal pin of servo to pin9 of arduino
}

void loop() {
    recvWithStartEndMarkers();
    processNewCommand();
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
 // if (Serial.available() > 0) {
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void processNewCommand() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
      
      	String newCommand(receivedChars);
      	String opCode = getValue(newCommand, ' ', 0);
    	int param = getValue(newCommand, ' ', 1).toInt();
        Serial.print("Command ... ");
        Serial.println(opCode);
        Serial.print("Param ... ");
      	Serial.println(param);
      
      	if (opCode == "direction") {
        	servoMotor(param);
        }
  			    
      	
    }
}


void servoMotor(int angle) {
  servo_test.write(angle);
}



String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
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


