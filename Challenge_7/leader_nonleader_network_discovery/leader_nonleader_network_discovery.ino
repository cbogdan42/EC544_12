#include <SoftwareSerial.h>

SoftwareSerial XBee(2, 3); // RX, TX

const int Uid = 2;                //UID of the current node
const byte buttonPin = 0;      //pin to which the push button is connected
const int RED_LED = 7;						// Infection
const int GREEN_LED = 5;          // Not Infected
const int BLUE_LED = 6;           // Leader 
int number_of_nodes = 5;          
int leader_current_node = 0;      //if the current node is the leader, this value is 1
int infected = 0;                 //node is infected if this value is 1
int leader_time;                  //last time the leader sent a message
int packet_length = 7;            //length of the message
int reading = 1;
    
int message_source;
int message_dest;
int clear_infection;
int get_infected;
int i;
int count = 0;
int UID_known = 0;
int Max_UID = 0;
int Min_UID = 0;
int UID_tracking;

const String str_UID = "1";
int my_leader_status = 0;
int my_infection = 0;
int current_index = 0;
int num_loops = 0;
int Network_Discovered = 0;
int Network_Count = 1;
String network_UIDs[0];
String network_leaders[0];
String network_infections[0];
String out_message = str_UID + String(my_leader_status) + String(my_infection);
String in_message = str_UID + String(my_leader_status) + String(my_infection);
int lastButtonState = 0;
int buttonState = 0;

unsigned long lastDebounceTime = 0;   // the last time the output pin was toggled
unsigned long debounceDelay = 50;     // the debounce time; increase if the output flickers


int isLeader(int Uid)             //returns 1 if the UID sent to the function is the UID of the leader
{
	if(Uid == 1)
  		return 1;
  	else
  		return 0;
}


void setup() 
{
  XBee.begin(9600);
  Serial.begin(9600);  
  //pinMode(interruptPin, INPUT_PULLUP);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  //attachInterrupt(digitalPinToInterrupt(interruptPin), button_ISR, RISING);       //button interrupn
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED,LOW);

  Serial.write("Setup Complete");
}

void loop() 
{
	char rx_data[packet_length] = {'\0'};
	char tx_data[packet_length] = {'\0'};
	leader_current_node = isLeader(Uid);

  // Network Discovery loop

//	while(Network_Discovered ==0)
//	{
//	  String leader_ID = "0";
//	  // Broadcast My ID ten times
//	  for(int i = 0; i < 10; i++)
//	  {
//	    out_message = str_UID + String(my_leader_status) + String(my_infection);
//	    XBee.println(out_message);
//	    delay(10);
//	  }
//	  // Listen for other IDs
//	  for(int i = 0; i < 10; i++)
//	  {
//	    // check message availble
//	    if(XBee.available()){   
//	        // Read in message
//	        in_message = XBee.read();
//	        in_message = out_message;
//	        
//	        // Pull out UID from message
//	        String in_UID = String(in_message.substring(0,1));
//	        UID_known = 0;
//
//	        // check if UID has already been found
//	        for(int n = 0; n<= int(sizeof(network_UIDs)); n ++){
//	          if (String(network_UIDs[n]) == in_UID){
//	            UID_known = 1;
//	          }
//	        }
//	    }
//	  
//	    // If new UID, add data to database
//	    if (UID_known == 0){
//	      current_index = sizeof(network_UIDs) + 1;
//	      network_UIDs[current_index] = in_message.substring(0,1);
//	      network_infections[current_index] = in_message.substring(1,2);
//	      network_leaders[current_index] = in_message.substring(2,3);
//	      num_loops = 0;
//        Network_Count = Network_Count + 1;
//	    }
//	    delay(10);
//	  }
//	  num_loops = num_loops++;
//	  if (num_loops > 10)
//	  {
//	    Network_Discovered = 1;
////	    Max_UID = int(network_UIDs[0]);
////	    Min_UID = int(network_UIDs[0]);
////	    for(i=0;i<int(sizeof(network_UIDs));i++)
////	    {
////	    	if(int(network_UIDs[i])>Max_UID)
////	    		Max_UID = int(network_UIDs[i]);
////	    	if(int(network_UIDs[i])<Min_UID)
////	    		Min_UID = int(network_UIDs[i]);
////	    }
//	  }
//	}

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
 
	switch(leader_current_node)
	{
	//if this node is a non leader
	case 0:
    // Turn off Blue LED
    digitalWrite(BLUE_LED, LOW);
    Serial.write("I am not the leader");
    Serial.println(infected);
    
	  if(infected)
	  {
	    if (XBee.available())       //read data if available
	    {
	      while(i<packet_length)
	      {
	        rx_data[i]= XBee.read();
	        if(rx_data[i] - '0' >= 0 && rx_data[i] - '0' < 10)
	          i++;
	      }
        Serial.println(rx_data);
        Serial.println(rx_data[4]);
	      message_source = rx_data[0] - '0';                //source of the message
	      message_dest = rx_data[1] - '0';                  //destination of the message
	      clear_infection = rx_data[4] - '0';               //clear infection message from the leader
	      if(isLeader(message_source) && clear_infection)   //clear infection
	      {
	        infected = 0;
	        digitalWrite(RED_LED, LOW);
	        digitalWrite(GREEN_LED, HIGH);
	        count = 0;
	      }
	    }
	    else if(count == 0)                                 //transmit infection message to other nodes
	    {
	      tx_data[0] = Uid + '0';
	      tx_data[1] = 0 + '0';
	      tx_data[2] = 0 + '0';
	      tx_data[3] = 1 + '0';
	      tx_data[4] = 0 + '0';
	      tx_data[5] = 0 + '0';
	      tx_data[6] = 0 + '0';
	      XBee.write(tx_data);
	      count++;
	    }
	    delay(2000);
	  } 
	  else
	  {
      Serial.println('Made it into infection Else');
      // Infected By Message
	    if (XBee.available())                               //read message
	    {
      Serial.println('Message was available');
	      while(i<packet_length)
	      {
	        rx_data[i]= XBee.read();
          Serial.println(rx_data);
	        if(rx_data[i] - '0' >= 0 && rx_data[i] - '0' < 10)
	          i++;
	      }
	      message_source = rx_data[0] - '0';
	      get_infected = rx_data[3] - '0';
	      if(isLeader(message_source))                      //log message time from the leader
	      {
	        leader_time = millis();
	      }
	      else
	      {
	        if(get_infected)                                //get infected if non leader nodes ask you to
	          infected = 1;
	      }
	    } 
      // Infected by Button
      if (reading == 0){
        infected = 1;  
      }
	  }       
    if(infected == 1){
      digitalWrite(RED_LED,HIGH);
      digitalWrite(GREEN_LED, LOW);
    }
    else{
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);
    }
	  break;

	case 1:  
  Serial.write('Leader Loop');
  // Set Blue LED HIGH
  digitalWrite(BLUE_LED,HIGH);
	if (reading == 1)
  {
    out_message = str_UID + "0" + String(my_leader_status) + "0000";
    XBee.println(out_message);
  }
	
	else {

    // loop through all network IDs
    for(int i = 0; i <= Network_Count; i++)
    {
      Serial.println(i);
      out_message = str_UID + "2" + String(my_leader_status) + "0100";
      //out_message = str_UID + String(network_UIDs[i]) + String(my_leader_status) + "0100";
      XBee.println(out_message);
      Serial.println(out_message);
      delay(10);
    }
   Serial.println("I sent a message to clear the infection");
   
   reading = 1;

  }

	  break;
	}
}

void button_ISR()
{
  if(!leader_current_node)
  {
   	infected = 1;
   	digitalWrite(RED_LED, HIGH);
   	digitalWrite(GREEN_LED, LOW);
  }
  else
    reading = 1;
}
