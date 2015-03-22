// 2 digital input sensor with sleep/interrupt
// This sketch reads 2 digital sensors and sends true or false over 433mhz (seperate)
// Based on AttinyX5
// Author: Wiebe Nieuwenhuis
//
//                    +-\/-+
//             RESET 1|o   |8 Vcc 2,7 - 5,5V
//      SENSOR 1 (3) 2|    |7 (2)
//      SENSOR 2 (4) 3|    |6 (1)
//               GND 4|    |5 (0) 433Mhz Transmitter
//                    +----+

#include <avr/sleep.h>
#include <avr/power.h>
#include <NewRemoteTransmitter.h>

int ID = 12345;            // KAKU address
const byte txPin = 0;  // 433Mhz Transmitter pin
const byte sensor1Pin = 3;  // Sensor 1 pin
const byte sensor2Pin = 4;  // Sensor 2 pin

boolean sensor1Value = false;
boolean sensor2Value = false;
boolean state  = false;
boolean state2  = false;

NewRemoteTransmitter transmitter(ID, txPin, 260, 3); // Set-up transmitter

ISR (PCINT0_vect)
{
  
}

void setup() {
  pinMode (txPin, OUTPUT);
  pinMode (sensor1Pin, INPUT);
  digitalWrite (sensor1Pin, HIGH);  // internal pull-up
  pinMode (sensor2Pin, INPUT);
  digitalWrite (sensor2Pin, HIGH);  // internal pull-up
  
  // pin change interrupt (example for D4)
  PCMSK  |= bit (PCINT3);  // want pin D4 / pin 3
  PCMSK  |= bit (PCINT4);  // want pin D4 / pin 3
  GIFR   |= bit (PCIF);    // clear any outstanding interrupts
  GIMSK  |= bit (PCIE);    // enable pin change interrupts 

}

void loop() {
  
  sensor2Value = digitalRead(sensor1Pin);  // Read value of sensor 1
  sensor1Value = digitalRead(sensor2Pin); // Read value of sensor 2  
  
 
  if(sensor1Value == true && state == false) {
       transmitter.sendUnit(1, true); // If door is open send TRUE signal over 433Mhz
       state = true;   
         
  }

  if(sensor1Value == false && state == true) {
       transmitter.sendUnit(1, false); // If door is closed send FALSE signal over 433Mhz
       state = false;
       
  }
 
  if(sensor2Value == true && state2 == false) {
       transmitter.sendUnit(2, true); // If its raining send TRUE signal over 433Mhz     
       state2 = true;
         
  }

  if(sensor2Value == false && state2 == true) {
       transmitter.sendUnit(2, false); // If its not raining send FALSE signal over 433Mhz
       state2 = false;
        
  }
  
  delay(100); // Wait
  
  goToSleep ();
}

void goToSleep () {
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  ADCSRA = 0;            // turn off ADC
  power_all_disable ();  // power off ADC, Timer 0 and 1, serial interface
  sleep_enable();
  sleep_cpu();
  sleep_disable();
  power_all_enable();    // power everything back on

}
