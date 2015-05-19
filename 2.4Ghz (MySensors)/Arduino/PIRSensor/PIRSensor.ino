#include <MySensor.h>
#include <SPI.h>

// ********** CONFIG **********************************

    #define NODE_ID 2             // ID of node
    #define CHILD_ID 1            // ID of sensor
    #define PIR_PIN 3             // Pin connected to the PIR
    
    #define MIN_V 1900            // empty voltage (0%)
    #define MAX_V 3200            // full voltage (100%)

// ****************************************************

MyMessage msg(CHILD_ID, V_TRIPPED);
MySensor node;

int oldBatteryPcnt;
int sentValue;

void setup()
{
  node.begin(NULL, NODE_ID, false);
  node.present(CHILD_ID, S_MOTION);
  pinMode(PIR_PIN, INPUT);
  digitalWrite(PIR_PIN, HIGH);
}


void loop()
{
  sendBattery(); // Send batterylevel 
  sendPIR();     // Send PIR value 
  node.sleep(PIR_PIN-2, CHANGE); // Sleep until something happens with the sensor
}



// FUNCTIONS

void sendBattery() // SEND BATTERYLEVEL
{
  int batteryPcnt = min(map(readVcc(), MIN_V, MAX_V, 0, 100), 100); // Get VCC and convert to percentage      
  if (batteryPcnt != oldBatteryPcnt) { // If battery percentage has changed
    node.sendBatteryLevel(batteryPcnt); // Send battery percentage to gateway
    oldBatteryPcnt = batteryPcnt;
  }
}

void sendPIR() // SEND PIR STATUS
{
  int value = digitalRead(PIR_PIN); // Get value of PIR
  if (value != sentValue) { // If status of PIR has changed
    node.send(msg.set(value == HIGH ? 1 : 0)); // Send PIR status to gateway
    sentValue = value;
  }
}

long readVcc() { // READ VCC VOLTAGE
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
 
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
 
  long result = (high<<8) | low;
 
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}
