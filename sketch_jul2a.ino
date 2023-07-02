
#include <PS2X_lib.h>  
#include <Adafruit_PWMServoDriver.h> 
#include <Wire.h>

#define PS2_DAT 12
#define PS2_CMD 13
#define PS2_CLK 14
#define PS2_ATT 15

#define PWM_DC1A 14 
#define PWM_DC1B 15 
#define PWM_DC2A 8 
#define PWM_DC2B 9 
#define PWM_DC3A 10 
#define PWM_DC3B 11 
#define PWM_DC4A 12 
#define PWM_DC4B 13 

#define MOT_LEFT 1
#define MOT_RIGHT 2
#define SHOOTER_1 3

#define SPD_FAST 2047

#define PWM_SERVO0          2
#define PWM_SERVO1          3
#define PWM_SERVO2          4
#define PWM_SERVO3          5
#define PWM_SERVO4          6

#define SRV_360_T1          698 
#define SRV_360_T2          1362 
#define SRV_360_T3          1440 
#define SRV_360_T4          2104 

#define SRV_INTAKE          2
#define SRV_WHEEL           3

#define SPD_INTAKE          200
#define SPD_WHEEL           100

Adafruit_PWMServoDriver pwm;  
PS2X ps2;

void setup() { 
  Serial.begin(115200); 

  Serial.println("Mastodon's BanhMi Connected To PC. Proceeding to connect PS2 Controller");

  while (ps2.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT) != 0) {
    Serial.println("Err: PS2 Controller Not Connected");
    delay(500);
  } 

  Serial.println("Controller works normally");

  pwm.begin();                           
  pwm.setOscillatorFrequency(27000000);  
  pwm.setPWMFreq(50);   
             
}

void ctrl_dc(uint8_t motor, int16_t speed) {
  switch (motor) {
    case 1: 
      pwm.setPWM(PWM_DC1A, 0, ((speed > 0) ? speed : 0) );  
      pwm.setPWM(PWM_DC1B, 0, ((speed < 0) ? (-speed) : 0) );
      break;
    case 2: 
      pwm.setPWM(PWM_DC2A, 0, ((speed > 0) ? speed : 0) );
      pwm.setPWM(PWM_DC2B, 0, ((speed < 0) ? (-speed) : 0) ) ;
      break;
    case 3:  
      pwm.setPWM(PWM_DC3A, 0, ((speed > 0) ? speed : 0) );
      pwm.setPWM(PWM_DC3B, 0, ((speed < 0) ? (-speed) : 0) );
      break;
    case 4:
      pwm.setPWM(PWM_DC4A, 0, ((speed > 0) ? speed : 0) );
      pwm.setPWM(PWM_DC4B, 0, ((speed < 0) ? (-speed) : 0) );
      break;
  }
}

void ctrl_servo360(uint8_t motor, float speed) {
  uint16_t us = (SRV_360_T2 + SRV_360_T3) / 2;
  if(speed > 0) 
    us = SRV_360_T3 + (speed / 100) * (SRV_360_T4 - SRV_360_T3);
  else if(speed < 0) 
    us = SRV_360_T2 + (speed / 100) * (SRV_360_T2 - SRV_360_T1);
  switch(motor) {
    case 0: pwm.writeMicroseconds(PWM_SERVO0, us); break;
    case 1: pwm.writeMicroseconds(PWM_SERVO1, us); break;
    case 2: pwm.writeMicroseconds(PWM_SERVO2, us); break;
    case 3: pwm.writeMicroseconds(PWM_SERVO3, us); break;
    case 4: pwm.writeMicroseconds(PWM_SERVO4, us); break;
  }
}

bool intake_toggle = false;
bool wheel_toggle = false;
bool intake = false;
bool wheel = false; 

void loop() {

  ps2.read_gamepad(); 

  ctrl_dc(MOT_LEFT, map(ps2.Analog(PSS_LY), 0, 255, -SPD_FAST, SPD_FAST));
  ctrl_dc(MOT_RIGHT, map(ps2.Analog(PSS_RY), 0, 255, SPD_FAST, -SPD_FAST));

  if (ps2.Button(PSB_R1)){
      if(!intake_toggle) {
        intake_toggle = true;
        intake = !intake;
        ctrl_servo360(SRV_INTAKE, (intake) ?  SPD_INTAKE : 0);
    } else {intake_toggle = false;}}

  if (ps2.Button(PSB_R1)){
      if(!wheel_toggle) {
        wheel_toggle = true;
        wheel = !wheel;
        ctrl_servo360(SRV_WHEEL, (wheel) ?  SPD_WHEEL : 0);}
    } else {wheel_toggle = false;}}
  