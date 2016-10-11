
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
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(XBee.available()){
    Serial.print("Made it into loop");
    
    // Read in from XBee
    state = int(XBee.read())-48;
    
    if(state == 0){
      digitalWrite(LED_1,0);
      digitalWrite(LED_2,0);
      digitalWrite(LED_3,0);
      delay(10);
      }
    else if(state == 1){
      digitalWrite(LED_1,1);
      digitalWrite(LED_2,1);
      digitalWrite(LED_3,1);
      delay(10);
      }
    else if(state == 2){
      for(int n = 0; n < 5; n++){
        for(int n = 0; n <4; n++){
          digitalWrite(LED_1,1);
          digitalWrite(LED_2,1);
          digitalWrite(LED_3,1);
          delay(100);
          digitalWrite(LED_1,0);
          digitalWrite(LED_2,0);
          digitalWrite(LED_3,0);
          delay(100);
        }
        for(int n = 0; n < 8; n++){
          digitalWrite(LED_1, random(0,2));
          digitalWrite(LED_2, random(0,2));
          digitalWrite(LED_3, random(0,2));
          delay(100);
          }
      }
      }

    // Send back Status
    Serial.println(state);
    XBee.print(state);

  }
  
  }
