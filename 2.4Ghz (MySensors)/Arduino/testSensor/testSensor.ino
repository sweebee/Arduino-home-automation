#include <MySensor.h>
#include <SPI.h>

#define SENSOR_INFO "Test sensor"
#define NODE_ID 100
#define FORECAST "Zonnig"
#define TEMP 10.2
#define HUM 56
#define PRESSURE 1045
#define RAIN 1.8
#define LUX 854
 
MySensor gw;
MyMessage msg0(0, V_FORECAST);
MyMessage msg1(1, V_TEMP);
MyMessage msg2(2, V_HUM);
MyMessage msg3(3, V_PRESSURE);
MyMessage msg4(4, V_LIGHT_LEVEL);
MyMessage msg5(5, V_LIGHT_LEVEL);

void setup() 
{ 
  gw.begin(NULL, NODE_ID, false);
}
 
void loop()
{
     gw.send(msg0.set(FORECAST));    
     gw.send(msg1.set(TEMP,1));
     gw.send(msg2.set(HUM));
     gw.send(msg3.set(PRESSURE));
     gw.send(msg4.set(RAIN,1));
     gw.send(msg5.set(LUX));
     gw.sleep(10000);
}
