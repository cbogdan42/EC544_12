#include <SoftwareSerial.h>

SoftwareSerial XBee(2, 3); // RX, TX

const int Uid = 1;                //UID of the current node
const byte interruptPin = 2;      //pin to which the push button is connected
const int RED_LED = 7;            
const int GREEN_LED = 5;
const int BLUE_LED = 3;
int number_of_nodes = 5;          
int leader_current_node = 0;      //if the current node is the leader, this value is 1
int infected = 0;                 //node is infected if this value is 1
int leader_time;                  //last time the leader sent a message
int packet_length = 7;            //length of the message
int reading = 0;
    
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
String network_UIDs[0];
String network_leaders[0];
String network_infections[0];
String out_message = str_UID + String(my_leader_status) + String(my_infection);
String in_message = str_UID + String(my_leader_status) + String(my_infection);

int electionStart = 0;
int to_UID;
int leader_ID=0;
int num_election = 0;
int temp,next;

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
  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), button_ISR, RISING);       //button interrupn
  digitalWrite(BLUE_LED, LOW);
}

void loop() 
{
  Serial.println("Hello World");
  char rx_data[packet_length] = {'\0'};
  char tx_data[packet_length] = {'\0'};
  leader_current_node = isLeader(Uid);
  while(Network_Discovered ==0)
  {
    leader_ID = 0;
    // Broadcast My ID ten times
    for(int i = 0; i < 10; i++)
    {
      out_message = str_UID + String(my_leader_status) + String(my_infection);
      XBee.println(out_message);
      delay(10);
    }
    // Listen for other IDs
    for(int i = 0; i < 10; i++)
    {
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
    if (num_loops > 10)
    {
      Network_Discovered = 1;
      electionStart = 1;
      Max_UID = network_UIDs[0].toInt();
      Min_UID = network_UIDs[0].toInt();
      for(i=0;i<int(sizeof(network_UIDs));i++)
      {
        if(network_UIDs[i].toInt()>Max_UID)
          Max_UID = network_UIDs[i].toInt();
        if((network_UIDs[i]).toInt()<Min_UID)
          Min_UID = network_UIDs[i].toInt();
      }
    }
  }

  if(in_message.substring(1,2).toInt() == Uid){
    if(electionStart == 0)
      electionStart = in_message.substring(5,6).toInt();
  }
  next = 4;
  for(int i = 0; i < int(sizeof(network_UIDs)); i++)  //Sends message to next node in list
  { 
  if(network_UIDs[i].toInt() > Uid)
  {
    temp = network_UIDs[i].toInt();
    next = min(next,temp);
    }
  }
  if(next == 4)
  { //UID is the max of the network_UIDs
    next = Min_UID;    
  }
  to_UID = next;

  if(electionStart == 1)
  {
  if (num_election == 0)
  {
    out_message = str_UID + String(to_UID) + String(my_leader_status) + String(my_infection) + String(electionStart) + String(Network_Discovered) + String(leader_ID);
    XBee.println(out_message);
    num_election++;
    }
  else if (num_election == 1)
  {
    leader_ID = Min_UID;
    if(Uid == Min_UID)   //Check if it is first node in ring, and set itself to leader
    {
      leader_ID = Min_UID;
      my_leader_status = 1;
    }
    else if(leader_ID != in_message.substring(7,8).toInt()) //If not, then check if nodes agree on who the leader is
    { 
      Network_Discovered = 0;      
    }      
    out_message = str_UID + String(to_UID) + String(my_leader_status) + String(my_infection) + String(electionStart) + String(Network_Discovered) + String(leader_ID);
    XBee.println(out_message);
    num_election = 0; 
    electionStart = 0;
    }
  }

  switch(leader_current_node)
  {
  //if this node is a non leader
  case 0:
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
      if (XBee.available())                               //read message
      {
        while(i<packet_length)
        {
          rx_data[i]= XBee.read();
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
    }       
    break;

  case 1:  
  if (reading == 1)
  {
    // loop through all network IDs
    for(int i = 0; i <= sizeof(network_UIDs); i++)
    {
      out_message = str_UID + String(network_UIDs[i]) + String(my_leader_status) + "0100";
      XBee.println(out_message);
    }
  }
  else 
  {
    out_message = str_UID + "0" + String(my_leader_status) + "0000";
    XBee.println(out_message);
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
