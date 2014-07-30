/*
This is a Arduino firmware program, for processing commands receiving
from Serial port.
The commands start with "four capital characters" with/without number
parameters floowing. Until now, the command set contains these commands:
HOME        # Homing the substrate motion stage. The Home postion is highest.
MOVE0.05    # Move the motion stage 0.05 mm downward.
GETP        # Get the current position of the motion stage.
SETT        # Set the target temperature.
GETT        # Get the current temperature.
RSET        # Reset the system status.
*/

#include <AccelStepper.h>

/***********************************CONSTANTS*********************************/
#define BAUDRATE     115200    //serial port baudrate
#define BUFFER_LEN   128       //Buffer length

#define STEP_PIN     3
#define DIR_PIN      6
#define HOME_PIN     24        //home endstop
#define SLEEP_PIN    26

#define STEPS_PER_MM 1000

//#define HEATER_PIN 8
//#define TEMP_PIN 14

/************************************parameters*******************************/
char cmd_buffer[BUFFER_LEN];   //Buffer for cmd read from serial port
int cmd_len = 0;               //Length of cmd string.
boolean done_read = false;     //the reading status of cmd_buffer
boolean done_check = false;    //the checksum status of command
boolean error_occur = false;   //the error status in working
int last_home_stat = LOW;      //the recent home status
int current_home_stat = LOW;
long targetPos = 0;

AccelStepper Motor(1, STEP_PIN, DIR_PIN);
/************************************Functions******************************/
void setup()
{
  Serial.begin(BAUDRATE);
  
  Motor.setMaxSpeed(1000);
  Motor.setAcceleration(10000);
  pinMode(HOME_PIN, INPUT);
  pinMode(SLEEP_PIN, OUTPUT);
  MotorSleep();
  
  last_home_stat = digitalRead(HOME_PIN);
  
  Serial.println("Initialization done.");
  Serial.println("READY");
}

void loop()
{
  read_cmd();    //read a command string from serial port
  check_sum();   //check the command string
  proc_cmd();    //process the command string
}

void read_cmd()
{
  char cmd_char = NULL;
  int count_char = 0;
  
  if (done_read || error_occur)
    return;
  
  while (true)
  {
    if (Serial.available()>0)
    {
      cmd_char = Serial.read();
      if (cmd_char == '\n' || cmd_char == '\r' || 
          cmd_char == EOF || count_char >= (BUFFER_LEN-1))
      {
        if (0 == count_char){ //return when empty line
          return; 
        }
        cmd_buffer[count_char] = 0; //terminate string
        Serial.print("Echo:");
        Serial.println(cmd_buffer);
        done_read = true;        //finish reading and return
        return;
      }else{
        cmd_buffer[count_char++] = cmd_char;
      }
    }
  }
}

void check_sum()
{
  int sum_comput = 0;
  int sum_got = 0;
  char *pointer = NULL;
  byte count = 0;
  
  if (!done_read || done_check || error_occur)
    return;
  
  pointer = strchr(cmd_buffer, '*');    //get the position of '*' in cmd string
  if (NULL == pointer){
    Serial.println("Failing in checking command.");
    Serial.println("RESEND");
    return;
  }

  sum_got = strtod(&cmd_buffer[pointer - cmd_buffer + 1], NULL);
  sum_got = (int)sum_got;
  
  while ('*' != cmd_buffer[count])
    sum_comput = sum_comput^cmd_buffer[count++];
    
  if (sum_got != sum_comput){
    Serial.print("Failing in checksum process.");
    Serial.println("RESEND");
    return;
  }else{
    *pointer = 0;
    cmd_len = pointer - cmd_buffer;
    done_check = true;        //finish checking and return
    return;
  }
}

void proc_cmd()
{
  if (!done_read || !done_check || error_occur)
    return;

  if (find_str("HOME")) {
    doHome();
  }
  else if (find_str("MOVE")) {
    Serial.println(cmd_buffer);
    float distance = find_value();
    Serial.print("Process MOVE:");
    Serial.println(distance);
  }
  else if (find_str("GETP")) {
    Serial.println("Process GETP.");
  }
  else if (find_str("SETT")) {
    float temperature = find_value();
    Serial.print("Process SETT:");
    Serial.println(temperature);
  }
  else if (find_str("GETT")) {
    Serial.println("Process GETT.");
  }
  else if (find_str("RSET")) {
    Serial.println("Process RSET.");
  }
  
  
  
  Serial.println("DONE: this command");
  // excute the cmd and set ready_proc = false
  done_read = false;
  done_check = false;
  Serial.println("READY");
  return;
}

boolean find_str(char string[])
{
  // Return True if the string was found.
  return (strstr(cmd_buffer, string) != NULL);
}

float find_value()
{
  return (strtod(cmd_buffer + 4, NULL));
}

long compute_steps(float dist)
{
  return (long)(dist*STEPS_PER_MM);
}

void doHome()
{
  MotorWake();
  Motor.setSpeed(-500);
  while (true){
    Motor.runSpeed();
    if (digitalRead(HOME_PIN) == HIGH){
      Motor.stop();
      Serial.println("HOME successfully.");
      break;
    }
  }
  Serial.println(Motor.currentPosition());
  Motor.setCurrentPosition(0);
  Serial.println(Motor.currentPosition());
  MotorSleep();
}

inline void MotorSleep()
{
  digitalWrite(SLEEP_PIN, LOW);
}

inline void MotorWake()
{
  digitalWrite(SLEEP_PIN, HIGH);
}
