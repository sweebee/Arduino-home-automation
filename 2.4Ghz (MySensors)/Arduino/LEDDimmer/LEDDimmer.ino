// ********** CONFIG **********************************

    //  #define MY_NODE_ID AUTO   // ID of node
    #define CHILD_ID 1        // ID of sensor
    #define LED_PIN 3         // Arduino pin attached to MOSFET Gate pin
    
    #define FADE_DELAY 6      // Delay in ms for each percentage fade up/down (10ms = 1s full-range dim)
    
    #define MY_REPEATER_FEATURE // Repeater
    //#define MY_DEBUG            // Debug messages

// ****************************************************

#define MY_RADIO_NRF24
#include <MySensor.h> 
#include <SPI.h>

static int currentLevel = 100;  // Current dim level...
int requestedLevel;
int fadeDelay = FADE_DELAY;
boolean done = true;
MyMessage dimmerMsg(CHILD_ID, V_PERCENTAGE);
MyMessage lightMsg(CHILD_ID, V_LIGHT);



void setup() {
   
  sendSketchInfo("LED Dimmer", "2.0");
  present(CHILD_ID, S_DIMMER);
  present(CHILD_ID, S_CUSTOM);
  
}

void loop() {
}

void receive(const MyMessage &message) {
  if ((message.type == V_LIGHT || message.type == V_PERCENTAGE) && done) {
    
    //  Retrieve the power or dim level from the incoming request message
    requestedLevel = atoi(message.data);
    
    // Adjust incoming level if this is a V_LIGHT variable update [0 == off, 1 == on]
    requestedLevel *= (message.type == V_LIGHT ? 100 : 1);
    
    // Clip incoming level to valid range of 0 to 100
    requestedLevel = requestedLevel > 100 ? 100 : requestedLevel;
    requestedLevel = requestedLevel < 0   ? 0   : requestedLevel;
    requestedLevel = requestedLevel * 255 / 100;
    
    Serial.print("Changing level from ");
    Serial.print(currentLevel );
    Serial.print(", to "); 
    Serial.println(requestedLevel);
    fadeToLevel(requestedLevel, fadeDelay);
    int fadeDelay = FADE_DELAY;
    // Inform the gateway of the current DimmableLED's SwitchPower1 and LoadLevelStatus value...
    send(lightMsg.set(currentLevel > 0 ? 1 : 0));
    
  }
  if(message.type == V_VAR1){
    fadeDelay = atoi(message.data);
  }
}

void fadeToLevel(int toLevel, int fadeTime) {
  done = false;
  int delta = (toLevel - currentLevel) < 0 ? -1 : 1;
  
  while (currentLevel != toLevel) {
    currentLevel += delta;
    analogWrite(LED_PIN, (int)(currentLevel));
    wait(fadeTime);
  }
  done = true;
}
