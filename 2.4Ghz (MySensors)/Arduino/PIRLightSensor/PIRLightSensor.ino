#include <MySensor.h>
#include <SPI.h>
#include <readVcc.h>

// ********** CONFIG **********************************

#define NODE_ID AUTO             // ID of node

#define PIR_ID 1              // ID of sensor
#define PIR_PIN 3             // Pin connected to the PIR

#define LIGHT_ID 2            // ID of light
#define LIGHT_PIN A2           // Pin connected to the LDR

#define MIN_V 2000            // empty voltage (0%)
#define MAX_V 3200            // full voltage (100%)

// ****************************************************

MySensor node;
MyMessage PIRMsg(PIR_ID, V_TRIPPED);
MyMessage lightMsg(LIGHT_ID, V_LIGHT_LEVEL);

int oldBatteryPcnt;
int tripped;
int sentTripped;
int lastLightLevel = -1;

void setup()
{
  node.begin(NULL, NODE_ID, false);
  
  // Register all sensors to gateway (they will be created as child devices)
  node.present(PIR_ID, S_MOTION);
  node.present(LIGHT_ID, S_LIGHT_LEVEL);
  
  pinMode(PIR_PIN, INPUT);
  digitalWrite(PIR_PIN, HIGH);
  digitalWrite(LIGHT_PIN, HIGH);
}


void loop()
{
  
  tripped = digitalRead(PIR_PIN); // Get value of PIR
  if(tripped == HIGH) {
    sendLight();   // Send lightlevel
  }
  sendPIR();     // Send PIR state
  sendBattery(); // Send batterylevel
  node.sleep(PIR_PIN - 2, CHANGE); // Sleep until something happens with the sensor
}



// FUNCTIONS

void sendBattery() // SEND BATTERYLEVEL
{
  int batteryPcnt = constrain(map(readVcc(), MIN_V, MAX_V, 0, 100), 0, 100); // Get VCC and convert to percentage
  if (batteryPcnt != oldBatteryPcnt) { // If battery percentage has changed
    node.sendBatteryLevel(batteryPcnt); // Send battery percentage to gateway
    oldBatteryPcnt = batteryPcnt;
  }
}

void sendPIR() // SEND PIR STATUS
{
  if (tripped != sentTripped) { // If status of PIR has changed
    node.send(PIRMsg.set(tripped == HIGH ? 1 : 0)); // Send PIR status to gateway
    sentTripped = tripped;
  }
}

void sendLight() // SEND LIGHTLEVEL
{
  int MAX_LIGHT = 1023;
  int MIN_LIGHT = 400;
  float light = analogRead(LIGHT_PIN); // Get value of the LDR
  
  int lightLevel = constrain(map(light, MAX_LIGHT, MIN_LIGHT, 0, 100), 0, 100); // Convert light to percentage
  Serial.println("Lightlevel: ");
  Serial.print(lightlevel);
   if (lightLevel != lastLightLevel) { // If value of LDR has changed
      node.send(lightMsg.set(lightLevel)); // Send value of LDR to gateway
      lastLightLevel = lightLevel;
   }
}
