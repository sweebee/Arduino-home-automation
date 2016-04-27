#include <MySensor.h>
#include <SPI.h>

#define NODE_ID 52
#define CHILD_ID 1
#define ALARM_PIN  3  // Arduino Digital I/O pin number for first relay (second on pin+1 etc)
#define ON 1  // GPIO value to write to turn on attached relay
#define OFF 0 // GPIO value to write to turn off attached relay

MySensor node;

void setup()  
{   
  node.begin(incomingMessage, NODE_ID, false);
  node.present(CHILD_ID, S_LIGHT);
  pinMode(ALARM_PIN, OUTPUT);   
}


void loop() 
{
  node.process();
}

void incomingMessage(const MyMessage &message) {
  if (message.type==V_LIGHT && message.sensor == CHILD_ID) {
     digitalWrite(ALARM_PIN, message.getBool()?ON:OFF);
     Serial.print("Alarm: ");
     Serial.println(message.getBool());
   } 
}

