#define SERIESRESISTOR 10000    
 
// What pin to connect the sensor to
#define THERMISTORPIN A0 
#define THERMISTORNOMINAL 8900 
#define TEMPERATURENOMINAL 25 
#define BCOEFFICIENT 3950
#define SERIESRESISTOR 9100
#define SOURCE 1
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
 
  //Serial.print("Analog reading "); 
  //Serial.println(reading);
 
  // convert the value to resistance
  reading = (1023 / reading)  - 1;
  reading = SERIESRESISTOR / reading;
  //Serial.print("Thermistor resistance "); 
  //Serial.println(reading);
  float steinhart;
  steinhart = reading/ THERMISTORNOMINAL;     // (R/Ro) //only if GDN is connected to 10k resistor, otherwise steinhart = THERMISTORNOMINAL/average;
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
  Serial.print("Source:"); 
  Serial.println(SOURCE);
  Serial.print("Temperature "); 
  steinhart = steinhart + SOURCE*100;
  XBee.println((steinhart));
  Serial.print(steinhart);
  Serial.println("*C");
 /* SOURCE++;
  if(SOURCE==5)
    SOURCE = 1;*/
  delay(2000);
}
