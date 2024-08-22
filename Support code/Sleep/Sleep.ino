//RTC_DATA_ATTR int bootCount = 0;

#include <WiFi.h>
#include <HTTPClient.h>
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

String URL = "http://192.168.3.10/sensor_project/test.php";

const char* ssid = "NAME"; 
const char* password = "PASSWORD"; 

unsigned long time1;
unsigned long time2;

float temperature = 0;
float humidity = 0;

void RTC_IRAM_ATTR esp_wake_deep_sleep() {
    esp_default_wake_deep_sleep();
    //bootCount++;
}

float temp(){
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

float hum(){
  sensors_event_t event;
  dht.humidity().getEvent(&event);
  return event.relative_humidity;
}

void connectWiFi() {
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void LoadData() {
  temperature = temp();
  humidity = hum();
  if (isnan(temperature) || isnan(humidity)) {
    temperature = 0;
    humidity = 0;
  }
}

void Upload(){
  if(WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  LoadData();
  String postData = "temperature=" + String(temperature) + "&humidity=" + String(humidity);
  
  HTTPClient http;
  http.begin(URL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  int httpCode = http.POST(postData);
  String payload = "";

  if(httpCode > 0) {
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
    } else {
    }
  } else {
  }
  
  http.end();
}

void setup()
{
  //Serial.begin(115200);
  time1 = millis();
  sensors.begin();
  dht.begin();
  connectWiFi();
  Upload();
  time2 = millis();
  //Serial.println("Starting...");
  //Serial.println(time1);
  //Serial.println(time2);
  //Serial.println(60 - (time2 - time1)/1000);
  esp_deep_sleep((60000 - (time2 - time1)) * 1000); 
}

void loop()
{
  // This function will never be executed due to Deep Sleep mode
}
