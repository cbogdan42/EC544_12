//Setup Servos
#include <Servo.h>
#include <SoftwareSerial.h>


#define TRIGGER1 6
#define MONITOR1 7
#define TRIGGER2 4
#define MONITOR2 5
#define max_threshold 75

#define xbee_RX  2
#define xbee_TX  3



Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'
float distance1;
float distance2;
float old_distance1;
float old_distance2;
int ultrasonic_flag=1;
float distance_us;

float Ki = 0;
float Kp = 2;
float Kd = 0.3;


float Kp_s = .000000001;
float Kd_s = .00003;

float integral = 0;
float setpoint = 80;
float process_feedback = 0;
int dt = 100;
float previous_error = 0;
float lidar = 90;
float error = 0;
float derivative = 0;
float output= 0;
const int pwPin1 = 10;
long pulse, inches, cm;
int arg;
SoftwareSerial xbee(xbee_RX, xbee_TX); // RX, TX

void setup()
{
  // Setup Wheels and ESC
  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9

  /*  If you're re-uploading code via USB while leaving the ESC powered on, 
   *  you don't need to re-calibrate each time, and you can comment this part out.
   */

 
  Serial.begin(9600); //Opens serial connection at 9600bps.     
  
  //Setup Timer
//  Timer1.initialize(TIMER_US);                  // Initialise timer 1
//  Timer1.attachInterrupt( timerIsr );


  //Setup Lidar

  //Sensor 1
  pinMode(TRIGGER1, OUTPUT); // Set pin 2 as trigger pin
  pinMode(MONITOR1, INPUT); // Set pin 3 as monitor pin
  digitalWrite(TRIGGER1, LOW); // Set trigger LOW for continuous read


  //Sensor 2
  pinMode(TRIGGER2, OUTPUT); // Set pin 4 as trigger pin
  pinMode(MONITOR2, INPUT); // Set pin 5 as monitor pin
  digitalWrite(TRIGGER2, LOW); // Set trigger LOW for continuous read

  pinMode(pwPin1, INPUT);
  ultrasonic_flag = 1;
  xbee.begin(9600);
  calibrateESC();
  arg = 1;
}

/* Convert degree value to radians */
double degToRad(double degrees){
  return (degrees * 71) / 4068;
}

/* Convert radian value to degrees */
double radToDeg(double radians){
  return (radians * 4068) / 71;
}

/* Calibrate the ESC by sending a high signal, then a low, then middle.*/
void calibrateESC(){
    esc.write(180); // full backwards
    delay(startupDelay);
    esc.write(0); // full forwards
    delay(startupDelay);
    esc.write(90); // neutral
    delay(startupDelay);
    esc.write(90); // reset the ESC to neutral (non-moving) value
}

void PID(int arg){
  if(arg)
  { 
    ultrasonic_flag=1;
    delay(dt);
    pulse = pulseIn(pwPin1, HIGH);

    distance_us = (pulse*2.54)/149;
    //Serial.println(distance_us);

    if (distance_us < 65){
      delay(100);
      
      pulse = pulseIn(pwPin1, HIGH);
      distance_us = (pulse*2.54)/149;
    if (distance_us < 65)
      ultrasonic_flag=0;
    }
      
  //Crawler continues driving if obstacle not detected  
    if (ultrasonic_flag==0)
      {
        wheels.write(90);
        esc.write(90);
        Serial.println("STOP");
      }
    
    else
    {
      wheels.write(75);  
      distance1 = pulseIn(MONITOR1, HIGH)/10; // Count how long the pulse is high in microseconds
      distance2 = pulseIn(MONITOR2, HIGH)/10; // Count how long the pulse is high in microseconds
      if(distance1 > 150){
          distance1 = 100;
      }
      if(distance2 > 150){
          distance2 = 100;
      }
       
    
      Serial.print(distance2);
      Serial.print(" - ");
      Serial.print(distance1);
      Serial.print(" => ");
      Serial.print(error);
      Serial.print("\n");    
    
      if(abs(distance2-distance1)>max_threshold)        //Don't consider sensor readings if the difference in distance between the two sensors is greater than a particular value
      {
        esc.write(90);                  
      }
      else
      {
        if(distance1 > 1.8*old_distance1 && distance1 < 3*old_distance1)
          distance1 = old_distance1;
        if(distance2 > 2*old_distance2 && distance2 < 3*old_distance2)
          distance2 = old_distance2;
        
      }
    
      error = distance2 - distance1;    
    
      integral = integral + (error*dt);
      derivative = (error-previous_error)/dt;
      output = (Kp*error) + (Ki*integral) + (Kd*derivative);
      previous_error = error;
      
      esc.write(90 - output);

  }
  

  
  
//  float output_s = abs(error)*Kp_s + Ki*abs(error) + Kd_s*abs(error);
  }
   else
    {
      esc.write(90);
      wheels.write(90);
    }

}

void loop()
{
   if (xbee.available()) 
   {
     byte A = xbee.read();
     if(A==49)
      arg = 1;
     else
      arg = 0;
   }

  PID(arg);
}


//Timer ISR called here at each interrupt
void timerIsr()
{
  Serial.println("Timer ISR");  
}
