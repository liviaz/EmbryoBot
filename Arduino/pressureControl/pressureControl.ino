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
Servo myservo;

int currVar = 0;
boolean commandComplete = false;
String commandString = "aaa";


void setup() {

  Serial.begin(9600);
  pinMode(VALVE_PIN, OUTPUT);
  pinMode(VENT_PIN, OUTPUT);
  myservo.attach(SERVO_PIN, 1000, 2000);
  commandString.reserve(3);

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
  }


  // read sensor value
  pressure = analogRead(PRESSURE_PIN);
  Serial.print(pressure);

  // print 1024 to signify end
  int finished = 1024;
  Serial.print(finished);
  Serial.flush();
  delay(25);

}


// read Serial bytes until a complete command is received
// each command is two bytes followed by an end of line
void serialEvent() {

  while (Serial.available()) {

    int inVal = (int) Serial.read();

    if (currVar == 0) {
      pinToWrite = inVal;
    } else if (currVar == 1) {
      valToWrite = inVal;
    } else if ((currVar == 2) && (inVal == -1)) {
      currVar = -1;
      commandComplete = true;
    } else if ((currVar == 2) || (inVal == -1)) {
      currVar = -1;
      commandComplete = false;
    }
    
    currVar += 1;
  }
  
}











