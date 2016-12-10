#include <Servo.h>
#include <SoftwareSerial.h>



//Pin connections
//XBEE pins
#define xbee_RX  2
#define xbee_TX  3

//LIDAR pins
#define LIDAR_OUTPUT 6
#define LIDAR_INPUT 7

//Crawler pins
#define WHEELS_PIN 8		//speed
#define ESC_PIN 9			//Steering

//Ultrasonic sensors
#define pwPin1 10       //Left sensor
#define pwPin2 11		//Right sensor

#define throttle_pin 4
#define reverse_pin	 5
#define left_pin 12
#define right_pin 13

//Constants used in code
//Front sensor distance
#define FRONT_MIN_DISTANCE 70
#define FRONT_MAX_DISTANCE 200

#define turn_timeout 20000

//Threshold for distance difference
#define max_threshold 95
#define min_threshold 40


Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!

int startupDelay = 1000; // time to pause at each calibration step

double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'

float distance1;
float distance2;
float distance_temp;
float pulse1;
float pulse2;
float pulse_temp;

int ultrasonic_flag=1;

float distance_front;
float Ki = 0;
float Kp = 1.0;
float Kd = 0.1;


int dt = 100;

float previous_error = 0;
float error = 0;
float integral = 0;
float derivative = 0;
float output= 0;

long pulse, inches, cm;
int arg;

unsigned long old_time, new_time;

int speed[7] = {75,90,105,90,75,90,105};
int directions[7] = {45,90,135,90,45,90,135};

int turn_right;
int turn_left;


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


  pinMode(throttle_pin, INPUT);      
  pinMode(reverse_pin, INPUT);      
  pinMode(left_pin, INPUT);      
  pinMode(right_pin, INPUT);      

  pinMode(LIDAR_OUTPUT, OUTPUT); // Set pin 2 as trigger pin
  pinMode(LIDAR_INPUT, INPUT); // Set pin 3 as monitor pin
  digitalWrite(LIDAR_OUTPUT, LOW); // Set trigger LOW for continuous read

  arg = 1;  
  old_time = 0;
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

void reverse_and_turn()
{
  int esc_value;
  //Get directions
  pulse1 = pulseIn(pwPin1, HIGH);
  distance1 = (pulse1*2.54)/149;
  pulse2 = pulseIn(pwPin2, HIGH);
  distance2 = (pulse2*2.54)/149;

  //reverse
  //Serial.println("Inside reverse and turn");
  esc.write(90);
  wheels.write(110);
  delay(3000);
  wheels.write(90);

  //turn
  esc.write(145);
  wheels.write(70);
  delay(3000);
  esc.write(90);
  wheels.write(90);
}

int check_front_sensor()
{
  distance_front = pulseIn(LIDAR_INPUT, HIGH)/10; // Count how long the pulse is high in microseconds;
  //Serial.println(distance_front);
  if (distance_front < FRONT_MIN_DISTANCE)
  {
    delay(100);
    //read lidar again
    distance_front = pulseIn(LIDAR_INPUT, HIGH)/10;
    if(distance_front < FRONT_MIN_DISTANCE)
      reverse_and_turn();
    return 1;
  }
  else if(distance_front > FRONT_MAX_DISTANCE)
  {
  	//check side distance
  	pulse1 = pulseIn(pwPin1, HIGH);
  	distance1 = (pulse1*2.54)/149;
  	
  	pulse2 = pulseIn(pwPin2, HIGH);
  	distance2 = (pulse2*2.54)/149;
        
  	if(distance1>FRONT_MAX_DISTANCE)
  	{
  		//turn left
        //Serial.println("Turn left");
        
        new_time = millis();
		if(old_time == 0)				//update old_time only once
        	old_time = new_time-(turn_timeout+1);

        if(new_time-old_time > turn_timeout)
	  	{
	  		esc.write(90);
	  		wheels.write(75);
	  		delay(1500);
	  		esc.write(90);
	  		wheels.write(90);
	  		reverse_and_turn();
	  		old_time = new_time;
	  	}
  	}  
  }
  return 1;
}
void PID(int arg)
{
  switch(arg)
  {
   
    case 0:
    {
      Serial.println("Remote stop");
      esc.write(90);
      wheels.write(90);
      break;
    }
    case 1:
    {  
      //Serial.println("Normal");
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
        
        Serial.println("Before");
        Serial.print("Distance1:");
        Serial.println(distance1);
        Serial.print("Distance2:");
        Serial.println(distance2); 
        
        
        if(distance2 > distance1 + max_threshold)        //Don't consider sensor readings if the difference in distance between the two sensors is greater than a particular value
        {
          distance2 = distance1;      
        }

        if(distance1 >distance2 + max_threshold)        //Don't consider sensor readings if the difference in distance between the two sensors is greater than a particular value
        {
          distance1 = distance2;      
        }      

      
        Serial.println("After");
        Serial.print("Distance1:");
        Serial.println(distance1);
        Serial.print("Distance2:");
        Serial.println(distance2);
      

        error = distance2 - distance1;    

        integral = integral + (error*dt);
        derivative = (error-previous_error)/dt;
        output = (Kp*error) + (Ki*integral) + (Kd*derivative);
        previous_error = error;
        
        esc.write(90 - output);
      }
  
      break;
    }

    case 2:
    {
    	Serial.println("Waiting for directions");

    }

    default:
    {
      Serial.println("Invalid argument");
      esc.write(90);
      wheels.write(90);
      break;
    }
  }  
}

void loop()
{   

  arg = 1;

  PID(arg);
  //delay(1000);
}

