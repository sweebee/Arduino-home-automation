//********** CONFIG **********************************

    #define MY_NODE_ID AUTO      // ID of node
    #define CHILD_ID 1           // ID of sensor
    #define LIGHT_PIN A0         // Pin connected to the PIR
    
    #define MAX_LIGHT 750        // Max light value of LDR (0%)

    #define SLEEP_TIME 60000     // Sleep time between reads (seconds)
    
    #define MIN_V 2000           // empty voltage (0%)
    #define MAX_V 3200           // full voltage (100%)

    //#define MY_DEBUG              // Debug

//****************************************************

#define MY_RADIO_NRF24
#include <SPI.h>
#include <MySensor.h>  
#include <readVcc.h>

MyMessage msg(CHILD_ID, V_LIGHT_LEVEL);

int lastLightLevel = -1;
int oldBatteryPcnt = -1;
int forceCnt = 0;
boolean forceSend = false;

void setup()  
{ 
  sendSketchInfo("Light sensor","2.0");
  present(CHILD_ID, S_LIGHT_LEVEL);
}

void loop()      
{ 
  if(forceCnt < 29){
    forceCnt++;
    forceSend = false;
  } else {
    forceCnt = 0;  
    forceSend = true;    
  }
  sendLight(forceSend);
  sendBattery(forceSend);
  sleep(SLEEP_TIME);
}

// FUNCTIONS

void sendLight(boolean forceSend) // SEND LIGHT LEVEL
{
  digitalWrite(LIGHT_PIN, HIGH);
  int light = analogRead(LIGHT_PIN);
  Serial.print("Light: ");
  Serial.println(light);
  digitalWrite(LIGHT_PIN, LOW);
  int lightLevel = max(map(light, MAX_LIGHT, 0, 0, 100), 0); // Convert light to percentage
  if (lightLevel != lastLightLevel || forceSend) {
      resend(msg.set(lightLevel), 5);
      lastLightLevel = lightLevel;
  }
}

void sendBattery(boolean forceSend) // SEND BATTERYLEVEL
{
  int batteryPcnt = min(map(readVcc(), MIN_V, MAX_V, 0, 100), 100); // Get VCC and convert to percentage      
  if (batteryPcnt != oldBatteryPcnt || forceSend) { // If battery percentage has changed
    sendBatteryLevel(batteryPcnt); // Send battery percentage to gateway
    oldBatteryPcnt = batteryPcnt;
  }
}

void resend(MyMessage &msg, int repeats) // Resend messages if not received by GW
{
  int repeat = 0;
  int repeatDelay = 0;
  boolean ack = false;

  while ((ack == false) and (repeat < repeats)) {
    if (send(msg)) {
      ack = true;
    } else {
      ack = false;
      repeatDelay += 100;
    } 
    repeat++;
    delay(repeatDelay);
  }
}
