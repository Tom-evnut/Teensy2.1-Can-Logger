#include <Arduino.h>
#include <FlexCAN.h> //https://github.com/teachop/FlexCAN_Library 

unsigned char mes[8] = {0, 0, 0, 0, 0, 0, 0, 0};
char msgString[128];                        // Array to store serial string

const int led = 13;

CAN_message_t msg;
CAN_message_t inMsg;
CAN_filter_t filter;

void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
  Can0.begin(500000);

  //set filters for standard
  for (int i = 0; i < 8; i++)
  {
    Can0.getFilter(filter, i);
    filter.flags.extended = 0;
    Can0.setFilter(filter, i);
  }
  //set filters for extended
  for (int i = 9; i < 13; i++)
  {
    Can0.getFilter(filter, i);
    filter.flags.extended = 1;
    Can0.setFilter(filter, i);
  }

  Serial.begin(115200);
  delay(4000);
  Serial.println("Starting up!");
  Serial.println("SimpBMS V2 CanLogger");
  
  Serial.println("Time Stamp,ID,Extended,LEN,D1,D2,D3,D4,D5,D6,D7,D8");
}

void loop() {
  // put your main code here, to run repeatedly:
  while (Can0.available())
  {
    canread();
  }
}

void canread()
{
  digitalWrite(led, HIGH);
  Can0.read(inMsg);
  Serial.print(millis());
  if ((inMsg.id & 0x80000000) == 0x80000000)    // Determine if ID is standard (11 bits) or extended (29 bits)
    sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (inMsg.id & 0x1FFFFFFF), inMsg.len);
  else
    sprintf(msgString, ",0x%.3lX,false,%1d", inMsg.id, inMsg.len);

  Serial.print(msgString);

  if ((inMsg.id & 0x40000000) == 0x40000000) {  // Determine if message is a remote request frame.
    sprintf(msgString, " REMOTE REQUEST FRAME");
    Serial.print(msgString);
  } else {
    for (byte i = 0; i < inMsg.len; i++) {
      sprintf(msgString, ", 0x%.2X", inMsg.buf[i]);
      Serial.print(msgString);
    }
  }
  Serial.println();
  digitalWrite(led, LOW);
}


