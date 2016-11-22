#include <SoftwareSerial.h>
SoftwareSerial XBee(2, 3); // RX, TX
int num_loops = 0;
const int Uid = 1;                //UID of the current node
const byte interruptPin = 2;      //pin to which the push button is connected
int number_of_nodes = 5;          
int leader_current_node = 0;      //if the current node is the leader, this value is 1
int infected = 0;                 //node is infected if this value is 1
int leader_time;                  //last time the leader sent a message
int packet_length = 7;            //length of the message
int isLeader(int Uid);             //returns 1 if the UID sent to the function is the UID of the leader
int my_leader_status = 0;
int my_infection = 0;
int UID_known = 0;
int current_index = 0;
int start_election = 0;
const String str_UID = "1";
String out_message = str_UID + String(my_leader_status) + String(my_infection);
String in_message = str_UID + String(my_leader_status) + String(my_infection);
String network_UIDs[0];
String network_infections[0];
String network_leaders[0];




void setup() 
{
  XBee.begin(9600);
  Serial.begin(9600);  
  pinMode(interruptPin, INPUT_PULLUP);                                  
  attachInterrupt(digitalPinToInterrupt(interruptPin), button_ISR, RISING);       //button interrupt
}

void loop() 
{
    
  int message_source;
  int message_dest;
  int clear_infection;
  int get_infected;
  int i;
  int count = 0;
  char rx_data[packet_length] = {'\0'};
  char tx_data[packet_length] = {'\0'};

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
      start_election = 1;
    } 
          
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
      break;
  }
}

void button_ISR()
{
  if(!leader_current_node)
    infected = 1;
}
