#include <I2C.h>

//Setup Servos
#include <Servo.h>
 
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'
int Ki = 0;
int Kp = 1;
int Kd = .1;
float integral = 0;
float setpoint = 80;
float process_feedback = 0;
int dt = 100;
float previous_error = 0;
float lidar = 90;
float error = 0;
float derivative = 0;
float output= 0;
const int pwPin1 = 7;
const int pwPin2 = 5;
long pulse1, inches1, cm1,pulse2, inches2, cm2;


void setup()
{
  // Setup Wheels and ESC
  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
  /*  If you're re-uploading code via USB while leaving the ESC powered on, 
   *  you don't need to re-calibrate each time, and you can comment this part out.
   */
  calibrateESC();

  // Setup Lidar
  Serial.begin(9600); //Opens serial connection at 9600bps.     
  I2c.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data  
  I2c.timeOut(50); // Sets a timeout to ensure no locking up of sketch if I2C communication fails
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

/* Oscillate between various servo/ESC states, using a sine wave to gradually 
 *  change speed and turn values.
 */
void oscillate(){
  for (int i =0; i < 360; i++){
    double rad = degToRad(i);
    double speedOffset = sin(rad) * maxSpeedOffset;
    double wheelOffset = sin(rad) * maxWheelOffset;
    esc.write(90 + speedOffset);
    wheels.write(90 + wheelOffset);
    delay(50);
  }
}

void PID(){
  delay(dt);
  pinMode(pwPin1, INPUT);
  pinMode(pwPin2, INPUT);

  int distance1 = pulseIn(pwPin1, HIGH);
  int distance2 = pulseIn(pwPin2, HIGH); 
  Serial.print("From sensor 1:");
  Serial.println(distance1);
  Serial.print("From sensor 2:");
  Serial.println(distance2);
  error = distance2 - distance1;
  integral = integral + (error*dt);
  derivative = (error-previous_error)/dt;
  output = (Kp*error) + (Ki*integral) + (Kd*derivative);
  previous_error = error;
  esc.write(90 + output);
  wheels.write(20);
  Serial.println(output);
  
}

void loop()
{
   PID();
}
