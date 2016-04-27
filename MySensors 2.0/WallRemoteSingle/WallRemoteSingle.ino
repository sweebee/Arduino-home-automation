// ********** CONFIG **********************************

    #define MY_NODE_ID 16         // ID of node
    #define CHILD_ID 1            // ID of sensor
    #define SWITCH_PIN 3             // Pin connected to the PIR
    
    #define MIN_V 2000            // empty voltage (0%)
    #define MAX_V 3200            // full voltage (100%)

    #define SLEEP_TIME 3600000    // Sleep 1h

    #define MY_DEBUG              // Debug

// ****************************************************

#define MY_RADIO_NRF24
#include <MySensor.h>
#include <SPI.h>
#include <readVcc.h>

MyMessage msg(CHILD_ID, V_TRIPPED);
int oldBatteryPcnt = -1;
boolean oldValue;
int forceSend = 0;

void setup()
{
  sendSketchInfo("Wall switch", "2.0");
  present(CHILD_ID, S_MOTION);
  pinMode(SWITCH_PIN, INPUT);
}

void loop()
{
  
  // Switch changed
  boolean value = digitalRead(SWITCH_PIN);
  if(value != oldValue){
    resend(msg.set(1),6);
    oldValue = value;
  }
  // Send batterylevel
  sendBattery(); 

  // Sleep until something happens with the switch
  sleep(SWITCH_PIN-2, CHANGE, SLEEP_TIME); 
}

// FUNCTIONS

void sendBattery() // Send battery percentage to GW
{
  forceSend++;
  int batteryPcnt = min(map(readVcc(), MIN_V, MAX_V, 0, 100), 100); // Get VCC and convert to percentage      
  if (batteryPcnt != oldBatteryPcnt || forceSend >= 23) { // If battery percentage has changed
    sendBatteryLevel(batteryPcnt); // Send battery percentage to gateway
    oldBatteryPcnt = batteryPcnt; 
    forceSend = 0;
  }
}

void resend(MyMessage &msg, int repeats) // Resend messages if not received by GW
{
  int repeat = 1;
  int repeatDelay = 0;

  while ((!send(msg)) and (repeat < repeats)) {
      repeatDelay += 100;
      repeat++;
      delay(repeatDelay);
    }    
}
