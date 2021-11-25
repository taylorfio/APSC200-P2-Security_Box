#include <SoftwareSerial.h>
#include <Servo.h>
/*
  This example connects to a WPA2 Enterprise WiFi network.
  Then it prints the MAC address of the WiFi module,
  the IP address obtained, and other network details.

  Based on ConnectWithWPA.ino by dlf (Metodo2 srl) and Tom Igoe
*/
#include <SPI.h>
#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"
using namespace std;
#define IFTTT_Key "dsjptALswZwGuM5dlyEKJh"
#define IFTTT_Event "ESP_MotionSMS"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;  // your WPA2 enterprise network SSID (name)
char user[] = SECRET_USER;  // your WPA2 enterprise username
char pass[] = SECRET_PASS;  // your WPA2 enterprise password
int status = WL_IDLE_STATUS;     // the WiFi radio's status
const char* resource = "https://maker.ifttt.com/trigger/ESP_MotionSMS/with/key/dsjptALswZwGuM5dlyEKJh";
const char* server = "maker.ifttt.com";
WiFiClient client;
HttpClient http(client, "maker.ifttt.com", 80);

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
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA2 enterprise network:
    // - You can optionally provide additional identity and CA cert (String) parameters if your network requires them:
    //      WiFi.beginEnterprise(ssid, user, pass, identity, caCert)
    status = WiFi.beginEnterprise(ssid, user, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();
  
  mySerial.begin(9600); // Begin the communication Serial at 9600 Baud
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
  /*
  // check the network connection once every 10 seconds:
  delay(10000);
  printCurrentNet();
  */
  
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
      sendEmail("A package has been delivered");
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
  if (SwitchState == HIGH) {
    digitalWrite(alarm, HIGH);
    // take photo - camera module not working
    // send Email notification
    sendEmail("We have detected a potential disturbance with your package box");
    delay(10000); // alarm continues for 10 seconds
    digitalWrite(alarm, LOW);
  }
  
  delay(10);
}

// Functions for internet communication

void sendEmail(String message){ // call to send email
  Serial.println("making POST request");
  String contentType = "application/json";
  String postData = "{\"value1\":\"" + message + "\"}";

  http.post("/trigger/ESP_MotionSMS/with/key/dsjptALswZwGuM5dlyEKJh", contentType, postData);

  // read the status code and body of the response
  int statusCode = http.responseStatusCode();
  String response = http.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
}

void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
