#include <DHT.h>
#include <DHT_U.h>
#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>

#define DHTTYPE DHT22

uint8_t DHTPIN = 19; 

OneWire ds(4);
DallasTemperature sensors(&ds);
DHT_Unified dht(DHTPIN, DHTTYPE);

float temp(){
  float t;
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

float hum(){
  sensors_event_t event;
  dht.humidity().getEvent(&event);
  return event.relative_humidity;
}

void processing(){
  float temperature = temp();
  float humidity = hum();
  Serial.println("Temperatura: " + String(temperature));
  Serial.println("Humedad: " + String(humidity));
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  sensors.begin();
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  processing();
  delay(2000);
}
