#include <MySensor.h>
#include <SPI.h>
#include <readVcc.h>

//********** CONFIG **********************************

    #define NODE_ID 1       // ID of node
    #define CHILD_ID 1      // ID of sensor
    #define PIR_PIN 3       // Pin connected to the PIR
    
    unsigned long SLEEP_TIME = 3600000; // Sleep time between reads

    // If you running a standard arduino pro mini 3.3V you have to set MIN_V to 2700
    // Its recommended to disable the BOD so you can go lower.
    // By default the clockspeed is 8 Mhz, that means you can go to MIN_V 2400
    // With a Custom bootloader you can set it to 1 Mhz and then you can set MIN_V to 1800
    // If your using a NRF24 you should set it to 2000 to be safe.

    int MIN_V = 2400; // empty voltage (0%)
    int MAX_V = 3200; // full voltage (100%)

//****************************************************

MyMessage msg(CHILD_ID, V_TRIPPED);
MySensor node;
int oldBatteryPcnt = 0;

void setup()
{
  node.begin(NULL, NODE_ID, false);
  pinMode(PIR_PIN, INPUT);
  digitalWrite(PIR_PIN, HIGH);
}

void loop()
{
  // Measure battery
  float batteryV = readVcc();
  int batteryPcnt = (((batteryV - MIN_V) / (MAX_V - MIN_V)) * 100 );
  if (batteryPcnt > 100) {
    batteryPcnt = 100;
  }
  if (batteryPcnt != oldBatteryPcnt) {
    node.sendBatteryLevel(batteryPcnt); // Send battery percentage
    oldBatteryPcnt = batteryPcnt;
  }

  // Do sensor things
  uint8_t value;
  static uint8_t sentValue = 2;

  value = digitalRead(PIR_PIN);
  if (value != sentValue) {
    // Value has changed from last transmission, send the updated value
    node.send(msg.set(value == HIGH ? 1 : 0)); // Send PIR status
    sentValue = value;
  }
  // Sleep until something happens with the sensor
  node.sleep(PIR_PIN-2, CHANGE, SLEEP_TIME);
}
