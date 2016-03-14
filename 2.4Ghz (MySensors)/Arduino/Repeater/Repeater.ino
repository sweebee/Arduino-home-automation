#include <MySensor.h>
#include <SPI.h>

//********** CONFIG **********************************

    #define NODE_ID AUTO

// ****************************************************

MySensor gw;

void setup()  
{  
  // The third argument enables repeater mode.
  gw.begin(NULL, NODE_ID, true);

  //Send the sensor node sketch version information to the gateway
  gw.sendSketchInfo("Repeater", "1.0");
}

void loop() 
{
  // By calling process() you route messages in the background
  gw.process();
}

