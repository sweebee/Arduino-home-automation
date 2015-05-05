#include <SPI.h>
#include <MySensor.h>  
#include <readVcc.h>

//********** CONFIG **********************************

    const int NODE_ID = 3;       // ID of node
    const int CHILD_ID = 1;      // ID of sensor
    const int LIGHT_PIN = 3;       // Pin connected to the PIR
    
    unsigned long SLEEP_TIME = 120000; // Sleep time between reads
    
    boolean BATTERY_SENSOR = true; // Set to false to disable the battery sensor
    const int MIN_V = 2400; // empty voltage (0%)
    const int MAX_V = 3200; // full voltage (100%)

//****************************************************

MySensor node;
MyMessage msg(CHILD_ID, V_LIGHT_LEVEL);
int lastLightLevel;
int oldBatteryPcnt = 0;

void setup()  
{ 
  node.begin(NULL, NODE_ID, false);

  // Register all sensors to gateway (they will be created as child devices)
  node.present(CHILD_ID, S_LIGHT_LEVEL);
}

void loop()      
{     
  
  // Measure battery
  if(BATTERY_SENSOR == true) {
    int batteryPcnt = min(map(readVcc(), MIN_V, MAX_V, 0, 100), 100); // Convert voltage to percentage
    
    if (batteryPcnt != oldBatteryPcnt) { // If battery percentage has changed
      node.sendBatteryLevel(batteryPcnt); // Send battery percentage to gateway
      oldBatteryPcnt = batteryPcnt;
    }
  }

  // Do sensor things
  int lightLevel = (1023-analogRead(LIGHT_PIN))/10.23; 
  Serial.println(lightLevel);
  if (lightLevel != lastLightLevel) {
      node.send(msg.set(lightLevel));
      lastLightLevel = lightLevel;
  }
  node.sleep(SLEEP_TIME);
}



