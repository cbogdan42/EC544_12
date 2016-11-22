int electionStart = 0;
int to_UID;
int leader_ID=0;
int num_election = 0;
int temp,next;

void loop() {
  // put your main code here, to run repeatedly:
  electionStart = in_message.substring(5,6);
  next = 4;
  for(int i = 0; i < int(sizeof(network_UIDs)); i++){ //Sends message to next node in list
    if(network_UIDs[i] > UID){
      temp = network_UIDs[i];
      next = min(next,temp);
      }
  }
  if(next == 4){ //UID is the max of the network_UIDs
    next = Min_UID;    
  }
  to_UID = next;
  
  if(electionStart == 1){
    if (num_election == 0){
      out_message = str_UID + String(to_UID) + String(my_leader_status) + String(my_infection) + String(electionStart) + String(Network_Discovered) + String(leader_ID);
      Xbee.println(out_message);
      num_election++;
      }
    else if (num_election == 1){
      leader_ID = Min_ID;
      if(UID == Min_UID){  //Check if it is first node in ring, and set itself to leader
        leader_ID = Min_UID;
        my_leader_status = 1;
      }
      else if(leader_ID != in_message.substring(7,8)){ //If not, then check if nodes agree on who the leader is
        Network_Discovered = 1;      
      }      
      out_message = str_UID + String(to_UID) + String(my_leader_status) + String(my_infection) + String(electionStart) + String(Network_Discovered) + String(leader_ID);
      Xbee.println(out_message);
      num_election = 0; 
      electionStart = 0;
      }
    }
}



