#include <AccelStepper.h>

#define BAUDRATE 115200   //serial port baudrate
#define BUFFER_LEN 128       //Buffer length

#define Z_STEP_PIN 3
#define Z_DIR_PIN 6
#define Z_ENABLE_PIN 22
#define Z_MIN_PIN 24      //home endstop
#define Z_MAX_PIN 26      
#define HEATER_BED_PIN 8
#define TEMP_BED_PIN 14


char cmd_buffer[BUFFER_LEN];   //Buffer for cmd read from serial port
boolean done_read = false;     //the reading status of cmd_buffer
boolean done_check = false;    //the checksum status of command
boolean error_occur = false;   //the error status in working

const float setps_per_mm = 100;  //for the motor of motion stage

AccelStepper Zmotor(1, Z_STEP_PIN, Z_DIR_PIN);

//=========================Routines========================
//=========>>>Function: setup()<<<==============
void setup()
{
  Serial.begin(BAUDRATE);
  Serial.println("DONE: Initialization.");
  Zmotor.setMaxSpeed(100);
  Zmotor.setSpeed(10);
  Serial.println("Start working.");
}

//==========>>>Function: loop()<<<==============
void loop()
{
  read_cmd();    //read a command string from serial port
  check_sum();   //check the command string
  proc_cmd();    //process the command string
  Zmotor.runSpeed();
}

//==========>>>Function: read_cmd()<<<=============
void read_cmd()
{
  char cmd_char = NULL;
  int count_char = 0;
  unsigned int count_interval = 0;
  
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
      }
      else
      {
        cmd_buffer[count_char++] = cmd_char;
      }
      count_interval = 0;
    }
    count_interval += 1;
    if (count_interval > 60000){
      Serial.println("ERROR: in reading command.");
      error_occur = true;
      return;
    }
  }
}

//===============>>>Function: checksum()<<<===================
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
    Serial.println("ERROR: in checking command.");
    error_occur = true;
    return;
  }

  sum_got = strtod(&cmd_buffer[pointer - cmd_buffer + 1], NULL);
  sum_got = (int)sum_got;
  
  while ('*' != cmd_buffer[count])
    sum_comput = sum_comput^cmd_buffer[count++];
  if (sum_got != sum_comput){
    Serial.print("ERROR: occured in checksum process.");
    error_occur = true;
    return;
  }
  else
  {
    done_check = true;        //finish checking and return
    return;
  }
}

//================>>>Function: read_cmd()<<<==================
void proc_cmd()
{
  if (!done_read || !done_check || error_occur)
    return;

  
  Serial.println("DONE: this command");
  // excute the cmd and set ready_proc = false
  done_read = false;
  done_check = false;
  return;
}

