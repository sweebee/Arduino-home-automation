#include <MySensor.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 4
#define NODE_ID 5

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, LED_PIN, NEO_GRB + NEO_KHZ800);

int R;
int G;
int B;

int Rold;
int Gold;
int Bold;

MySensor gw;

void setup() {
  strip.begin();
  strip.show();

  gw.begin(incomingMessage, NODE_ID, true);
  gw.sendSketchInfo("Mood Light", "1.1");
  gw.present(1, S_LIGHT);
  gw.present(2, S_LIGHT);
  gw.present(3, S_LIGHT);
  gw.present(4, S_LIGHT);

  // colorChange(RED, GREEN, BUE, FADE)
  colorChange(255, 0, 255, true); // Purple
  colorChange(0, 0, 0, true); // Off
}

void loop() {
  gw.process();
}

void incomingMessage(const MyMessage &message) {
  if (message.type == V_LIGHT) {

    int ID = message.sensor;
    int VAL = message.getBool();

    if (ID == 1 && VAL == 0) { // Off
      R = 0;
      G = 0;
      B = 0;
      colorChange(R, G, B, true);
    }

    if (ID == 1 && VAL == 1) { // Red
      R = 255;
      G = 0;
      B = 0;
      colorChange(R, G, B, true);
    }

    if (ID == 2 && VAL == 0) { // Green
      R = 0;
      G = 255;
      B = 30;
      colorChange(R, G, B, true);
    }

    if (ID == 2 && VAL == 1) { // Bue
      R = 0;
      G = 150;
      B = 255;
      colorChange(R, G, B, true);
    }

    if (ID == 3 && VAL == 0) { // Warm white
      R = 255;
      G = 141;
      B = 41;
      colorChange(R, G, B, true);
    }


    // NOTIFICATION
    if (ID == 3 && VAL == 1) {
      colorChange(0, 0, 0, true); // Off
      colorChange(255, 0, 0, true); // Red
      colorChange(0, 0, 0, true); // Off
      colorChange(255, 0, 0, true); // Red
      colorChange(0, 0, 0, true); // Off
      colorChange(R, G, B, true); // Last state
    }

    if (ID == 4 && VAL == 0) { // Purple
      R = 255;
      G = 0;
      B = 255;
      colorChange(R, G, B, 255, true);
    }

    if (ID == 4 && VAL == 1) { // White
      R = 255;
      G = 197;
      B = 143;
      colorChange(R, G, B, 255, true);
    }
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
      delay(5);
    }
  }
}
