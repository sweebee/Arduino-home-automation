#include <MySensor.h> 
#include <SPI.h>

// ********** CONFIG **********************************

    #define NODE_ID AUTO   // ID of node
    #define CHILD_ID 1     // ID of sensor
    #define LED_PIN 3      // Arduino pin attached to MOSFET Gate pin
    
    #define FADE_DELAY 6  // Delay in ms for each percentage fade up/down (10ms = 1s full-range dim)
    #define REPEATER true  // Enable repeater mode

// ****************************************************

static int currentLevel = 0;  // Current dim level...
int requestedLevel = 0;
int fadeDelay = FADE_DELAY;
boolean done = true;

MySensor node;
MyMessage dimmerMsg(CHILD_ID, V_DIMMER);
MyMessage lightMsg(CHILD_ID, V_LIGHT);



void setup()  
{ 
  requestedLevel = node.loadState(0); // Load last dimlevel
  node.begin( incomingMessage, NODE_ID, REPEATER );
  node.sendSketchInfo("LED Dimmer", "1.1");
  node.present( CHILD_ID, S_DIMMER );
  node.present(CHILD_ID, S_CUSTOM);
  
  Serial.print( "Last dimlevel: " ); 
  Serial.println( requestedLevel );
  
}

void loop() 
{
  node.process();
  if(currentLevel != requestedLevel){
    if(requestedLevel > currentLevel){
      currentLevel++;
    } else {
      currentLevel--;
    }
    analogWrite( LED_PIN, (int)(currentLevel) );
    delay( fadeDelay );
    if(currentLevel == requestedLevel){
      done = true;
    }
  } else {
    if(fadeDelay != FADE_DELAY && done == true){
      fadeDelay = FADE_DELAY;
    }
  }
}


void incomingMessage(const MyMessage &message) {
  if (message.type == V_LIGHT || message.type == V_DIMMER) {
    
    //  Retrieve the power or dim level from the incoming request message
    requestedLevel = atoi( message.data );
    
    // Adjust incoming level if this is a V_LIGHT variable update [0 == off, 1 == on]
    requestedLevel *= ( message.type == V_LIGHT ? 100 : 1 );
    
    // Clip incoming level to valid range of 0 to 100
    requestedLevel = requestedLevel > 100 ? 100 : requestedLevel;
    requestedLevel = requestedLevel < 0   ? 0   : requestedLevel;
    requestedLevel = requestedLevel * 255 / 100;
    
    Serial.print( "Changing level from " );
    Serial.print( currentLevel );
    Serial.print( ", to " ); 
    Serial.println( requestedLevel );
    
    node.saveState(0, requestedLevel); // Save last dimlevel
    int fadeDelay = FADE_DELAY;
    // Inform the gateway of the current DimmableLED's SwitchPower1 and LoadLevelStatus value...
    node.send(lightMsg.set(currentLevel > 0 ? 1 : 0));
    
  }
  if(message.type == V_VAR1){
    fadeDelay = atoi( message.data );
    done = false;
  }
}
