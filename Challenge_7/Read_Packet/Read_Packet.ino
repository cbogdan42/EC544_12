#include <SoftwareSerial.h>
SoftwareSerial XBee(2, 3); // RX, TX

int in_message;
const int packetLength = 8;
String convert_message;
String concat_message;
int i = 0;
String out_message;



void setup() {
  // put your setup code here, to run once:
  XBee.begin(9600);
  Serial.begin(9600);
}

// This function reads in a single XBee value and converts it to an int, 0-9
void read_int(){
  if(XBee.available()){
    in_message = XBee.read();
    convert_message = in_message - 48;
    return convert_message;
  }
}

void read_packet(){
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
    delay(100);
  }
}
out_message = concat_message;
i = 0;
concat_message = "";
return out_message;
}
void loop() {
  // put your main code here, to run repeatedly:
//  XBee.write("Hello, World! 12\n");
//  Serial.write("Sent a message...\n");
//  delay(1000);
//  Serial.write(XBee.read());

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
    delay(100);
  }
}
i = 0;
concat_message = "";

}
