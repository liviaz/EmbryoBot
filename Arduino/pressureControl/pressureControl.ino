// read pressure sensor
// control linear actuator and solenoid valves
//
// Livia Z Yanez
// 8-18-16

#include <Servo.h>

#define VALVE_PIN 3
#define VENT_PIN 4
#define SERVO_PIN 9
#define PRESSURE_PIN 0

// globals
int pressure = 0; // 0-1023 read from analog input
int valToWrite = 0; // digital out for valve, or 1000 to 2000 map over motor stroke (Firgelli L12-I)
int pinToWrite = -1;
byte outArray[4] = {0,0,0,0};

Servo myservo;

char currVar;
boolean firstEndCharRight = false;
boolean commandComplete = false;
char *readBuff;


void setup() {

  Serial.begin(9600);
  pinMode(VALVE_PIN, OUTPUT);
  pinMode(VENT_PIN, OUTPUT);
  myservo.attach(SERVO_PIN, 1000, 2000);
  readBuff = (char *) calloc(100, sizeof(char));
  currVar = 0x10;
}


void loop() {

  // set values if necessary
  if (commandComplete) {

    if (pinToWrite == SERVO_PIN) {
       myservo.writeMicroseconds(valToWrite);
    } else if ((pinToWrite == VENT_PIN) || (pinToWrite == VALVE_PIN)) {
      if (valToWrite > 0) {
          digitalWrite(pinToWrite, HIGH);
        } else {
          digitalWrite(pinToWrite, LOW);
        }
    } 
    
    commandComplete = false;
    firstEndCharRight = false;
  }


  // read sensor value
  pressure = analogRead(PRESSURE_PIN);
  outArray[0] = pressure >> 8;
  outArray[1] = pressure;
  outArray[2] = 0xFF;
  outArray[3] = 0xFF;

  Serial.write(outArray, sizeof(outArray));
  Serial.flush();
  delay(20);

}


// read Serial bytes until a complete command is received
// each command is three bytes followed by two 0xFF chars
void serialEvent() {

  int currInd = 0;
  int numBytesRead = Serial.available();
  Serial.readBytes(readBuff, min(100,numBytesRead));

  Serial.println("readBuff: ");
  Serial.println(readBuff);

  while (currInd < min(100,numBytesRead) && (commandComplete == false)){

    // first char is pin number
    if (currVar == 0x10) {
      pinToWrite = readBuff[currInd];
      currVar = 0x11;

      // second char is first byte of value
    } else if (currVar == 0x11) {
      valToWrite = (signed char) readBuff[currInd];
      currVar = 0x12;

      // third char is second byte of value
    } else if (currVar == 0x12) {

      valToWrite = (valToWrite << 8) + ((unsigned char) readBuff[currInd]);
      currVar = 0x13;

      // fourth char should be 255
    } else if (currVar == 0x13) {

        if (readBuff[currInd] == ((signed char) 0xFF)) {
          
          firstEndCharRight = true;
          currVar = 0x14; 
        } else {
          currVar = 0x10;
        }

      // fifth char should also be 255
    } else if (currVar == 0x14) {
      if ((readBuff[currInd] == ((signed char) 0xFF)) && (firstEndCharRight = true)){
        commandComplete = true;
        currInd = numBytesRead;
      } 
      currVar = 0x10;
      
    }
    
    currInd++;
  }
  
}











