#define SERIESRESISTOR 10000    
 
// What pin to connect the sensor to
#define THERMISTORPIN A0 
#define THERMISTORNOMINAL 8900 
#define TEMPERATURENOMINAL 25 
#define BCOEFFICIENT 3950
#define SERIESRESISTOR 9100
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
  Serial.print("Thermistor resistance "); 
  Serial.println(reading);
  XBee.println(int(reading));
  float steinhart;
  steinhart = reading/ THERMISTORNOMINAL;     // (R/Ro) //only if GDN is connected to 10k resistor, otherwise steinhart = THERMISTORNOMINAL/average;
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
 
  Serial.print("Temperature "); 
  Serial.print(steinhart);

  Serial.println(" *C");
 


  delay(1000);
}
