#include <Keypad.h>
#include <SoftwareSerial.h>

int signal; // communicating variable
#define rx 0 // receiver pin deifined but not used
#define tx 13 // transmitter pin
SoftwareSerial mySerial =  SoftwareSerial(rx, tx);

#define Password_Length 5 //sets the length of the password
char keypressed;
char Data[Password_Length]; 
char Master[Password_Length] = "2580"; //sets password
byte data_count = 0, master_count = 0;
const byte Rows = 4; //number of rows on the keypad
const byte Cols = 4; //number of columns on the keypad

//keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[Rows][Cols]= 
{
{'1', '2', '3', 'A'}, 
{'4', '5', '6', 'B'}, 
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};
//Code that shows the the keypad connections to the arduino terminals
byte rowPins[Rows] = {9, 8, 7, 6};
byte colPins[Cols] = {5, 4, 3, 2};
//initializes an instance of the Keypad class
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, Rows, Cols);

void setup() {
  mySerial.begin(9600); // Begin the communication Serial at 9600 Baud
  Serial.begin(9600); // Begin the Serial at 9600 Baud
}

//If key is pressed, this key is stored in 'keypressed' variable
//If key is not equal to 'NO_KEY', then this key is printed out

void loop(){  
  keypressed = myKeypad.getKey();
  if (keypressed){
    Data[data_count] = keypressed;
    data_count++;
    Serial.println(keypressed);
  }

  if(data_count == Password_Length-1){
    delay(500);
    if(!strcmp(Data, Master)){ //correct
      Serial.print("true \n");
      signal = 1;
      mySerial.write(signal); //Write the serial data
      delay(100);
      signal = 0; // resets the boolean
      data_count = 0, master_count = 0;
      
    } else { //incorrect
      Serial.print("false \n");
      signal = 0;
      mySerial.write(signal); //Write the serial data
      delay(100);
      signal = 2;
      data_count = 0, master_count = 0;
    }
  delay(500);
  }
delay(10);
}
