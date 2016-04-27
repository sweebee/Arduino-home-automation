#include <MySensor.h>
#include <readVcc.h>
#include <SPI.h>
#include <DHT.h>


#define NODE_ID 100                       // ID of node
unsigned long SLEEP_TIME = 60000;        // Sleep time between reports (in milliseconds)

#define CHILD_ID_PIR 1                   // Id of the sensor PIR
#define CHILD_ID_HUM 2                   // Id of the sensor HUM
#define CHILD_ID_TEMP 3                  // Id of the sensor TEMP
#define CHILD_ID_LIGHT 4                 // Id of the sensor LIGHT

#define PIR_SENSOR_DIGITAL 3           // The digital input you attached your motion sensor.  (Only 2 and 3 generates interrupt!)
#define INTERRUPT PIR_SENSOR_DIGITAL-2 // Usually the interrupt = pin -2 (on uno/nano anyway)
#define HUMIDITY_SENSOR_DIGITAL_PIN 4
#define LIGHT_SENSOR_ANALOG_PIN 0

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


int MIN_V = 2700; // empty voltage (0%)
int MAX_V = 3200; // full voltage (100%)

void setup()
{
  gw.begin(NULL, NODE_ID, false);

  //PIR
  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("Motion Sensor", "1.0");

  pinMode(PIR_SENSOR_DIGITAL, INPUT);      // sets the motion sensor digital pin as input
  digitalWrite(PIR_SENSOR_DIGITAL, HIGH);
  // Register all sensors to gw (they will be created as child devices)
  gw.present(CHILD_ID_PIR, S_MOTION);
  
  //DHT
  dht.setup(HUMIDITY_SENSOR_DIGITAL_PIN);

  // Send the Sketch Version Information to the Gateway
  gw.sendSketchInfo("Humidity", "1.0");

  // Register all sensors to gw (they will be created as child devices)
  gw.present(CHILD_ID_HUM, S_HUM);
  gw.present(CHILD_ID_TEMP, S_TEMP);

  metric = gw.getConfig().isMetric;
  
  //LIGHT
  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("Light Sensor", "1.0");

  // Register all sensors to gateway (they will be created as child devices)
  gw.present(CHILD_ID_LIGHT, S_LIGHT_LEVEL);

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
    gw.sendBatteryLevel(batteryPcnt); // Send battery percentage
    oldBatteryPcnt = batteryPcnt;
  }
  Serial.print("---------- Battery: ");
  Serial.println(batteryPcnt);

  // Read digital motion value
  boolean tripped = digitalRead(PIR_SENSOR_DIGITAL) == HIGH;
  Serial.print("---------- PIR: ");
  Serial.println(tripped);
  gw.send(msgPir.set(tripped ? "1" : "0")); // Send tripped value to gw

  float temperature = dht.getTemperature();
  if (isnan(temperature)) {
    Serial.println("Failed reading temperature from DHT");
  }  
    
    if (!metric) {
      temperature = dht.toFahrenheit(temperature);
    }
    
    if (temperature != lastTemp) {
      gw.send(msgTemp.set(temperature, 1));
      Serial.print("---------- Temp: ");
      Serial.println(temperature);
      lastTemp = temperature;
   }

  float humidity = dht.getHumidity();
  if (isnan(humidity)) {
    Serial.println("Failed reading humidity from DHT");
  } 
  
  if (humidity != lastHum) {
    gw.send(msgHum.set(humidity, 1));
    Serial.print("---------- Humidity: ");
    Serial.println(humidity);
    lastHum = humidity;
  }
  // Light
  int lightLevel = (1023 - analogRead(LIGHT_SENSOR_ANALOG_PIN)) / 10.23;
  //Serial.println(lightLevel);
  if (lightLevel != lastLightLevel) {
    gw.send(msgLight.set(lightLevel));
    lastLightLevel = lightLevel;
    Serial.print("---------- Light: ");
    Serial.println(lightLevel);
  }
  // Sleep until interrupt comes in on motion sensor. Send update every two minute.
  gw.sleep(INTERRUPT, CHANGE, SLEEP_TIME);
}
