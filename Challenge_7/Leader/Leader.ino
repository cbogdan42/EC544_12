
// Libraries
#include <SoftwareSerial.h>

// Variables
const int UID = 1;
const String str_UID = "1";
int my_infection = 0;
int my_leader_status = 0;
String network_UIDs[0];
String network_infections[0];
String network_leaders[0];
int Network_Discovered = 0;
int UID_known = 0;
int num_loops = 0;
String out_message = str_UID + String(my_leader_status) + String(my_infection);
String in_message = str_UID + String(my_leader_status) + String(my_infection);
int current_index = 0;
const int buttonPin = 2;    // the number of the pushbotton pin
int buttonState;            // the current reading from the input pin
int lastButtonState = LOW;  // the previous reading from the input pin
String incoming_message = str_UID + String(my_leader_status) + String(my_infection);

unsigned long lastDebounceTime = 0;   // the last time the output pin was toggled
unsigned long debounceDelay = 50;     // the debounce time; increase if the output flickers

// Communication Setup
SoftwareSerial XBee(2,3); //RX,TX



void setup() {
  // put your setup code here, to run once:

// Open Communication Channels
XBee.begin(9600);
Serial.begin(9600);

// Network Discovery Loop
while(Network_Discovered ==0){
  
  // Broadcast My ID ten times
  for(int i = 0; i <= 10; i++){
    out_message = str_UID + String(my_leader_status) + String(my_infection);
    XBee.println(out_message);
    delay(10);
  }
  // Listen for other IDs
  for(int i = 0; i <= 10; i++){
    // check message availble
    if(XBee.available()){   
        // Read in message
        in_message = XBee.read();
        in_message = out_message;
        
        // Pull out UID from message
        String in_UID = String(in_message.substring(0,1));
        UID_known = 0;

        // check if UID has already been found
        for(int n = 0; n<= int(sizeof(network_UIDs)); n ++){
          if (String(network_UIDs[n]) == in_UID){
            UID_known = 1;
          }
        }
    }
  
        // If new UID, add data to database
        if (UID_known == 0){
          current_index = sizeof(network_UIDs) + 1;
          network_UIDs[current_index] = in_message.substring(0,1);
          network_infections[current_index] = in_message.substring(1,2);
          network_leaders[current_index] = in_message.substring(2,3);
          num_loops = 0;
        }
        delay(10);
        }
    num_loops = num_loops++;
    if (num_loops > 10){
      Network_Discovered = 1;
    }
    }
  
  }

void loop() {
  // put your main code here, to run repeatedly:

// Check for button press - begin debounce code
int reading = digitalRead(buttonPin);

if (reading != lastButtonState) {
  // reset the debouncing timer
  lastDebounceTime = millis();
}

if ((millis() - lastDebounceTime) > debounceDelay) {
  // whatever the reading is at, its been there for longer
  // than the debounce delay, so take it as the actual current state

  // if the button state has changed:
  if (reading != buttonState) {
    buttonState = reading;
  }
}
lastButtonState = reading;

// debounce ended

if (reading == 1){
// loop through all network IDs
  for(int i = 0; i <= sizeof(network_UIDs); i++){
    out_message = str_UID + String(network_UIDs[i]) + String(my_leader_status) + "0100";
    XBee.println(out_message);
  }
}
else {
  out_message = str_UID + "0" + String(my_leader_status) + "0000";
  XBee.println(out_message);
}

// Bonus Code to read in infection status

// Read incoming message
if(XBee.available()){
  incoming_message = XBee.read();
  if (incoming_message[4] == "1"){
    int network_infection_status = 1;
  }
}


}
