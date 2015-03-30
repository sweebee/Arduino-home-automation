// MySensors PIR sensor
// Based on Attiny85
// Author: Wiebe Nieuwenhuis
//
//                    +-\/-+
//  ORANGE   CE      1|o   |8  VCC   RED
//  YELLOW   CSN     2|    |7  SCK   GREEN
//  -        SENSOR  3|    |6  MOSI  BLUE
//  BLACK    GND     4|    |5  MISO  VIOLET
//                    +----+

#include <MySensor.h>
#include <Bounce2.h>

#define SENSOR_INFO "PIR sensor"
#define NODE_ID 1
#define CHILD_ID 1
#define INPUT_PIN 4

MySensor gw;
Bounce debouncer = Bounce(); 
int oldValue = -1;

MyMessage msg(NODE_ID, V_TRIPPED);

void setup()
{
  pinMode(INPUT_PIN, INPUT);
  digitalWrite(INPUT_PIN, HIGH);
  
  debouncer.attach(INPUT_PIN);
  debouncer.interval(5);
  
  gw.begin(NULL, NODE_ID, false, 0);
  gw.sendSketchInfo(SENSOR_INFO, "1.0");
  gw.present(CHILD_ID, S_DOOR);
}


void loop()
{
  debouncer.update();
  int value = debouncer.read();
  if (value != oldValue) {
     gw.send(msg.set(value==HIGH ? 1 : 0));
     oldValue = value;
  }
}


