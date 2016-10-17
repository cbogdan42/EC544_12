#define SERIESRESISTOR 10000    
 
// What pin to connect the sensor to
#define THERMISTORPIN A0 

// XBEE setup
#include <SoftwareSerial.h>
SoftwareSerial XBee(2, 3); // RX, TX

void setup(void) {
  XBee.begin(9600);
  Serial.begin(9600);
}
 
void loop(void) {
  float reading;  
 
  reading = analogRead(THERMISTORPIN);
 
  Serial.print("Analog reading "); 
  Serial.println(reading);
 
  // convert the value to resistance
  reading = (1023 / reading)  - 1;
  reading = SERIESRESISTOR / reading;
  Serial.print("Thermistor resistance "); 
  Serial.println(reading);
  XBee.println(reading);
 
  delay(1000);
}
