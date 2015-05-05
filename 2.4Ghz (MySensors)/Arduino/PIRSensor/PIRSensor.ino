#include <MySensor.h>
#include <SPI.h>
#include <readVcc.h>

// ********** CONFIG **********************************

    const int NODE_ID = 1;             // ID of node
    const int CHILD_ID = 1;            // ID of sensor
    const int PIR_PIN = 3;             // Pin connected to the PIR
    
    unsigned long SLEEP_TIME = 0;      // Sleep time between reads (in milliseconds)
    
    boolean BATTERY_SENSOR = true;     // Set to false to disable the battery sensor
    const int MIN_V = 2400;            // empty voltage (0%)
    const int MAX_V = 3200;            // full voltage (100%)

// ****************************************************

MyMessage msg(CHILD_ID, V_TRIPPED);
MySensor node;

int oldBatteryPcnt = 0;
int sentValue = -1;

void setup()
{
  node.begin(NULL, NODE_ID, false);
  pinMode(PIR_PIN, INPUT);
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
  int value = digitalRead(PIR_PIN);
  Serial.println(value);
  if (value != sentValue) { // If status of PIR has changed
    node.send(msg.set(value == HIGH ? 1 : 0)); // Send PIR status to gateway
    sentValue = value;
  }
  
  node.sleep(PIR_PIN-2, CHANGE, SLEEP_TIME); // Sleep until something happens with the sensor
}
