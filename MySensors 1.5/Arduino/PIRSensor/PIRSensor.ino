#include <MySensor.h>
#include <SPI.h>
#include <readVcc.h>

// ********** CONFIG **********************************

    #define NODE_ID AUTO          // ID of node
    #define CHILD_ID 1            // ID of sensor
    #define PIR_PIN 3             // Pin connected to the PIR
    
    #define MIN_V 2000            // empty voltage (0%)
    #define MAX_V 3200            // full voltage (100%)

// ****************************************************

MyMessage msg(CHILD_ID, V_TRIPPED);
MySensor node;

int oldBatteryPcnt;
int sentValue;
int forceSend = 0;

void setup()
{
  node.begin(NULL, NODE_ID, false);
  node.sendSketchInfo("PIR sensor", "1.1");
  node.present(CHILD_ID, S_MOTION);
  pinMode(PIR_PIN, INPUT);
  digitalWrite(PIR_PIN, HIGH);
}

void loop()
{
  sendPIR();     // Send PIR value
  sendBattery(); // Send batterylevel    
  node.sleep(PIR_PIN-2, CHANGE); // Sleep until something happens with the sensor
}

// FUNCTIONS

void sendPIR() // SEND PIR STATUS
{
  int value = digitalRead(PIR_PIN); // Get value of PIR
  if (value != sentValue) { // If status of PIR has changed
    node.send(msg.set(value)); // Send PIR status to gateway
    sentValue = value;
  }
}

void sendBattery() // SEND BATTERYLEVEL
{
  forceSend++;
  int batteryPcnt = min(map(readVcc(), MIN_V, MAX_V, 0, 100), 100); // Get VCC and convert to percentage      
  if (batteryPcnt != oldBatteryPcnt || forceSend >= 20) { // If battery percentage has changed
    node.sendBatteryLevel(batteryPcnt); // Send battery percentage to gateway
    oldBatteryPcnt = batteryPcnt;
    forceSend = 0;
  }
}
