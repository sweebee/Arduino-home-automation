// ********** CONFIG **********************************

    #define MY_NODE_ID 16       // ID of node
    #define TX_PIN 8              // Pin connected to the 433MHz transmitter
    #define ADDRESS 38484         // KaKu ID

    #define MY_DEBUG              // Debug
    #define MY_REPEATER_FEATURE   // Repeater

// ****************************************************

#define MY_RADIO_NRF24
#include <NewRemoteTransmitter.h>
#include <SPI.h>
#include <MySensor.h>

NewRemoteTransmitter transmitter(ADDRESS, TX_PIN, 260, 3);

void setup() {
  sendSketchInfo("KaKu transmitter", "1.0");
}

void loop() {  
}

void receive(const MyMessage &message) {
  // If switch
  if (message.type==V_STATUS) {
    Serial.print("Switching to ");
    Serial.println(message.getBool());
    transmitter.sendUnit(message.sensor, message.getBool());
  }
  // If dimmer
  if (message.type==V_PERCENTAGE) {
    int dimlevel = round(0.15 * atoi(message.data));
    Serial.print("Dimming to ");
    Serial.println(dimlevel);
    transmitter.sendDim(message.sensor, dimlevel); 
  }
}
