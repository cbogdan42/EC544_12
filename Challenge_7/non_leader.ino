#include <SoftwareSerial.h>
SoftwareSerial XBee(2, 3); // RX, TX

const int Uid = 1;                //UID of the current node

const byte interruptPin = 2;      //pin to which the push button is connected

int number_of_nodes = 5;          

int leader_current_node = 0;      //if the current node is the leader, this value is 1

int infected = 0;                 //node is infected if this value is 1

int leader_time;                  //last time the leader sent a message

int packet_length = 7;            //length of the message

int isLeader(int Uid)             //returns 1 if the UID sent to the function is the UID of the leader
{
  return 0;
}


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