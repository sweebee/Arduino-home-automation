#include <MySensor.h>
#include <readVcc.h>
#include <SPI.h>
#include <DHT.h>

#define NODE_ID 100                      // ID of node
#define SLEEP_TIME 10000                 // Sleep time between reports (in milliseconds)

#define CHILD_ID_PIR 1                   // ID of the sensor PIR
#define CHILD_ID_HUM 2                   // ID of the sensor HUM
#define CHILD_ID_TEMP 3                  // ID of the sensor TEMP
#define CHILD_ID_LIGHT 4                 // ID of the sensor LIGHT

#define PIR_SENSOR_DIGITAL 3             // PIR pin
#define HUMIDITY_SENSOR_DIGITAL_PIN 4    // DHT pin
#define LIGHT_SENSOR_ANALOG_PIN 0        // LDR pin

#define MIN_V 2700 // empty voltage (0%)
#define MAX_V 3200 // full voltage (100%)

MySensor gw;
// Initialize Variables
MyMessage msgPir(CHILD_ID_PIR, V_TRIPPED);
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);
MyMessage msgLight(CHILD_ID_LIGHT, V_LIGHT_LEVEL);

DHT dht;
float lastTemp;
float lastHum;
boolean metric = true;
int oldBatteryPcnt;
int lastLightLevel;
int sentValue;

void setup()
{
  gw.begin(NULL, NODE_ID, false);
  
  // Register all sensors to gateway (they will be created as child devices)
  gw.present(CHILD_ID_PIR, S_MOTION);
  gw.present(CHILD_ID_HUM, S_HUM);
  gw.present(CHILD_ID_TEMP, S_TEMP);
  gw.present(CHILD_ID_LIGHT, S_LIGHT_LEVEL);

  pinMode(PIR_SENSOR_DIGITAL, INPUT);
  digitalWrite(PIR_SENSOR_DIGITAL, HIGH);
  
  dht.setup(HUMIDITY_SENSOR_DIGITAL_PIN);
}

void loop()
{
  Serial.println("Waking up...");
  sendBattery();
  sendPir();
  sendTemp();
  sendHum();
  sendLight();
  Serial.println("Going to sleep...");
  Serial.println("");
  gw.sleep(PIR_SENSOR_DIGITAL-2, CHANGE, SLEEP_TIME);
}


void sendBattery() // Measure battery
{  
  int batteryPcnt = min(map(readVcc(), MIN_V, MAX_V, 0, 100), 100);
  if (batteryPcnt != oldBatteryPcnt) {
    gw.sendBatteryLevel(batteryPcnt); // Send battery percentage
    oldBatteryPcnt = batteryPcnt;
  }
  Serial.print("---------- Battery: ");
  Serial.println(batteryPcnt);
}



void sendPir() // Get value of PIR
{
  int value = digitalRead(PIR_SENSOR_DIGITAL); // Get value of PIR
  if (value != sentValue) { // If status of PIR has changed
    gw.send(msgPir.set(value == HIGH ? 1 : 0)); // Send PIR status to gateway
    sentValue = value;
  }
  Serial.print("---------- PIR: ");
  Serial.println(value);
}



void sendTemp() // Get temperature
{
  float temperature = dht.getTemperature();
  if (isnan(temperature)) {
    Serial.println("Failed reading temperature from DHT");
  } else {   
    if (temperature != lastTemp) {
      gw.send(msgTemp.set(temperature, 1));
      lastTemp = temperature;
    }
    Serial.print("---------- Temp: ");
    Serial.println(temperature);
  }
}


void sendHum() // Get humidity
{
  float humidity = dht.getHumidity();
  if (isnan(humidity)) {
    Serial.println("Failed reading humidity from DHT");
  } else { 
    if (humidity != lastHum) {
      gw.send(msgHum.set(humidity, 1));
      lastHum = humidity;
    }
    Serial.print("---------- Humidity: ");
    Serial.println(humidity);
  }
}


void sendLight() // Get light level
{
  int lightLevel = (1023 - analogRead(LIGHT_SENSOR_ANALOG_PIN)) / 10.23;
  if (lightLevel != lastLightLevel) {
    gw.send(msgLight.set(lightLevel));
    lastLightLevel = lightLevel;
  }
  Serial.print("---------- Light: ");
  Serial.println(lightLevel);
}
