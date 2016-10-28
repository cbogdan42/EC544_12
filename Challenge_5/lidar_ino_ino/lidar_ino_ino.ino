/*
http://pulsedlight3d.com
LIDAR-Lite Continuous Pulse-Width Modulation
The LIDAR-Lite has the ability to read distance without the use of I2C, simply 
by reading the amount of time a signal from the mode pin is HIGH. We then take that
duration (here measured in microseconds) and convert it to centimeters using the 
LIDAR-Lite constant of 10usec/cm. We then print that value to the serial monitor. 
Technical explanation is on page 15/16 of the operating manual: 
http://pulsedlight3d.com/pl3d/wp-content/uploads/2014/11/LIDAR-Lite-Operating-Manual.pdf
*/

unsigned long pulse_width1;
unsigned long pulse_width2;
#define TRIGGER1 2
#define MONITOR1 3
#define TRIGGER2 4
#define MONITOR2 6
#define TEST_LED_PIN 8


void setup()
{
  Serial.begin(9600); // Start serial communications
  pinMode(TRIGGER1, OUTPUT); // Set pin 2 as trigger pin
  pinMode(MONITOR1, INPUT); // Set pin 3 as monitor pin
  digitalWrite(TRIGGER1, LOW); // Set trigger LOW for continuous read
  pinMode(TRIGGER2, OUTPUT); // Set pin 2 as trigger pin
  pinMode(MONITOR2, INPUT); // Set pin 3 as monitor pin
  digitalWrite(TRIGGER1, LOW); // Set trigger LOW for continuous read
  digitalWrite(TRIGGER2, LOW); // Set trigger LOW for continuous read
  pinMode(TEST_LED_PIN, OUTPUT);
}

void loop()
{
  
  pulse_width1 = pulseIn(MONITOR1, HIGH); // Count how long the pulse is high in microseconds
  pulse_width1 = pulse_width1/10; // 10usec = 1 cm of distance for LIDAR-Lite
  pulse_width2 = pulseIn(MONITOR2, HIGH); // Count how long the pulse is high in microseconds
  pulse_width2 = pulse_width2/10; // 10usec = 1 cm of distance for LIDAR-Lite
  Serial.println(pulse_width1);
  Serial.println(pulse_width2);

  if(pulse_width2<100 && pulse_width2<100)
    digitalWrite(TEST_LED_PIN, HIGH);
  else
    digitalWrite(TEST_LED_PIN, LOW);
 
  delay(20); //Delay so we don't overload the serial port
  
}

