#include <MySensor.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>

#define PIN 4
#define NODE_ID 5

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);
 
int Rold = 0;
int Gold = 0;
int Bold = 0;

int RL = 0;
int GL = 0;
int BL = 0;

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
   
   colorDirect(255, 0, 0); // Red
   delay(200);
   colorDirect(0, 255, 0); // Green
   delay(200);
   colorDirect(0, 0, 255); // Blue
   delay(200);
   colorFade(0, 0, 0, 5); // Off
}
 
void loop() {
    gw.process();
}

void incomingMessage(const MyMessage &message) {
  if (message.type==V_LIGHT) {
      
      int ID = message.sensor;
      int VAL = message.getBool();
      
      
     if(ID == 1 && VAL == 0) {
        colorFade(0, 0, 0, 5); // Off
        RL = 0;
        GL = 0;
        BL = 0;
     }
     
     if(ID == 1 && VAL == 1) {
        colorFade(255, 0, 0, 5); // Red
        RL = 255;
        GL = 0;
        BL = 0;
     }
     
     if(ID == 2 && VAL == 0) {
        colorFade(0, 255, 30, 5); // Green
        RL = 0;
        GL = 255;
        BL = 30;
     }
   
    if(ID == 2 && VAL == 1) {
        colorFade(0, 150, 255, 5); // Blue
        RL = 0;
        GL = 150;
        BL = 255;
     }
     if(ID == 3 && VAL == 0) {
        colorFade(255, 147, 41, 5); // Warm white
        RL = 255;
        GL = 141;
        BL = 41;
     }
     
     
     // NOTIFICATION
     if(ID == 3 && VAL == 1) {
       
        colorFade(0, 0, 0, 2); // Off
        colorFade(255, 0, 0, 2); // Red
        colorFade(0, 0, 0, 2); // Off
        colorFade(255, 0, 0, 2); // Red
        colorFade(0, 0, 0, 2); // Off
        colorFade(RL, GL, BL, 2); // Last state

     }
     
      if(ID == 4 && VAL == 0) {
        colorFade(255, 0, 115, 5); // Purple
        RL = 255;
        GL = 0;
        BL = 255;
     }
     
     if(ID == 4 && VAL == 1) {
        colorFade(255, 197, 143, 5); // White
        RL = 255;
        GL = 197;
        BL = 143;
     }
   } 
}

// FADE COLOR TO COLOR
void colorFade(uint32_t R, uint32_t G, uint32_t B, uint32_t wait) {
      strip.setBrightness(255);
      int T = 255;
      while(T != 0) {
        if(R > Rold) {Rold++;}
        if(R < Rold) {Rold--;}
        if(G > Gold) {Gold++;}
        if(G < Gold) {Gold--;}
        if(B > Bold) {Bold++;}
        if(B < Bold) {Bold--;}
        T--;
        for(int i=0;i<16;i++){
          strip.setPixelColor(i, strip.Color(Rold,Gold,Bold));
        }
        strip.show();
       delay(wait);
      } 
 }

 // SWITCH DIRECT TO COLOR
  void colorDirect(uint32_t R, uint32_t G, uint32_t B) {
    strip.setBrightness(255);
    Rold = R;
    Gold = G;
    Bold = B;
   for(int i=0;i<16;i++){
          strip.setPixelColor(i, strip.Color(R,G,B));
        }
        strip.show();
 }
