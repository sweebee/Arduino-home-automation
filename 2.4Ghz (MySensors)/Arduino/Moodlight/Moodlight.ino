#include <MySensor.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 4
#define NODE_ID 50

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, LED_PIN, NEO_GRB + NEO_KHZ800);

int R;
int G;
int B;

int Rold;
int Gold;
int Bold;

int oldRequestedLevel;
long RGB_values[3] = {0,0,0};

MySensor gw;
MyMessage stateRedMsg(1, V_LIGHT);
MyMessage redMsg(1, V_DIMMER);
MyMessage stateGreenMsg(1, V_LIGHT);
MyMessage greenMsg(1, V_DIMMER);
MyMessage stateBlueMsg(1, V_LIGHT);
MyMessage blueMsg(1, V_DIMMER);


void setup() {
  strip.begin();
  strip.show();

  gw.begin(incomingMessage, NODE_ID, false);
  gw.sendSketchInfo("Mood Light", "2.0");
  gw.present(0, S_CUSTOM);
  gw.present(1, S_DIMMER );
  gw.present(2, S_DIMMER );
  gw.present(3, S_DIMMER );
  // colorChange(RED, GREEN, BUE, FADE)
  colorChange(255, 0, 255, true); // Purple
  colorChange(0, 0, 0, true); // Off
}

void loop() {
  gw.process();
}

void incomingMessage(const MyMessage &message) {
  
  if (message.type == V_LIGHT || message.type == V_DIMMER) {
    
    int ID = message.sensor;
    int requestedLevel = atoi( message.data );
    requestedLevel *= ( message.type == V_LIGHT ? 100 : 1 );
    
    // Clip incoming level to valid range of 0 to 100
    requestedLevel = requestedLevel > 100 ? 100 : requestedLevel;
    requestedLevel = requestedLevel < 0   ? 0   : requestedLevel;
    requestedLevel = requestedLevel * 2.55;
    
    Serial.print("Sensor: ");
    Serial.println(ID);
    
    if ( ID == 1) { // Dimmer rood
      R = requestedLevel;
      G = Gold;
      B = Bold;
      colorChange(R, G, B, false);
      oldRequestedLevel = requestedLevel;
    }
    
    if ( ID == 2) { // Dimmer groen
      R = Rold;
      G = requestedLevel;
      B = Bold;
      colorChange(R, G, B, false);
      oldRequestedLevel = requestedLevel;
    }
    
    if ( ID == 3) { // Dimmer blauw
      R = Rold;
      G = Gold;
      B = requestedLevel;
      colorChange(R, G, B, false);
      oldRequestedLevel = requestedLevel;
    }
    Serial.print("RED: ");
    Serial.println(R);
    Serial.print("GREEN: ");
    Serial.println(G);
    Serial.print("BLUE: ");
    Serial.println(B);
  }
  
  if (message.type==V_VAR1) {
   String hexstring = message.getString();
   long number = (long) strtol( &hexstring[0], NULL, 16);
   RGB_values[0] = number >> 16;
   RGB_values[1] = number >> 8 & 0xFF;
   RGB_values[2] = number & 0xFF;
   Serial.println(hexstring);  
   R = RGB_values[0];
   G = RGB_values[1];
   B = RGB_values[3];
   colorChange(R, G, B, true);
   
   // Write some debug info
   Serial.print("Red is " );
   Serial.println(RGB_values[0]);
   Serial.print("Green is " );
   Serial.println(RGB_values[1]);
   Serial.print("Blue is " );
   Serial.println(RGB_values[2]);
   
  }

}

// CHANGE TO COLOR
void colorChange(uint32_t R, uint32_t G, uint32_t B, uint32_t fade) {
  if(fade == false) {
    Rold = R;
    Bold = B;
    Gold = G;
  for (int i = 0; i < 16; i++) {
        strip.setPixelColor(i, strip.Color(Rold, Gold, Bold));
      }
      strip.show();
  } else {
    for(int t = 0; t < 256; t++) {
      if (R > Rold) { Rold++; }
      if (R < Rold) { Rold--; }
      if (G > Gold) { Gold++; }
      if (G < Gold) { Gold--; }      
      if (B > Bold) { Bold++; }
      if (B < Bold) { Bold--; }  
      for (int i = 0; i < 16; i++) {
        strip.setPixelColor(i, strip.Color(Rold, Gold, Bold));
      }
      strip.show();
      delay(1);
    }
  }
}
