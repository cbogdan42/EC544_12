/// The following Constants need to be set for each Arduino 
const int UID = 5;               // UID of the current node - do not use UID = 2 (to distinguish network discovery blasts)

/// Button Variables

// Hardware Variables
const byte buttonPin = 0;             //pin to which the push button is connected
int reading = 1;                      // no pull-down resistor, so off is 1, set here as default

// Debounce Variables
unsigned long lastDebounceTime = 0;   // the last time the output pin was toggled
unsigned long debounceDelay =5;// the debounce time; increase if the output flickers
int lastButtonState = 0;              // Previous button state
int buttonState = 0;                  // Current button state

// Check Button Press Routine
int checkButton(){
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
int network_UIDs[4];              // Array of Network IDs
int election_pass;                // Node to pass to in an election
int election_previous;             // Node to receive from for election
int network_min;                  // Minimum UID in the network
int network_max;                  // Maximum UID in the network

/// Election constants
int election_initiator = 0;       // Set to 1 if initiated election
int electionconcluded = 0;       // Set to 1 to end election

/// Infection Variables
int infected = 0;                 // node is infected if this value is 1
int network_infected = 0;         // allows leader to keep track of network infections

/// Serial Communication Info
#include <SoftwareSerial.h>

/// XBee Setup Info
SoftwareSerial XBee(2, 3); // RX, TX

/// Network Communication Values

// Packet Lengths
int networkDiscoveryPL = 2;           // Packet Length for Network Discovery Messages - 'NetworkDiscoveryInt + FromUID
int electionPL = 4;                   // Packet Length for Election - 'ElectionInt + FromUID + ToUID + ProposedLeader'
char LeaderBroadcastChar = '0';           // Char for Leader Ping
char ElectionChar = '1';                  // Char for Election Broadcast
char InfectionSpreadChar = '2';           // Char for Spreading Infection
char InfectionClearChar = '3';            // Char for Clearing Infection
char NetworkDiscoveryChar= '4';           // Char for Network Discovery
int LeaderBroadcastInt = 0;               // Int for... 
int ElectionInt = 1;
int InfectionSpreadInt = 2;
int InfectionClearInt = 3;
int NetworkDiscoveryInt = 4;
int int_rx;                           // Variable to read in integers

// Read in Single Value - used under normal operations to determine state changes
int read_int(){
  if(XBee.available()){
    int in_message = XBee.read();
    int convert_message = in_message - 48;
    return convert_message;
  }
}

// Read in Packet - after certain state changes longer messages needed - may need to make it able to start at packetint values
String read_packet(int packet_Length){
  int packet_length_count = 0;
  String concat_message;
  
  while (packet_length_count<packet_Length){
  if(XBee.available()){
    int in_message = XBee.read();
    int convert_message = in_message - 48;
    String str_message = String(convert_message);
    Serial.println(in_message);
    Serial.println(convert_message);
    Serial.println(str_message);
    delay(10);
    concat_message = concat_message + str_message;
    Serial.println(concat_message);
    packet_length_count = packet_length_count + 1;
    }
  else{
    Serial.println("No Message");
    delay(10);
    }
  }
String out_message = concat_message;
return out_message;
}

/// State Routines

// Network Discovery Routine
void NetworkDiscovery(){
  int num_loops = 0;
  
  // Broadcast NetworkDiscoveryInt 10 times
  for(int i = 0; i < 10; i++){
    XBee.write(NetworkDiscoveryChar);
    delay(10);
  }
  
  // Let Network Go Silent
  delay(100);

  // Alternate between sending out information and receiving information until 10 times not adding new info
  Network_Discovered = 0;
  
  while(Network_Discovered == 0){  

    
    for(int i = 0; i < 10; i++)
    {
      String out_message = String(NetworkDiscoveryInt) + String(UID);
      XBee.println(out_message);
      delay(10);
    }
    
    // Listen for other IDs
    for(int i = 0; i < 10; i++)
    {          
      // check message availble
      if(XBee.available()){ 

          int check = XBee.read();
          // Read in message
          String in_message = read_packet(networkDiscoveryPL);
          // Check that it is a network discovery packet
          if (in_message.substring(0,1) == NetworkDiscoveryInt){
            // Check that it is not a broadcast
            if (in_message.substring(1,2) != NetworkDiscoveryInt){
              // Pull out UID from message
              String in_UID = String(in_message.substring(0,1));
              int UID_known = 0;

              // check if UID has already been found
              for(int n = 0; n<= int(sizeof(network_UIDs)); n ++){
                if (String(network_UIDs[n]) == in_UID){
                UID_known = 1;
                }
              }

              // If new UID, add data to database
              if (UID_known == 0){
                network_UIDs[Network_Count] = strtol(in_message.substring(1,2).c_str(),NULL,10);
                num_loops = 0;
                Network_Count = Network_Count + 1;
              }
              delay(10);
            }
          }
       }
       
    
    }
    num_loops = num_loops + 1;
    if (num_loops > 10){
      Network_Discovered = 1;
    }
  }

  // Identify min and max network UIDs
  int temp_min = network_UIDs[0];
  int temp_max = network_UIDs[0];
  for(int i=1; i>Network_Count; i++){
    if(network_UIDs[i]<temp_min){
      temp_min = network_UIDs[i];
    }
    if(network_UIDs[i]>temp_max){
      temp_max = network_UIDs[i];
    }
  }
  network_min = temp_min;
  network_max = temp_max;
  
  // Identify passing partners for election - pass is node messages go to, previous is messages come from
    int pass_found = 0;
    int temp_pass = network_min;
    int previous_found = 0;
    int temp_previous = network_max;
    
    for(int i = 0; i > Network_Count; i++){
      int check = network_UIDs[i];
      // Check for pass
      if(check>UID){
        if(check<=temp_pass){
          temp_pass = check;
          pass_found = 1;
        }
      }
      // Check for previous
      if(check<UID){
        if(check >=temp_previous){
          temp_previous = check;
          previous_found = 1;
        }
      }
    }
    if(pass_found){
      election_pass = temp_pass;
    }
    else{
      election_pass = network_min;
    }
    if(previous_found){
      election_previous = temp_previous;
    }
    else{
      election_pass = network_max;
    }
    for(int i = 0;i<Network_Count;i++){
      Serial.println(network_UIDs[i]);
    }
  
}
// Election Routine
void Election(){
  // Set All LEDs High to show that an Election is occurring
  digitalWrite(BLUE_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED,HIGH);
  
  // Determine Leader - min UID in network library
  int proposed_leader = network_min;
  
  // Round 1 - pass along message to next after receiving message from previous 
  if(election_initiator){
    // If you started the election, just start broadcasting election
    //String election_message = String(ElectionInt) + String(UID) + String(election_pass) + String(proposed_leader);
    //XBee.write(election_message);
    XBee.write(ElectionChar);
    XBee.write(UID);
    XBee.write(election_pass);
    XBee.write(proposed_leader);
  }
  else{
    // If you didn't start the election, wait for a message from your previous node
    int previousreceived = 0;
    while(previousreceived == 0){
      String election_packet = read_packet(electionPL);
      //check its an election packet
      if(election_packet.substring(0,1) == ElectionInt){
        // check its from the previous
        if(election_packet.substring(1,2) == election_previous){
          // Set previous_received and pass along message
          previousreceived = 1;
          String election_message = String(ElectionInt) + String(UID) + String(election_pass) + String(proposed_leader);
        }
      }
      
    }
  }

  // Round 2 - process proposed leader
  if(election_initiator){
    // Wait for previous round to conclude
    int previousreceived = 0;
    while(previousreceived == 0){
      String election_packet = read_packet(electionPL);
      //check its an election packet
      if(election_packet.substring(0,1) == ElectionInt){
        // check its from the previous
        if(election_packet.substring(1,2) == election_previous){
          // Set previous_received and pass along message with proposed leader
          previousreceived = 1;
          String election_message = String(ElectionInt) + String(UID) + String(election_pass) + String(proposed_leader);
        }
      }
    }
    // If you started the election, just start broadcasting election
    //String election_message = String(ElectionInt) + String(UID) + String(election_pass) + String(proposed_leader);
    XBee.write(ElectionChar);
    XBee.write(UID);
    XBee.write(election_pass);
    XBee.write(proposed_leader);
    

    // Wait for second round to conclude
    previousreceived = 0;
    int electionconcluded = 0;
    String network_proposed;
    
    while(previousreceived == 0){
      String election_packet = read_packet(electionPL);
      //check its an election packet
      if(election_packet.substring(0,1) == ElectionInt){
        // check its from the previous
        if(election_packet.substring(1,2) == election_previous){
          // Read in network proposed leader
          network_proposed = election_packet.substring(3,4);
          if(network_proposed == proposed_leader){
            electionconcluded =1;
            if(proposed_leader == UID){
              leader_current_node = 1;
            }
          }
        }
      }
  }
    }
  else{
    // If you didn't start the election, wait for a message from your previous node then compare
    int previousreceived = 0;
    electionconcluded = 0;
    String network_proposed;
    
    while(previousreceived == 0){
      String election_packet = read_packet(electionPL);
      //check its an election packet
      if(election_packet.substring(0,1) == ElectionInt){
        // check its from the previous
        if(election_packet.substring(1,2) == election_previous){
          // Read in network proposed leader
          network_proposed = election_packet.substring(3,4);
          if(network_proposed == proposed_leader){
            electionconcluded =1;
            if(proposed_leader == UID){
              leader_current_node = 1;
            }
          }
        }
      }
    }
  }

  if (electionconcluded == 0){
    NetworkDiscovery();
  }
  else{
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  }

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

  // Initialize Button
  pinMode(buttonPin,INPUT);
  // Run 10 check buttons to get over initial weird thing
  for (int i=0; i<20;i++){
    checkButton();
  }

  // Set to High to show in NetworkDiscovery
  digitalWrite(BLUE_LED,HIGH);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, HIGH);

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
      election_initiator = 1;
      Election();
    }

    // Read in State Message
    if(XBee.available()){
      int_rx = read_int();
    }
    else{
      int_rx = 9;           // 9 has no currently assigned meaning
    }

    // Check if need to start Election, Network Discovery, get Infected or Reset Leader Count
    if (int_rx == LeaderBroadcastInt){
      leader_time = 0;
    }
    else if (int_rx == ElectionInt){
      election_initiator = 0;
      Election();                  
    }
    else if (int_rx == NetworkDiscoveryInt){
      NetworkDiscovery();
    }
    else if (int_rx == InfectionSpreadInt){
      infected = 1;
    }
    
	  if(infected){
      // Set LEDs
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
      // If int_rx = InfectionClearInt, clear infection, else broadcast infection
      if(int_rx == InfectionClearInt){
        // Clear Infection
        infected = 0;
        digitalWrite(RED_LED, LOW);
	      digitalWrite(GREEN_LED, HIGH);
	      }
	    else{
        XBee.write(InfectionSpreadChar);
        Serial.write("Spreading Infection");
        delay(1000);
	    }
	  }
	  // Not Infected - Check button for infection
	  else{
      // Set LEDs to Not Infected Default
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
      Serial.println(infected);
      
      // Check if button pressed (== 0)
      int buttonPress = checkButton();
      if(buttonPress == 0){
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
    // Read in State Message
    if(XBee.available()){
      int_rx = read_int();
    }
    else{
      int_rx = 9;           // 9 has no currently assigned meaning
    }
//    Serial.println("int_rx");
//    Serial.println(int_rx);
    if(int_rx == 2){
      Serial.println("UGH");
    }
    
    // Check if need to start Election, Network Discovery, Network Infection or Broadcast Leader Ping
    if (leaderPingCNT > leaderPingThreashold){
      XBee.write(LeaderBroadcastChar);
      leaderPingCNT = 0;
    }
    else if (int_rx == int(ElectionInt)){
      election_initiator = 0;
      Election();                  
    }
    else if (int_rx == NetworkDiscoveryInt){
      NetworkDiscovery();
    }
    else if (int_rx == InfectionSpreadInt){
      // Set Network_Infected
      network_infected = 1;
      Serial.println("Network Infected");
    }

    // Check button press
    int buttonPress = checkButton();
    if(buttonPress == 0){
      // Send Clear Infection Message
      XBee.write(InfectionClearChar);
      delay(10);

      // Reset Network Infection Status & LEDs
      network_infected = 0;
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
    
    }
    delay(10);
	  break;
	}
}

