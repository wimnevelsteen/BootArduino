#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//<direction 7>


#include <Servo.h>             //Servo library

Servo servo_test;    		//initialize a servo object for the connected servo  
                    
const byte numChars = 32;
char receivedChars[numChars];

boolean newData = false;

void setup() {
    Serial.begin(9600);
    Serial.println("<Arduino is ready>");
  	servo_test.attach(9); 		 // attach the signal pin of servo to pin9 of arduino

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
      Serial.println(F("SSD1306 allocation failed"));
    }
    
    display.clearDisplay();
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0,0);             // Start at top-left corner
    display.println(F("Boot"));
    display.setCursor(0,10);             // Start at top-left corner
    display.println(F("Kobe"));
    display.setCursor(0,20);             // Start at top-left corner
    display.println(F("Daan"));
    display.display();
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
        Serial.print("Commando ... ");
        Serial.println(receivedChars);
        newData = false;
      
      	String newCommand(receivedChars);
      	String opCode = getValue(newCommand, ' ', 0);
    	  int param = getValue(newCommand, ' ', 1).toInt();

        
        Serial.print("Command ... ");
        Serial.println(opCode);
        Serial.print("Param ... ");
      	Serial.println(param);

        display.clearDisplay();
        display.setCursor(0,0);             // Start at top-left corner
        display.println("Boot Kobe Daan");
        display.setCursor(0,10);             // Start at top-left corner
        display.println(newCommand);
        display.display();
        
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
