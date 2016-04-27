// ********** CONFIG **********************************

    #define MY_NODE_ID 99         // ID of node
    #define KWH_ID 1              // ID of pulse meter
    #define PIR_ID 2              // ID of the PIR
    
    #define KWH_PIN 3             // Pin connected to the pulse meter
    #define PIR_PIN 5             // Pin connected to the PIR

    #define PULSE_FACTOR 800      // Nummber of blinks per KWH of your meeter
    #define MAX_WATT 15000        // Max watt value to report. This filetrs outliers.
    #define SLEEP_MODE false      // Watt-value can only be reported when sleep mode is false.

    //#define MY_DEBUG              // Debug

// ****************************************************

#define MY_RADIO_NRF24
#include <SPI.h>
#include <MySensor.h>  

#define INTERRUPT KWH_PIN-2 // Usually the interrupt = pin -2 (on uno/nano anyway)


unsigned long SEND_FREQUENCY = 20000; // Minimum time between send (in milliseconds). We don't wnat to spam the gateway.
double ppwh = ((double)PULSE_FACTOR)/1000; // Pulses per watt hour
boolean pcReceived = false;
boolean sentDoorbell;
volatile unsigned long pulseCount = 0;   
volatile unsigned long lastBlink = 0;
volatile unsigned long watt = 0;
unsigned long oldPulseCount = 0;   
unsigned long oldWatt = 0;
double oldKwh;
unsigned long lastSend;
MyMessage wattMsg(KWH_ID,V_WATT);
MyMessage kwhMsg(KWH_ID,V_KWH);
MyMessage pcMsg(KWH_ID,V_VAR1);
MyMessage msg(PIR_ID,V_TRIPPED);


void setup()  
{  
  // Fetch last known pulse count value from gw
  request(KWH_ID, V_VAR1);
  pinMode(PIR_PIN, INPUT);
  digitalWrite(PIR_PIN,HIGH);

  // Use the internal pullup to be able to hook up this sketch directly to an energy meter with S0 output
  // If no pullup is used, the reported usage will be too high because of the floating pin
  
  attachInterrupt(INTERRUPT, onPulse, RISING);
  lastSend=millis();
}

void presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Energy Meter", "1.0");

  // Register this device as power sensor
  present(KWH_ID, S_POWER);
  present(PIR_ID, S_MOTION);
}

void loop()     
{ 
  boolean doorbell = digitalRead(PIR_PIN);
  if(doorbell != sentDoorbell){
    if(doorbell){
      resend(msg.set(1),5);
    }
    sentDoorbell = doorbell;
  }
  unsigned long now = millis();
  // Only send values at a maximum frequency or woken up from sleep
  bool sendTime = now - lastSend > SEND_FREQUENCY;
  if (pcReceived && (SLEEP_MODE || sendTime)) {
    // New watt value has been calculated  
    if (!SLEEP_MODE && watt != oldWatt) {
      // Check that we dont get unresonable large watt value. 
      // could hapen when long wraps or false interrupt triggered
      if (watt<((unsigned long)MAX_WATT)) {
        resend(wattMsg.set(watt),5);  // Send watt value to gw 
      }  
      Serial.print("Watt:");
      Serial.println(watt);
      oldWatt = watt;
    }
  
    // Pulse cout has changed
    if (pulseCount != oldPulseCount) {
      resend(pcMsg.set(pulseCount),5);  // Send pulse count value to gw 
      double kwh = ((double)pulseCount/((double)PULSE_FACTOR));     
      oldPulseCount = pulseCount;
      if (kwh != oldKwh) {
        send(kwhMsg.set(kwh, 4));  // Send kwh value to gw 
        oldKwh = kwh;
      }
    }    
    lastSend = now;
  } else if (sendTime && !pcReceived) {
    // No count received. Try requesting it again
    request(KWH_ID, V_VAR1);
    lastSend=now;
  }
  
  if (SLEEP_MODE) {
    sleep(SEND_FREQUENCY);
  }
}

void receive(const MyMessage &message) {
  if (message.type==V_VAR1) {  
    pulseCount = oldPulseCount = message.getLong();
    Serial.print("Received last pulse count from gw:");
    Serial.println(pulseCount);
    pcReceived = true;
  }
}

void onPulse()     
{ 
  if (!SLEEP_MODE) {
    Serial.println("Pulse!");
    unsigned long newBlink = micros();  
    unsigned long interval = newBlink-lastBlink;
    if (interval<10000L) { // Sometimes we get interrupt on RISING
      return;
    }
    watt = (3600000000.0 /interval) / ppwh;
    lastBlink = newBlink;
  } 
  pulseCount++;
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

