// KlikAanKlikUit PIR sensor
// Based on AttinyX5
// Author: Wiebe Nieuwenhuis
//
//                    +-\/-+
//             RESET 1|o   |8 Vcc 2,7 - 5,5V
//               (3) 2|    |7 (2)
//    PIR SENSOR (4) 3|    |6 (1)
//               GND 4|    |5 (0) 433Mhz Transmitter
//                    +----+

#include <avr/sleep.h>
#include <avr/power.h>
#include <NewRemoteTransmitter.h>

#define ID 1234         // KAKU address

#define txPin 0  // 433Mhz TX pin
#define inPin 4  // PIR sensor pin

boolean input = false;
boolean state  = false;

NewRemoteTransmitter transmitter(ID, txPin, 260, 3); // Set-up transmitter

ISR (PCINT0_vect) {
  
}

void setup () { 
  
  pinMode (txPin, OUTPUT);
  pinMode (inPin, INPUT);
  digitalWrite (inPin, HIGH);  // internal pull-up

  // pin change interrupt
  PCMSK  |= bit (PCINT4);  // want pin D4 / pin 3
  GIFR   |= bit (PCIF);    // clear any outstanding interrupts
  GIMSK  |= bit (PCIE);    // enable pin change interrupts
  
}

void loop () {
  
  input = digitalRead(inPin);   // read the input pin
  
  if(input == true && state == false) {
       transmitter.sendUnit(1, true);
       state = true;      
  }

  if(input == false && state == true) {
       transmitter.sendUnit(1, false);
       state = false;
  }
  
  delay (10);
  goToSleep ();
  
}

// SLEEP FUNCTION
void goToSleep () {  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  ADCSRA = 0;            // turn off ADC
  power_all_disable ();  // power off ADC, Timer 0 and 1, serial interface
  sleep_enable();
  sleep_cpu();
  sleep_disable();
  power_all_enable();    // power everything back on
}
