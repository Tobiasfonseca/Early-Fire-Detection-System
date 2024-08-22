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

String URL = "http://192.168.209.42/sensor_project/test.php";

const char* ssid = "NAME"; 
const char* password = "PASSWORD";

float temperature = 0;
float humidity = 0;

float temp(){
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

float hum(){
  sensors_event_t event;
  dht.humidity().getEvent(&event);
  return event.relative_humidity;
}

void setup() {
  // put your setup code here, to run once:
  sensors.begin();
  dht.begin();
  connectWiFi();
}

void loop() {
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
    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
    } else {
      // HTTP header has been send and Server response header has been handled
    }
  } else {
  }
  
  http.end();  //Close connection
  delay(1000*30);
}


void LoadData() {
  //-----------------------------------------------------------
  temperature = temp(); //Celsius
  humidity = hum();
  //-----------------------------------------------------------
  // Check if any reads failed.
  if (isnan(temperature) || isnan(humidity)) {
    temperature = 0;
    humidity = 0;
  }
  //-----------------------------------------------------------
}

void connectWiFi() {
  WiFi.mode(WIFI_OFF);
  delay(1000);
  //This line hides the viewing of ESP as wifi hotspot
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}
