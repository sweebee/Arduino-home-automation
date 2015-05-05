#include <SPI.h>
#include <MySensor.h>  
#include <DHT.h>  
#include <readVcc.h>

/* MySensors multisensor
 
 Batterylevel
 Humidity
 Temperature
 Motion
 Light 

*/

//********** CONFIG **********************************

    #define NODE_ID 100     // ID of node
    
    #define HUM_ID 0        // ID of HUM
    #define TEMP_ID 1       // ID of TEMP
    #define DHT_PIN 3       // Pin of DHT
    
    #define PIR_ID 2        // ID of PIR
    #define PIR_PIN 4       // Pin of PIR
    
    #define LIGHT_ID 3      // ID of LDR
    #define LIGHT_PIN A0     // Pin of LDR
    
    unsigned long SLEEP_TIME = 120000; // Sleep time between reads
    
    boolean BATTERY_SENSOR = true; // Set to false to disable the battery sensor
    int MIN_V = 2400; // empty voltage (0%)
    int MAX_V = 3200; // full voltage (100%)

//****************************************************

MySensor node;
MyMessage HUMmsg(HUM_ID, V_HUM);
MyMessage TEMPmsg(TEMP_ID, V_TEMP);
MyMessage PIRmsg(PIR_ID, V_TRIPPED);
MyMessage LIGHTmsg(LIGHT_ID, V_LIGHT_LEVEL);

int batteryPcnt;
int oldBatteryPcnt;
DHT dht;
float lastTemp;
float lastHum;
int lastPIR = 2;
int lastLightLevel;

void setup() {
  
  node.begin(NULL, NODE_ID, false);

  // Register all sensors to gateway
  node.present(HUM_ID, S_HUM);
  node.present(TEMP_ID, S_TEMP);
  node.present(PIR_ID, S_DOOR); 
  node.present(LIGHT_ID, S_LIGHT_LEVEL);
  
}

void loop() {
  
  
// *** BATTERY SENSOR **********************************************


  if(BATTERY_SENSOR == true) {
    batteryPcnt = min(map(readVcc(), MIN_V, MAX_V, 0, 100), 100); // Convert voltage to percentage
    
    if (batteryPcnt != oldBatteryPcnt) { // If battery percentage has changed
      node.sendBatteryLevel(batteryPcnt); // Send battery percentage to gateway
      oldBatteryPcnt = batteryPcnt;
    }
  }
  
  
// *** HUMIDITY SENSOR ***********************************************


  float humidity = dht.getHumidity();
  if (isnan(humidity)) {
      Serial.println("Failed reading humidity from DHT");
  } else if (humidity != lastHum) {
      lastHum = humidity;
      node.send(HUMmsg.set(humidity, 1));
  }
  
  
// *** TEMPERATURE SENSOR ********************************************


  float temperature = dht.getTemperature();
  if (isnan(temperature)) {
      Serial.println("Failed reading temperature from DHT");
  } else if (temperature != lastTemp) {
    lastTemp = temperature;
    node.send(TEMPmsg.set(temperature, 1));
  }
  
  
// *** PIR SENSOR ****************************************************


  int PIR = digitalRead(PIR_PIN);
  if (PIR != lastPIR) {
    lastPIR = PIR;
    node.send(PIRmsg.set(PIR == HIGH ? 1 : 0));
  }
  
  
// *** LIGHT SENSOR  *************************************************


  float light = analogRead(LIGHT_PIN);
  int lightLevel = (light / 1023) * 100;
  if (lightLevel != lastLightLevel) {
    lastLightLevel = lightLevel;
    node.send(LIGHTmsg.set(lightLevel));
  }
  
// *** PRINT SENSOR VALUES

  Serial.println();
  
  Serial.print("Battery: ");
  Serial.print(batteryPcnt);
  Serial.println("%");
  
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");  
    
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");

  Serial.print("Motion: ");
  if (PIR == HIGH) {
    Serial.println("true");
  } else {
   Serial.println("false");
  }

  Serial.print("Light: ");
  Serial.print(lightLevel);
  Serial.println("%");
  
  node.sleep(PIR_PIN-2, CHANGE, SLEEP_TIME);
}
