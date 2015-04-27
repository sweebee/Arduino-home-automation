#include <SPI.h>
#include <MySensor.h>  
#include <DHT.h>  
#include <readVcc.h> 

//********** CONFIG **********************************

    #define NODE_ID 13          // ID of node
    #define CHILD_ID_HUM 0      // ID of humidity
    #define CHILD_ID_TEMP 1     // ID of temperature
    #define SENSOR_PIN 3        // Pin connected to the sensor
    #define SENSOR_POWER 4      // Power pin of sensor, only HIGH when measuring to reduce power

    unsigned long SLEEP_TIME = 60000; // Sleep time between reads

    int MIN_V = 2400; // empty voltage (0%)
    int MAX_V = 3200; // full voltage (100%)

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
  pinMode(SENSOR_POWER, OUTPUT);
  node.begin(NULL,NODE_ID,false);
  dht.setup(SENSOR_PIN); 

  // Send the Sketch Version Information to the Gateway
  node.sendSketchInfo("Temp & Hum sensor", "1.0");

  // Register all sensors to node (they will be created as child devices)
  node.present(CHILD_ID_HUM, S_HUM);
  node.present(CHILD_ID_TEMP, S_TEMP);
  
  metric = node.getConfig().isMetric;
}

void loop()      
{  
  
  // Measure battery
  float batteryV = readVcc();
  int batteryPcnt = (((batteryV - MIN_V) / (MAX_V - MIN_V)) * 100 );

  if (batteryPcnt > 100) {
    batteryPcnt = 100;
  }
  if (batteryPcnt != oldBatteryPcnt) {
    node.sendBatteryLevel(batteryPcnt); // Send battery percentage
    oldBatteryPcnt = batteryPcnt;
  }
  
  // Do sensor things

  digitalWrite(SENSOR_POWER, HIGH); // Power up the sensor
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
  digitalWrite(ledPin, LOW); // Power down the sensor
  node.sleep(SLEEP_TIME); //sleep a bit
}
