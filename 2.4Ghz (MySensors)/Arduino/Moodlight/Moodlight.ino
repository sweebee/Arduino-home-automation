#include <MySensor.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#define PIN 4
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
   strip.show(); // Initialize all pixels to 'off'
  
   gw.begin(incomingMessage, 5, true);
   gw.sendSketchInfo("Mood Light", "1.1");
   gw.present(1, S_LIGHT);
   gw.present(2, S_LIGHT);
   gw.present(3, S_LIGHT);
   gw.present(4, S_LIGHT);
   
   colorDirect(255, 0, 0); // Rood
   delay(200);
   colorDirect(0, 255, 0); // Groen
   delay(200);
   colorDirect(0, 0, 255); // Blauw
   delay(200);
   colorFade(0, 0, 0, 5); // Uit
}
 
void loop() {
    gw.process();
}

void incomingMessage(const MyMessage &message) {
  if (message.type==V_LIGHT) {
      
      int ID = message.sensor;
      int VAL = message.getBool();
      
      
     
      
     if(ID == 1 && VAL == 0) {
        colorFade(0, 0, 0, 5); // Uit
        RL = 0;
        GL = 0;
        BL = 0;
     }
     
     if(ID == 1 && VAL == 1) {
        colorFade(255, 0, 0, 5); // Rood
        RL = 255;
        GL = 0;
        BL = 0;
     }
     
     if(ID == 2 && VAL == 0) {
        colorFade(0, 255, 30, 5); // Groen
        RL = 0;
        GL = 255;
        BL = 30;
     }
   
    if(ID == 2 && VAL == 1) {
        colorFade(0, 150, 255, 5); // Blauw
        RL = 0;
        GL = 150;
        BL = 255;
     }
     if(ID == 3 && VAL == 0) {
        colorFade(255, 147, 41, 5); // Warm wit
        RL = 255;
        GL = 141;
        BL = 41;
     }
     
     
     // MELDING
     if(ID == 3 && VAL == 1) {
       
        colorFade(0, 0, 0, 2); // Uit
        colorFade(255, 0, 0, 2); // Rood
        colorFade(0, 0, 0, 2); // Uit
        colorFade(255, 0, 0, 2); // Rood
        colorFade(0, 0, 0, 2); // Uit
        colorFade(RL, GL, BL, 2); // Last state

     }
     
      if(ID == 4 && VAL == 0) {
        colorFade(255, 0, 115, 5); // Paars
        RL = 255;
        GL = 0;
        BL = 255;
     }
     
     if(ID == 4 && VAL == 1) {
        colorFade(255, 197, 143, 5); // wit
        RL = 255;
        GL = 197;
        BL = 143;
     }
      
      
      
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   } 
}

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
          // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
          strip.setPixelColor(i, strip.Color(Rold,Gold,Bold)); // Moderately bright green color.
          
        }
        strip.show();
       delay(wait);
      } 
 }

 
  void colorDirect(uint32_t RO, uint32_t GR, uint32_t BL) {
    strip.setBrightness(255);
    Rold = RO;
    Gold = GR;
    Bold = BL;
   for(int i=0;i<16;i++){
          // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
          strip.setPixelColor(i, strip.Color(RO,GR,BL)); // Moderately bright green color.
          
        }
        strip.show();
 }
 



