#include <SPI.h>
#include <MySensor.h> 
#include "DHT.h"

// ********** CONFIG **********************************

    #define NODE_ID 11
    #define CHILD_ID_HUM 0
    #define CHILD_ID_TEMP 1
    #define DHT_PIN 3
    #define SLEEP_TIME 60000

// ****************************************************


MySensor gw;
float hum;
float temp;
float lastHum;
float lastTemp;
int force;
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);

DHT dht(DHT_PIN, DHT22);

void setup() {
  gw.begin(NULL, NODE_ID, false);
  dht.begin();
  gw.sendSketchInfo("Temp/Hum sensor", "1.0");
  gw.present(CHILD_ID_HUM, S_HUM);
  gw.present(CHILD_ID_TEMP, S_TEMP);
}

void loop() {
  delay(500);
  hum = dht.readHumidity();
  temp = dht.readTemperature();

  if(hum != lastHum || force == 5){
    gw.send(msgHum.set(hum, 1));
    lastHum = hum;
  }

  if(temp != lastTemp || force == 5){
    gw.send(msgTemp.set(temp, 1));
    lastTemp = temp;
  }

  if(force == 5){
    force = 0;
  } else {
    force++;
  }

  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println("%");
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println("*C");

  gw.sleep(SLEEP_TIME);
}
