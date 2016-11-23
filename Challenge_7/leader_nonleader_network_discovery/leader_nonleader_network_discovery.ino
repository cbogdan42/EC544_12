/// The following Constants need to be set for each Arduino 
const int UID = 2;                // UID of the current node - do not use UID = 2 (to distinguish network discovery blasts)

/// Button Variables

// Hardware Variables
const byte buttonPin = 0;             //pin to which the push button is connected
int reading = 1;                      // no pull-down resistor, so off is 1, set here as default

// Debounce Variables
unsigned long lastDebounceTime = 0;   // the last time the output pin was toggled
unsigned long debounceDelay = 50;     // the debounce time; increase if the output flickers

// Check Button Press Routine
void checkButton(){
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
  return buttonState;
}

/// LED Variables
const int RED_LED = 7;						// Infection
const int GREEN_LED = 5;          // Not Infected
const int BLUE_LED = 6;           // Leader 
const int blink_LED = 100;        // If LED blinked, lenght of on or off cycle

/// Network Discovery Constants
int number_of_nodes = 0;          // To be updated during network discovery  
int leader_current_node = 0;      // if the current node is the leader, this value is 1, starts at 0 unless debugging
int leader_time = 0;              // number of iterations since last heard from leader
int leader_threshold = 10;        // max number of iterations without hearing from leader before starting election
int leaderPingCNT = 0;            // Leader Pings Network every leaderPingThreshold loops
int leaderPingThreashold = 10;    // Number of loops through leader code before pinging network
int Network_Discovered = 0;       // Starts Network Discovery assuming we don't yet know the network
int Network_Count = 0;            // Number of nodes in the network

/// Infection Variables
int infected = 0;                 // node is infected if this value is 1
int network_infected = 0;         // allows leader to keep track of network infections

/// XBee Setup Info
SoftwareSerial XBee(2, 3); // RX, TX

/// Serial Communication Info
#include <SoftwareSerial.h>

/// Network Communication Values

// Packet Lengths
int networkDiscoveryPL = 2;           // Packet Length for Network Discovery Messages - 'NetworkDiscoveryInt + FromUID
int electionPL = 4;                   // Packet Length for Election - 'ElectionInt + FromUID + ToUID + ProposedLeader'
int LeaderBroadcastInt = 0;           // Integer for Leader Ping
int ElectionInt = 1;                  // Integer for Election Broadcast
int InfectionSpreadInt = 2;           // Integer for Spreading Infection
int InfectionClearInt = 3;            // Integer for Clearing Infection
int NetworkDiscoveryInt = 4;          // Integer for Network Discovery

// Read in Single Value - used under normal operations to determine state changes
void read_int(){
  if(XBee.available()){
    in_message = XBee.read();
    convert_message = in_message - 48;
    return convert_message;
  }
}

// Read in Packet - after certain state changes longer messages needed - may need to make it able to start at packetint values
void read_packet(int packet_Length){
  while (i<packetLength){
  if(XBee.available()){
    in_message = XBee.read();
    convert_message = in_message - 48;
    String str_message = String(convert_message);
    Serial.println(in_message);
    Serial.println(convert_message);
    Serial.println(str_message);
    delay(10);
    concat_message = concat_message + str_message;
    Serial.println(concat_message);
    i = i + 1;
  }
  else{
    Serial.println("No Message");
    delay(10);
  }
}
out_message = concat_message;
i = 0;
concat_message = "";
return out_message;
}

/// State Routines

// Network Discovery Routine
void NetworkDiscovery(){
  // Broadcast NetworkDiscoveryInt 10 times
  for(int i = 0; i < 10; i++){
    XBee.write(NetworkDiscoveryInt);
    delay(10);
  }
  
  // Let Network Go Silent
  delay(100);


  // Alternate between sending out information and receiving information until 10 times not adding new info
  Network_Discoverd = 0;
  
  while(Network_Discovered == 0){  
    for(int i = 0; i < 10; i++)
    {
      out_message = String(NetworkDiscoveryInt) + String(UID);
      XBee.println(out_message);
      delay(10);
    }
    // Listen for other IDs
    for(int i = 0; i < 10; i++)
    {
      // check message availble
      if(XBee.available()){   
          // Read in message
          in_message = read_packet(NetworkDiscoveryPL);

          // Check that it is a network discovery packet
          if (in_message.substring(0,1) == NetworkDiscoveryInt){
            // Check that it is not a broadcast
            if (in_message.substing(1,2) != NetworkDiscoveryInt){
              // Pull out UID from message
              String in_UID = String(in_message.substring(0,1));
              UID_known = 0;

              // check if UID has already been found
              for(int n = 0; n<= int(sizeof(network_UIDs)); n ++){
                if (String(network_UIDs[n]) == in_UID){
                UID_known = 1;
                }
              }

              // If new UID, add data to database
              if (UID_known == 0){
                network_UIDs[Network_Count] = in_message.substring(1,2);
                num_loops = 0;
                Network_Count = Network_Count + 1;
              }
              delay(10);
            }
          }
       }
    
    }
    num_loops = num_loops++;
    if (num_loops > 10){
      Network_Discovered = 1;
    }
  }
}
// Election Routine
void Election(){
  
}

void setup() 
{
  // Initialize Communication
  XBee.begin(9600);
  Serial.begin(9600); 

  // Initialize LEDs
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  // First Network Discovery
  NetworkDiscovery();

  // Indicate End of Setup Loop by blinking all LEDs 4 times
  for (int i;i<4; i++){
      // Set LEDs to Low
      digitalWrite(BLUE_LED, LOW);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED,LOW);
      delay(blink_LED);

      // Set LEDs to Low
      digitalWrite(BLUE_LED, LOW);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED,LOW);
      delay(blink_LED);
  }
  Serial.write("Setup Complete");
}

void loop() 
{
  // Determine whether to run leader or non-leader code
	switch(leader_current_node)
	{
  
	//Non Leader Case
	case 0:
 
    // Turn off Blue LED
    digitalWrite(BLUE_LED, LOW);

    // Has the Leader gone silent?
    if (leader_time > leader_threshold){
      Election();
    }

    // Read in State Message
    int_rx = read_int();

    // Check if need to start Election, Network Discovery or Reset Leader Count
    if (int_rx == LeaderBroadcastInt){
      leader_time = 0;
    }
    else if (int_rx == ElectionInt){
      Election();                  
    }
    else if (int_rx == NetworkDiscoveryInt){
      Network_Discovery();
    }
    
	  if(infected){
      // If int_rx = InfectionClearInt, clear infection, else broadcast infection
      if(int_rx == InfectionClearInt){
        // Clear Infection
        infected = 0;
        digitalWrite(RED_LED, LOW);
	      digitalWrite(GREEN_LED, HIGH);
	      }
	    else{
        XBee.write('InfectionSpreadInt');
	    }
	  }
	  // Not Infected - Check button for infection
	  else{
      // Set LEDs to Not Infected Default
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
      
      // Check if button pressed (== 0)
      if(check_button == 0){
        // Get Infected
        infected = 1;
        // Reset LEDs
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, LOW);
      }
	  }
	  
	  break;

  // Leader Case
	case 1:
 
    // Turn on Blue LED
    digitalWrite(BLUE_LED, HIGH);

    // Set Network Status LEDs
    if (network_infected){
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
    }
    else{
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
    }

    // Iterate LeaderPingCNT
    leaderPingCNT = leaderPingCNT + 1;
    
    // Read in State Message
    int_rx = read_int();

    // Check if need to start Election, Network Discovery, Network Infection or Broadcast Leader Ping
    if (leaderPingCNT > leaderPingThreashold){
      XBee.write(LeaderBroadcastInt);
      leaderPingCNT = 0;
    }
    else if (int_rx == ElectionInt){
      Election();                  
    }
    else if (int_rx == NetworkDiscoveryInt){
      Network_Discovery();
    }
    else if (int_rx == InfectionSpreadInt){
      // Set Network_Infected
      network_infected = 1;
    }

    // Check button press
    if(checkButton() == 0){
      // Send Clear Infection Message
      XBee.write(InfectionClearInt);

      // Reset Network Infection Status & LEDs
      network_infected = 0;
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
      
    }

	  break;
	}
}

