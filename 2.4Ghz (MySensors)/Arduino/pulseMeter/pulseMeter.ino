#include <SPI.h>
#include <MySensor.h>  

//********** CONFIG **********************************

    #define NODE_ID AUTO            // ID of node
    #define CHILD_ID 1              // ID of sensor
    #define SENSOR_PIN 3            // The digital input you attached your (light) sensor.
    
    #define PULSE_FACTOR 800        // Number of blinks per KWH of your meter.
    #define MAX_WATT 10000          // Max watt value to report. This filters outliers.
    #define SEND_FREQUENCY 20000    // Minimum time between send (in milliseconds).
    
    #define REPEATER true           // Enable repeater mode

// ****************************************************

int COUNT = 0;

MySensor node;
double ppwh = ((double)PULSE_FACTOR)/1000; // Pulses per watt hour
boolean pcReceived = false;
volatile unsigned long pulseCount = 0;   
volatile unsigned long lastBlink = 0;
volatile unsigned long watt = 0;
volatile unsigned long oldPulseCount = 0;   
volatile unsigned long oldWatt = 0;
double oldKwh;
unsigned long lastSend;
MyMessage wattMsg(CHILD_ID,V_WATT);
MyMessage kwhMsg(CHILD_ID,V_KWH);
MyMessage pcMsg(CHILD_ID,V_VAR1);

void setup()  
{  
  node.begin(incomingMessage, NODE_ID, REPEATER);
  node.sendSketchInfo("kWh meter", "1.1");
  node.present(CHILD_ID, S_POWER);

  // Fetch last known pulse count value from gw
  node.request(CHILD_ID, V_VAR1);
  
  attachInterrupt(SENSOR_PIN-2, onPulse, RISING);
  lastSend=millis();
}


void loop()     
{ 
  node.process();
  unsigned long now = millis();
  // Only send values at a maximum frequency or woken up from sleep
  bool sendTime = now - lastSend > SEND_FREQUENCY;
  if (pcReceived && sendTime) {
    // New watt value has been calculated  
    if (watt != oldWatt) {
      // Check that we dont get unresonable large watt value. 
      // could hapen when long wraps or false interrupt triggered
      if (watt<((unsigned long)MAX_WATT)) {
        node.send(wattMsg.set(watt));  // Send watt value to gw 
      }  
      Serial.print("Watt:");
      Serial.println(watt);
      oldWatt = watt;
    }
  
    // Pulse count has changed
    if (pulseCount != oldPulseCount) {
      node.send(pcMsg.set(pulseCount));  // Send pulse count value to gw 
      double kwh = ((double)pulseCount/((double)PULSE_FACTOR));     
      oldPulseCount = pulseCount;
      if (kwh != oldKwh) {
        node.send(kwhMsg.set(kwh, 4));  // Send kwh value to gw 
        oldKwh = kwh;
      }
    }    
    lastSend = now;
  } else if (sendTime && !pcReceived) {
    // No count received. Try requesting it again
    node.request(CHILD_ID, V_VAR1); 
  }
}

void incomingMessage(const MyMessage &message) {
  if (message.type==V_VAR1) {  
    pulseCount = oldPulseCount = message.getLong();
    node.saveState(0, message.getLong());
    Serial.print("Received last pulse count from gw:");
    Serial.println(pulseCount);
    pcReceived = true;
  }
}

void onPulse()     
{ 
    unsigned long newBlink = micros();  
    unsigned long interval = newBlink-lastBlink;
    if (interval<10000L) { // Sometimes we get interrupt on RISING
      return;
    }
    watt = (3600000000.0 / interval) / ppwh;
    lastBlink = newBlink;
  
  Serial.println("Pulse");
  COUNT++;
  pulseCount++;
  if(COUNT == 10){
      if(node.loadState(0) > pulseCount) {
        pulseCount = node.loadState(0);
      } else {
        node.saveState(0, pulseCount);
      }
      COUNT = 0;
    }
}
