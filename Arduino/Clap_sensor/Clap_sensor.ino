// Clap sensor to 433Mhz
// Control your lights with your hands
// Author: Wiebe Nieuwenhuis
// Based on: http://en.code-bude.net/2014/12/08/how-to-build-a-clap-switch-using-arduino/

#include <NewRemoteTransmitter.h>

int txPin = 4;        // 433Mhz TX pin
int ID = 123456;      // KAKU address
int soundPin = 3;     // Mic sensor
int LED = 5;          // Test LED

int claps = 0;
long detectionSpanInitial = 0;
long detectionSpan = 0;
boolean lightState = false;

NewRemoteTransmitter transmitter(ID, txPin, 260, 3); // Set-up transmitter

void setup() {
  pinMode(soundPin, INPUT);
}

void loop() {

  int sensorState = digitalRead(soundPin);

  if (sensorState == 0) {
    if (claps == 0) {
      detectionSpanInitial = detectionSpan = millis();
      claps++;
    }
    else if (claps > 0 && millis()-detectionSpan >= 50) {
      detectionSpan = millis();
      claps++;
    }
  }

  if (millis()-detectionSpanInitial >= 400) {
    if (claps == 2) {
      if (!lightState) {
          lightState = true;
          digitalWrite(LED, HIGH);
        }
        else if (lightState) {
          lightState = false;
          digitalWrite(LED, LOW);
        }
    }
    claps = 0;
  }
}
