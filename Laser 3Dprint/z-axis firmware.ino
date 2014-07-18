
#define BAUDRATE 115200   //serial port baudrate

const int BUFFER_LEN = 128;    //Buffer length
char cmd_buffer[BUFFER_LEN];   //Buffer for cmd read from serial port
char cmd_char;      // One character in cmd_buffer
int char_count;     // Count # of chars in cmd_buffer

boolean ready_proc = false; //if the cmd in buffer is ready to process

//=========================Routines========================
//=========>>>Function: setup()<<<==============
void setup()
{
  Serial.begin(BAUDRATE);
  Serial.println("Arduino ready!");
}

//==========>>>Function: loop()<<<==============
void loop()
{
  read_cmd();
  proc_cmd();
}

//==========>>>Function: read_cmd()<<<=============
inline void read_cmd()
{
  while (Serial.available()>0 && false == ready_proc)
  {
    cmd_char = Serial.read();
    if (cmd_char == '\n' || cmd_char == '\r' || 
        cmd_char == ':' || char_count >= (BUFFER_LEN-1))
    {
      if (0==char_count) return; //return when empty line
      cmd_buffer[char_count] = 0; //terminate string
      Serial.print("Echo:");
      Serial.println(&cmd_buffer[0]);
      char_count = 0;
      ready_proc = true;
    }
    else
      cmd_buffer[char_count++] = cmd_char;
  }
}

//================>>>Function: read_cmd()<<<==================
inline void proc_cmd()
{
  if (false == ready_proc){
    return;
  }
  
  // excute the cmd and set ready_proc = false
  ready_proc = false;
  return;
}
