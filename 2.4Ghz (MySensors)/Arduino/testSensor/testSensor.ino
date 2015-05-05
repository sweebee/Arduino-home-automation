#include <MySensor.h>
#include <SPI.h>

#define SENSOR_INFO "Test sensor"
#define NODE_ID 10
#define CHILD_ID 1
#define OPEN 1
#define CLOSE 0
 
MySensor gw;
MyMessage msg(CHILD_ID, V_TRIPPED);
 
void setup() 
{ 
  gw.begin(NULL, NODE_ID, false);
  gw.sendSketchInfo(SENSOR_INFO, "1.0");
  gw.present(CHILD_ID, S_DOOR); 
}
 
void loop()
{
     gw.send(msg.set(OPEN)); 
  
     delay(10000); // Wait 10 seconds
     gw.send(msg.set(CLOSE));

     delay(10000); // Wait 10 seconds
}
