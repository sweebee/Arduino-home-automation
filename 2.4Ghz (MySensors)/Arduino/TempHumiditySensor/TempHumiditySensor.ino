#include <SPI.h>
#include <MySensor.h>  
#include <DHT.h>  
#include <readVcc.h> 

//********** CONFIG **********************************

    #define NODE_ID 210             // ID of node
    #define CHILD_ID_HUM 0          // ID of humidity
    #define CHILD_ID_TEMP 1         // ID of temperature
    #define SENSOR_PIN 3            // Pin connected to the sensor

    #define SLEEP_TIME 60000        // Sleep time between reads
    
    #define MIN_V 2000              // empty voltage (0%)
    #define MAX_V 3200              // full voltage (100%)
    

//****************************************************

MySensor node;
int oldBatteryPcnt = 0;
DHT dht;
float lastTemp;
float lastHum;
boolean metric = true; 
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);


void setup()  
{ 
  node.begin(NULL,NODE_ID,false);
  dht.setup(SENSOR_PIN); 

  // Register all sensors to node (they will be created as child devices)
  node.present(CHILD_ID_HUM, S_HUM);
  node.present(CHILD_ID_TEMP, S_TEMP);
  
  metric = node.getConfig().isMetric;
}

void loop()      
{  
  
  // Measure battery
    int batteryPcnt = min(map(readVcc(), MIN_V, MAX_V, 0, 100), 100); // Convert voltage to percentage
    
    if (batteryPcnt != oldBatteryPcnt) { // If battery percentage has changed
      node.sendBatteryLevel(batteryPcnt); // Send battery percentage to gateway
      oldBatteryPcnt = batteryPcnt;
  }
  
  // Do sensor things
  delay(dht.getMinimumSamplingPeriod());

  float temperature = dht.getTemperature();
  if (isnan(temperature)) {
      Serial.println("Failed reading temperature from DHT");
  } else if (temperature != lastTemp) {
    lastTemp = temperature;
    if (!metric) {
      temperature = dht.toFahrenheit(temperature);
    }
    node.send(msgTemp.set(temperature, 1));
    Serial.print("T: ");
    Serial.println(temperature);
  }
  
  float humidity = dht.getHumidity();
  if (isnan(humidity)) {
      Serial.println("Failed reading humidity from DHT");
  } else if (humidity != lastHum) {
      lastHum = humidity;
      node.send(msgHum.set(humidity, 1));
      Serial.print("H: ");
      Serial.println(humidity);
  }

  node.sleep(SLEEP_TIME); //sleep a bit
}
