#include <MySensor.h>
#include <SPI.h>

//********** CONFIG **********************************

    #define SENSOR_INFO "Test sensor"
    #define NODE_ID 100
    #define CHILD_ID 1
    #define OPEN 1
    #define CLOSE 0

// ****************************************************
 
MySensor gw;
MyMessage msg(CHILD_ID, V_TRIPPED);
 
void setup() 
{ 
  gw.begin(NULL, NODE_ID, false);
  gw.sendSketchInfo(SENSOR_INFO, NULL);
  gw.present(CHILD_ID, S_DOOR); 
}
 
void loop()
{
  gw.send(msg.set(OPEN),true);
  gw.sleep(2000);
     
  gw.send(msg.set(CLOSE),false);
  gw.sleep(2000);
}
