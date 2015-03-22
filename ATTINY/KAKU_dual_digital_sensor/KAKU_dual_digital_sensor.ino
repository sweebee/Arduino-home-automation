
//                    +-\/-+
//             RESET 1|o   |8 Vcc 2,7 - 5,5V
//   RAIN SENSOR (3) 2|    |7 (2)
//   DOOR SENSOR (4) 3|    |6 (1)
//               GND 4|    |5 (0) 433Mhz Transmitter
//                    +----+

#include <avr/sleep.h>
#include <avr/power.h>
#include <NewRemoteTransmitter.h>

int ID = 123;            // KAKU address
const byte txPin   = 0;  // 433Mhz Transmitter pin
const byte rainPin = 3;  // Rain meter pin
const byte doorPin = 4;  // Door sensor pin

boolean rainValue = false;
boolean doorValue = false;
boolean state  = false;
boolean state2  = false;

NewRemoteTransmitter transmitter(ID, txPin, 260, 3); // Set-up transmitter

ISR (PCINT0_vect)
{
  // do something interesting here
}

void setup() {
  pinMode (txPin, OUTPUT);
  pinMode (rainPin, INPUT);
  digitalWrite (rainPin, HIGH);  // internal pull-up
  pinMode (doorPin, INPUT);
  digitalWrite (doorPin, HIGH);  // internal pull-up
  
  // pin change interrupt (example for D4)
  PCMSK  |= bit (PCINT3);  // want pin D4 / pin 3
  PCMSK  |= bit (PCINT4);  // want pin D4 / pin 3
  GIFR   |= bit (PCIF);    // clear any outstanding interrupts
  GIMSK  |= bit (PCIE);    // enable pin change interrupts 

}

void loop() {
  
  doorValue = digitalRead(doorPin); // Read value of door sensor
  rainValue = digitalRead(rainPin);  // Read value of rain sensor
  
 
  if(doorValue == true && state == false) {
       transmitter.sendUnit(1, true); // If door is open send TRUE signal to pimatic
       state = true;   
         
  }

  if(doorValue == false && state == true) {
       transmitter.sendUnit(1, false); // If door is closed send FALSE signal to pimatic
       state = false;
       
  }
 
  if(rainValue == true && state2 == false) {
       transmitter.sendUnit(2, true); // If rainsensor has value highter than 300 send TRUE signal to pimatic     
       state2 = true;
         
  }

  if(rainValue == false && state2 == true) {
       transmitter.sendUnit(2, false); // IF rainsensor has value lower than 300 send FALSE signal to pimatic
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
