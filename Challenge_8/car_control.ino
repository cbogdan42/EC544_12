#include <Servo.h>
#include <SoftwareSerial.h>

//Ultrasonic sensors
#define pwPin1 10       //INPUT
#define pwPin2 11

//Crawler pins
#define WHEELS_PIN 8
#define ESC_PIN 9

//Threshold for distance difference
#define max_threshold 90
#define min_threshold 40

//XBEE pins
#define xbee_RX  2
#define xbee_TX  3

//LIDAR pins
#define LIDAR_OUTPUT 6
#define LIDAR_INPUT 7


//Front sensor distance
#define FRONT_MAX_DISTANCE 60

Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!

int startupDelay = 1000; // time to pause at each calibration step

double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'

float distance1;
float distance2;
float old_distance1;
float old_distance2;
float pulse1;
float pulse2;

int ultrasonic_flag=1;

float distance_front;
float Ki = 0;
float Kp = 1;
float Kd = 0.1;


int dt = 100;

float previous_error = 0;
float error = 0;
float integral = 0;
float derivative = 0;
float output= 0;

long pulse, inches, cm;
int arg;

SoftwareSerial xbee(2, 3); // RX, TX

void setup()
{
  // Setup Wheels and ESC

  wheels.attach(WHEELS_PIN); // initialize wheel servo to Digital IO Pin #8
  esc.attach(ESC_PIN); // initialize ESC to Digital IO Pin #9

  /*  If you're re-uploading code via USB while leaving the ESC powered on, 
   *  you don't need to re-calibrate each time, and you can comment this part out.
   */
 
  Serial.begin(9600); //Opens serial connection at 9600bps.     
  xbee.begin(9600);

  pinMode(pwPin1, INPUT);
  pinMode(pwPin2, INPUT);  

  pinMode(LIDAR_OUTPUT, OUTPUT); // Set pin 2 as trigger pin
  pinMode(LIDAR_INPUT, INPUT); // Set pin 3 as monitor pin
  digitalWrite(LIDAR_OUTPUT, LOW); // Set trigger LOW for continuous read

  arg = 1;  
  calibrateESC();
}


/* Calibrate the ESC by sending a high signal, then a low, then middle.*/
void calibrateESC()
{
    esc.write(180); // full backwards
    delay(startupDelay);
    esc.write(0); // full forwards
    delay(startupDelay);
    esc.write(90); // neutral
    delay(startupDelay);
    esc.write(90); // reset the ESC to neutral (non-moving) value
}

int check_front_sensor()
{
  distance_front = pulseIn(LIDAR_INPUT, HIGH)/10; // Count how long the pulse is high in microseconds;
  if (distance_front < FRONT_MAX_DISTANCE)
  {
    delay(100);
    //read lidar again
    distance_front = pulseIn(LIDAR_INPUT, HIGH)/10;
    if(distance_front < FRONT_MAX_DISTANCE)
      return 0;
    else
      return 1;   //false alarm
  }
  return 1;
}
void PID(int arg)
{
  if(arg)
  { 
    ultrasonic_flag = check_front_sensor();      

    if(!ultrasonic_flag)
    {
      wheels.write(90);
      esc.write(90);
      Serial.println("STOP");
    }

    else
    {
      //Serial.println("GO");      
      wheels.write(75);  

      pulse1 = pulseIn(pwPin1, HIGH);
      distance1 = (pulse1*2.54)/149;
      pulse2 = pulseIn(pwPin2, HIGH);
      distance2 = (pulse2*2.54)/149;
      
      /*if(abs(distance2-distance1)>max_threshold)        //Don't consider sensor readings if the difference in distance between the two sensors is greater than a particular value
      {
        esc.write(90);                  
      }

      else
      {
        if(distance1 > 1.8*old_distance1 && distance1 < 3*old_distance1)
          distance1 = old_distance1;
        if(distance2 > 2*old_distance2 && distance2 < 3*old_distance2)
          distance2 = old_distance2;
        
      }*/

      if(distance2 > distance1 + max_threshold)        //Don't consider sensor readings if the difference in distance between the two sensors is greater than a particular value
      {
        distance2 = distance1;      
      }

      if(distance1 >distance2 + max_threshold)        //Don't consider sensor readings if the difference in distance between the two sensors is greater than a particular value
      {
        distance1 = distance2;      
      }      

/*      Serial.println("After");
      Serial.print("Distance1:");
      Serial.println(distance1);
      Serial.print("Distance2:");
      Serial.println(distance2); 
*/
      error = distance2 - distance1;    

      integral = integral + (error*dt);
      derivative = (error-previous_error)/dt;
      output = (Kp*error) + (Ki*integral) + (Kd*derivative);
      previous_error = error;
      
      esc.write(90 - output);
    }
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
