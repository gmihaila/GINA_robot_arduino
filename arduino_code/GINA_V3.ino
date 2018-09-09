/*
* GINA Control arm GUI + independent. All components connected
* Release under George Mihaila permission
* (c)2015 George M. Home programming
*/

/*
 NOTES:
 - potentiometer1 analog values 4-1021 (polarity: - Analog0 +)
*/

//libraries:
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <PID_v1.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

//define driver dc motor 1
#define In1A 5
#define In1B 6
//define driver dc motor 2
#define In2A 9
#define In2B 10

//servo pulse width min and max
#define SERVOMIN  100 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  400 // this is the 'maximum' pulse length count (out of 4096)
//I@C lcd address
#define I2C_ADDR    0x3F  

//global variables
uint8_t servo_pos1 = 50;  //servo position 1(platform) in degrese(0-180)
uint8_t servo_pos2 = 90;  //servo position 2(claw rotation) in degrese(0-180)
uint8_t servo_pos3 = 90;  //servo position 3(calw) in degrese(0-180)
double Setpoint, Input, Output; //PID variables
double Kp=1, Ki=0.00025, Kd=0.00000000000001; //tuning parameters
boolean finish = false;   //if the arm is in desire position finish = true

//using Adafruit: called this way it uses the default address ox40
Adafruit_PWMServoDriver servo_pwm = Adafruit_PWMServoDriver();
//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
//define LCD function
LiquidCrystal_I2C	lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

void setup()
{
  Serial.begin(9600);
  lcd.begin (16,2);            //start LCD
  lcd.print("Welcome! My name");  
  lcd.setCursor ( 0, 1 );        
  lcd.print("is  GINA!"); 
  DDRD = B11111110;          //set digital pins 7-1 output pin0 input
  DDRB = B11111111;          //set digital pins 13-8 output
  Serial.begin(9600);        //start comunication over serial bus at 9600bit/s
  servo_pwm.begin();         //start PWM for servos
  servo_pwm.setPWMFreq(50);  // MG995 servo runs at 50Hz
  //initialize position servomotors 1,2,3...........................
  servo_pwm.setPWM(0 , 0 , map(servo_pos1, 0, 180, SERVOMIN, SERVOMAX));
  servo_pwm.setPWM(1 , 0 , map(servo_pos2, 0, 180, SERVOMIN, SERVOMAX));
  servo_pwm.setPWM(2 , 0 , map(servo_pos3, 0, 180, SERVOMIN, SERVOMAX));
  //initialize the variables we're linked to PID....................
  Input = analogRead(0);
  myPID.SetMode(AUTOMATIC);  //turn the PID on
  myPID.SetOutputLimits(50 , 200);
  myPID.SetSampleTime(100);
  delay(2000);
}

void loop()
{  
  servo_move(0 , 1 , 80 , &servo_pos1);
  delay(1000);
  DCmotor_move(0 , 85);
  delay(2000);
  DCmotor_move(0 , 20);
  servo_move(0 , 1 , 50 , &servo_pos1);
  delay(1000);
  DCmotor_move(0 , 90);
  delay(2000);
  DCmotor_move(0 , 20);
  servo_move(0 , 1 , 10 , &servo_pos1);
  delay(1000);
  DCmotor_move(0 , 85);
  delay(2000);
  DCmotor_move(0 , 20);
  
  //lock arm  
  //DCmotor_move(0 , 95);
}

//function controle all movements
void arm_move(uint8_t servo1 , uint8_t dc1)
{
  
}
//function control SERVOS variables #servo(0,1,2) , speed gear(1,2,3) , position(degrese 0-180 , &address current position)....................
void servo_move(uint8_t pwmnum , uint8_t spd , uint8_t pos , uint8_t *servo_pos)
{
  if(pos > *servo_pos)
  {
  for(uint8_t temp = *servo_pos; temp < pos; temp++)
  {
    uint16_t pulselength = map(temp, 0, 180, SERVOMIN, SERVOMAX); //calibrate min-max position to 0-180 degrese
    servo_pwm.setPWM(pwmnum , 0 , pulselength);
    delay((4-spd)*10);
    LCD_display('SERVO' , pwmnum , temp , pulselength);   //display real time position
  }
  //help reduce servo noise-stabilize position
  servo_pwm.setPWM(pwmnum , 0 , map(pos, 0, 180, SERVOMIN, SERVOMAX)+4);
  delay(2);
  servo_pwm.setPWM(pwmnum , 0 , map(pos, 0, 180, SERVOMIN, SERVOMAX)-4); 
  }
  else if(pos < *servo_pos)
  {
  for(uint8_t temp = *servo_pos; temp >pos; temp--)
  {
    uint16_t pulselength = map(temp, 0, 180, SERVOMIN, SERVOMAX); //calibrate min-max position to 0-180 degrese
    servo_pwm.setPWM(pwmnum , 0 , pulselength);
    delay((4-spd)*10);
    LCD_display('SERVO' , pwmnum , temp , pulselength);   //display real time position
  }
  //help reduce servo noise-stabilize position
  servo_pwm.setPWM(pwmnum , 0 , map(pos, 0, 180, SERVOMIN, SERVOMAX)+4);
  delay(2);
  servo_pwm.setPWM(pwmnum , 0 , map(pos, 0, 180, SERVOMIN, SERVOMAX)-4);
  }
  *servo_pos = pos;  //update current position
}

//function PID controll DC motors: #DCmotor:1-2 , final position (read position from #DC motor AnalogRead(0)/analogRead(1)......................
void DCmotor_move(uint8_t motornum , uint8_t set)
{
  
  Setpoint = set;
  Input = map(analogRead(motornum) , 5 , 1021 , 0 , 180); //set input to degrese 0-180
  while(set > Input)
  {
    myPID.SetMode(AUTOMATIC);
    myPID.SetControllerDirection(DIRECT);
    //we're accelerating to setpoint
    myPID.SetTunings(Kp, Ki, Kd);
    myPID.Compute(); //calculate output PID
    //drive motor forward
    digitalWrite(5+motornum*4 , LOW);
    analogWrite(6+motornum*4 , Output);
    Input = map(analogRead(motornum) , 10 , 1015 , 0 , 180); //set input to degrese 0-180
    LCD_display('DCmotor', motornum , Input , Output);       //Real time positioning 
    digitalWrite(5+motornum*4 , LOW);
    digitalWrite(6+motornum*4 , LOW);   
    delay(5);
  }
  while(set < Input)
  {
    myPID.SetMode(AUTOMATIC);
    myPID.SetControllerDirection(REVERSE);
    //we're accelerating to setpoint
    myPID.SetTunings(Kp, Ki, Kd);
    myPID.Compute(); //calculate output PID
    //drive motor backwards
    digitalWrite(6+motornum*4 , LOW);
    analogWrite(5+motornum*4 , Output);
    Input = map(analogRead(motornum) , 10 , 1015 , 0 , 180); //set input to degrese 0-180
    LCD_display('DCmotor', motornum , Input , Output);       //Real time positioning
    digitalWrite(5+motornum*4 , LOW);
    digitalWrite(6+motornum*4 , LOW);
    delay(5);
   }
}
//function display real time position name: servo / dcmot............................................................................................
void LCD_display(char name , uint8_t motornum , uint8_t pos , uint16_t output)
{
  String str = String(name);
  switch(name)
  {
    case 'SERVO':
    {
      lcd.clear();
      lcd.home();
      lcd.print("SERVO");
      lcd.setCursor(7 , 0);
      lcd.print(motornum);
      lcd.setCursor(9 , 0);
      lcd.print("pos");
      lcd.setCursor(12 , 0);
      lcd.print(pos);
      lcd.print((char)223);
      lcd.setCursor(0 , 1);
      lcd.print("Output: ");
      lcd.print(output);
      break;
    }
    case 'DCmotor':
    {
      lcd.clear();
      lcd.home();
      lcd.print("DCmotor");
      lcd.setCursor(7 , 0);
      lcd.print(motornum);
      lcd.setCursor(9 , 0);
      lcd.print("pos");
      lcd.setCursor(12 , 0);
      lcd.print(pos);
      lcd.print((char)223);
      lcd.setCursor(0 , 1);
      lcd.print("Output: ");
      lcd.print(output);
      break;
    }
    default:
    {
      lcd.clear();
      lcd.home ();                  
      lcd.print("1-");
      lcd.print(map(servo_pos1, SERVOMIN, SERVOMAX , 0 , 180));
      lcd.setCursor(5,0);
      lcd.print("2-");
      lcd.print(map(analogRead(0) , 10 , 1015 , 0 , 180));
      lcd.setCursor(11,0);
      lcd.print("3-");
      lcd.print(analogRead(1));
      lcd.setCursor(0,1);
      lcd.print("4-");
      lcd.print(servo_pos2);
      lcd.setCursor(5,1);
      lcd.print("5-");
      lcd.print(servo_pos3);
      lcd.setCursor(11,1);
      lcd.print("6-");
      lcd.print("GINA");
      break;
    }
  }
}
