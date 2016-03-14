#include <SPI.h>
#include <MySensor.h>  
#include <readVcc.h>

//********** CONFIG **********************************

    #define NODE_ID AUTO         // ID of node
    #define CHILD_ID 1           // ID of sensor
    #define LIGHT_PIN A0         // Pin connected to the PIR
    
    #define MAX_LIGHT 500        // Max light value of LDR (100%)
    #define INT_PULL false       // Use internal Pull-up resistor
    #define SLEEP_TIME 60000     // Sleep time between reads
    
    #define MIN_V 2000           // empty voltage (0%)
    #define MAX_V 3200           // full voltage (100%)

//****************************************************

MySensor node;
MyMessage msg(CHILD_ID, V_LIGHT_LEVEL);

int lastLightLevel;
int oldBatteryPcnt = 0;
int forceCnt = 0;
boolean forceSend = false;

void setup()  
{ 
  node.begin(NULL, NODE_ID, false);
  node.sendSketchInfo("Light sensor","1.1");
  node.present(CHILD_ID, S_LIGHT_LEVEL);
  if(INT_PULL){
    pinMode(LIGHT_PIN, HIGH);
  }
}

void loop()      
{ 
  if(forceCnt < 30){
    forceCnt++;
    forceSend = false;
  } else {
    forceCnt = 0;  
    forceSend = true;    
  }
  sendLight(forceSend);
  sendBattery(forceSend);
  node.sleep(SLEEP_TIME);
}

// FUNCTIONS

void sendLight(boolean forceSend) // SEND LIGHT LEVEL
{
  float light = analogRead(LIGHT_PIN);
  int lightLevel = max(map(light, MAX_LIGHT, 0, 0, 100), 0); // Convert light to percentage
  if (lightLevel != lastLightLevel || forceSend) {
      node.send(msg.set(lightLevel));
      lastLightLevel = lightLevel;
  }
}

void sendBattery(boolean forceSend) // SEND BATTERYLEVEL
{
  int batteryPcnt = min(map(readVcc(), MIN_V, MAX_V, 0, 100), 100); // Get VCC and convert to percentage      
  if (batteryPcnt != oldBatteryPcnt || forceSend) { // If battery percentage has changed
    node.sendBatteryLevel(batteryPcnt); // Send battery percentage to gateway
    oldBatteryPcnt = batteryPcnt;
  }
}
