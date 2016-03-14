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



int currentLevel = 0; // Dimlevel
long RGB_values[3] = {0, 0, 0}; // Colors

MySensor gw;
MyMessage dimmerMsg(0, V_DIMMER);

void setup() {
  strip.begin();
  strip.show();

  gw.begin(incomingMessage, NODE_ID, false);
  gw.sendSketchInfo("Mood Light", "3.0");
  gw.present(0, S_CUSTOM);
  gw.present( 0, S_DIMMER );
  // colorChange(RED, GREEN, BUE, FADE)
  colorChange(255, 0, 255, true); // Purple
  colorChange(0, 0, 0, true);     // Off
}

void loop() {
  gw.process();
}

void incomingMessage(const MyMessage &message) {

  if (message.type == V_VAR1) {
    String hexstring = message.getString();

    // Check if contains hex character

      // Remove the character
      hexstring.remove(0, 1);
    
    Serial.println(hexstring);
    long number = (long) strtol( &hexstring[0], NULL, 16);
    RGB_values[0] = number >> 16;
    RGB_values[1] = number >> 8 & 0xFF;
    RGB_values[2] = number & 0xFF;
    
    R = RGB_values[0];
    G = RGB_values[1];
    B = RGB_values[2];
    colorChange(R, G, B, true);

    // Write some debug info
    Serial.print("Red is " );
    Serial.println(RGB_values[0]);
    Serial.print("Green is " );
    Serial.println(RGB_values[1]);
    Serial.print("Blue is " );
    Serial.println(RGB_values[2]);
  }

  if (message.type == V_DIMMER) {
    // Get the dimlevel
    int reqLevel = atoi( message.data );
    // Clip incoming level to valid range of 0 to 100
    reqLevel = reqLevel > 100 ? 100 : reqLevel;
    reqLevel = reqLevel < 0   ? 0   : reqLevel;

    // Set brightness
    Serial.print("Dimming to ");
    Serial.println(reqLevel);
    fadeToLevel(reqLevel);
    
  }

}

// CHANGE TO COLOR
void colorChange(uint32_t R, uint32_t G, uint32_t B, uint32_t fade) {
  if (!fade) {
    Rold = R;
    Bold = B;
    Gold = G;
    for (int i = 0; i < 16; i++) {
      strip.setPixelColor(i, strip.Color(Rold, Gold, Bold));
    }
    strip.show();
  } else {
    for (int t = 0; t < 256; t++) {
      if (R > Rold) {
        Rold++;
      }
      if (R < Rold) {
        Rold--;
      }
      if (G > Gold) {
        Gold++;
      }
      if (G < Gold) {
        Gold--;
      }
      if (B > Bold) {
        Bold++;
      }
      if (B < Bold) {
        Bold--;
      }
      for (int i = 0; i < 16; i++) {
        strip.setPixelColor(i, strip.Color(Rold, Gold, Bold));
      }
      strip.show();
      delay(2);
    }
  }
}

// DIMMER FADE
void fadeToLevel( int toLevel ) {

  int delta = ( toLevel - currentLevel ) < 0 ? -1 : 1;

  while ( currentLevel != toLevel ) {
    currentLevel += delta;
    strip.setBrightness(currentLevel);
    if(currentLevel == 1){
      for (int i = 0; i < 16; i++) {
        strip.setPixelColor(i, strip.Color(Rold, Gold, Bold));
      }
    }
    strip.show();
    delay(15);
  }
}
