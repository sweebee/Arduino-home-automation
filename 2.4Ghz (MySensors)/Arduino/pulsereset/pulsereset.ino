#include <MySensor.h>
#include <SPI.h>

#define SENSOR_INFO "Test sensor"
#define NODE_ID 8
#define CHILD_ID 1
#define pulseCount 614826
 
MySensor gw;
MyMessage pcMsg(CHILD_ID,V_VAR1);
 
void setup() 
{ 
  gw.begin(NULL, NODE_ID, false);
  gw.sendSketchInfo(SENSOR_INFO, "1.0");
  gw.present(CHILD_ID, S_DOOR); 
}
 
void loop()
{
     gw.send(pcMsg.set(pulseCount)); 
     gw.sleep(10000);
}
