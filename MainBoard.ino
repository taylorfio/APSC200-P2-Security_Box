#include <SoftwareSerial.h>
#include <Servo.h>

bool locked; // boolean for locked status
// LEDs
int red = 3, green = 2;
// switch
int SwitchState = 0;
int Switch = 6;
// force sensors
int pSensor1 = A0, pSensor2 = A1, pSensor3 = A2, pSensor4 = A3;
int force1, force2, force3, force4;
int forceVal = 150;
// alarm
int alarm = 5;
// servo
int pos = 0;
Servo servo_4;
// serial communication
int signal = 2; // communicating variable
#define rx 13 // receiver pin
#define tx 0 // transmitter pin deifined but not used
SoftwareSerial mySerial = SoftwareSerial(rx, tx);

void setup() {
  mySerial.begin(9600); // Begin the communication Serial at 9600 Baud
  Serial.begin(9600); // Begin the Serial at 9600 Baud
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(Switch, INPUT);
  pinMode(pSensor1, INPUT);
  pinMode(pSensor2, INPUT);
  pinMode(pSensor3, INPUT);
  pinMode(pSensor4, INPUT);
  servo_4.attach(4, 500, 2500); // servo
  pinMode(alarm, OUTPUT);
}

void loop() {
  if (mySerial.available()) {
    signal = mySerial.read(); //Read the serial data and store in var
    Serial.print("Serial reading = ");
    Serial.print(signal); //Print data on Serial Monitor
    Serial.print("\n");
    delay(10);
  }
  
  force1 = analogRead(pSensor1);
  force2 = analogRead(pSensor2);
  force3 = analogRead(pSensor3);
  force4 = analogRead(pSensor4);
  // use to print test force values
  //Serial.println("Analog reading 1 = %d \n", force1);
  
  // if any of the force sensors are triggered it locks
  if (force1 > forceVal || force2 > forceVal || force3 > forceVal || force4 > forceVal) {
    digitalWrite(green, HIGH);
    if (locked != true) {
      // closes lock
      delay(10000); // ten second delay after packege is dropped
      for (pos; pos <= 90; pos += 1) { // sweep the servo from 0 to 180 degrees in steps of 1 degrees
        // tell servo to go to position in variable 'pos'
        servo_4.write(pos);
        pos = 90; // sets servo position
        locked = true; // sets lock status
        delay(15);
      }
    }
  } else {
    digitalWrite(green, LOW);
  }
  
  if (signal == 1) { // if the open signal is true
    digitalWrite(red, HIGH);
    delay(1500);
    digitalWrite(red, LOW);
    if (locked == true){
      // opens lock
      for (pos; pos >= 0; pos -= 1) {
        // tell servo to go to position in variable 'pos'
        servo_4.write(pos);
        pos = 0; // sets servo position
        delay(10000); // ten second delay to retrieve packege
        locked = false; // sets lock status
        delay(15);
      }
    }
    signal = 2;
  } else if (signal == 0) { // if the open signal is false
    digitalWrite(red, HIGH);
    delay(500);
    digitalWrite(red, LOW);
    delay(500);
    digitalWrite(red, HIGH);
    delay(500);
    digitalWrite(red, LOW);
    signal = 2;
    delay(15);
  }
  
  // code for alarm
  SwitchState = digitalRead(Switch);
  if (SwitchState == LOW) {
    digitalWrite(alarm, HIGH);
    delay(10000); // alarm continues for 10 seconds
    digitalWrite(alarm, LOW);
  }
  
  delay(10);
}
