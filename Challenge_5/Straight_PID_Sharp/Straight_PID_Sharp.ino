//Setup Servos
#include <Servo.h>
 
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'
int Ki = 0;
int Kp = 1.5;
int Kd = 0;
int Kp_s = 2;
float integral = 0;
float setpoint = 80;
float process_feedback = 0;
int dt = 200;
float previous_error = 0;
float lidar = 90;
float error = 0;
float derivative = 0;
float output= 0;

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
  Serial.begin(9600); //Opens serial connection at 9600bps.     8
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
  
  int distance_1 = analogRead(5);  // Shift high byte [0] 8 to the left and add low byte [1] to create 16-bit int

  int distance_2 = analogRead(4);  // Shift high byte [0] 8 to the left and add low byte [1] to create 16-bit int

  
  Serial.println(distance_1);
  Serial.println(distance_2);

  
  error = distance_1-distance_2;
  integral = integral + (error*dt);
  derivative = (error-previous_error)/dt;
  output = (Kp*error) + (Ki*integral) + (Kd*derivative);
  previous_error = error;
  esc.write(90 + output);
  output_s = Kp_s*abs(error);
  wheels.write(180-output_s);
  Serial.println(output);
  
}

void loop()
{
   PID();
}


