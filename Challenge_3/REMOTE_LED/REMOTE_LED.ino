
// ID LED pins
int LED_1 = 9;
int LED_2 = 10;
int LED_3 = 11;

// Reading Variabale
int reading;

// Setup XBee
// XBEE setup
#include <SoftwareSerial.h>
SoftwareSerial XBee(2, 3); // RX, TX

// Initiate State for LEDs (0 - off, 1 - on, 2 - rave)
int state = 0;

void setup() {
  // put your setup code here, to run once:
  
  // Set up pins
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  
  // Turn On XBEE
  XBee.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:

  // Read in from XBee
  reading = int(XBee.read());
  
  if(reading == 0){
    digitalWrite(LED_1,0);
    digitalWrite(LED_2,0);
    digitalWrite(LED_3,0);
    delay(.5);
    }
  else if(reading == 1){
    digitalWrite(LED_1,1);
    digitalWrite(LED_2,1);
    digitalWrite(LED_3,1);
    delay(.5);
    }
  else if(reading == 2){
    for(int n = 0; n<=4; n = n+1){
      digitalWrite(LED_1,1);
      digitalWrite(LED_2,1);
      digitalWrite(LED_3,1);
      delay(.05);
      digitalWrite(LED_1,0);
      digitalWrite(LED_2,0);
      digitalWrite(LED_3,0);
      delay(.05);
      }
    for(int n = 0; n<= 8; n = n+1){
      digitalWrite(LED_1, random(0,1));
      digitalWrite(LED_2, random(0,1));
      digitalWrite(LED_3, random(0,1));
      delay(.05);
      }
  }
  
  // Send back Status
  XBee.print(reading);
  
  }
