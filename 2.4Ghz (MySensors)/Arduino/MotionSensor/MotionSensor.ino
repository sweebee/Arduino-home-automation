// ********** CONFIG **********************************

    //#define MY_NODE_ID 99         // ID of node
    #define CHILD_ID 1            // ID of sensor
    #define PIR_PIN 3             // Pin connected to the PIR
    
    #define MIN_V 2000            // empty voltage (0%)
    #define MAX_V 3200            // full voltage (100%)

    #define MY_DEBUG              // Debug

// ****************************************************

#define MY_RADIO_NRF24
#include <MySensor.h>
#include <SPI.h>
#include <readVcc.h>

MyMessage msg(CHILD_ID, V_TRIPPED);
int oldBatteryPcnt = -1;
int forceSend = 0;

void setup()
{
  sendSketchInfo("PIR Sensor", "2.0");
  present(CHILD_ID, S_MOTION);
  pinMode(PIR_PIN, INPUT);
}

void loop()
{
  
  // Get PIR status and send to the gateway
  resend(msg.set(digitalRead(PIR_PIN)),6);
  
  // Send batterylevel
  sendBattery(); 

  // Sleep until something happens with the PIR
  sleep(PIR_PIN-2, CHANGE); 
}

// FUNCTIONS

void sendBattery() // Send battery percentage to GW
{
  forceSend++;
  int batteryPcnt = min(map(readVcc(), MIN_V, MAX_V, 0, 100), 100); // Get VCC and convert to percentage      
  if (batteryPcnt != oldBatteryPcnt || forceSend >= 20) { // If battery percentage has changed
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
