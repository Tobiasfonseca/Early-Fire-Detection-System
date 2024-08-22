#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>

#define DHTTYPE DHT22

#define         MQ_PIN                       (14)     //define which analog input channel you are going to use
#define         RL_VALUE                     (5)     //define the load resistance on the board, in kilo ohms
#define         RO_CLEAN_AIR_FACTOR          (9.83)  //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
                                                     //which is derived from the chart in datasheet

/***********************Software Related Macros************************************/
#define         CALIBARAION_SAMPLE_TIMES     (50)    //define how many samples you are going to take in the calibration phase
#define         CALIBRATION_SAMPLE_INTERVAL  (500)   //define the time interal(in milisecond) between each samples in the
                                                     //cablibration phase
#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interal(in milisecond) between each samples in 
                                                     //normal operation

/**********************Application Related Macros**********************************/
#define         GAS_CO                       (1)
#define         GAS_SMOKE                    (2)

float           COCurve[3]  =  {2.3,0.72,-0.34};    //two points are taken from the curve. 
                                                    //with these two points, a line is formed which is "approximately equivalent" 
                                                    //to the original curve.
                                                    //data format:{ x, y, slope}; point1: (lg200, 0.72), point2: (lg10000,  0.15) 
float           SmokeCurve[3] ={2.3,0.53,-0.44};    //two points are taken from the curve. 
                                                    //with these two points, a line is formed which is "approximately equivalent" 
                                                    //to the original curve.
                                                    //data format:{ x, y, slope}; point1: (lg200, 0.53), point2: (lg10000,  -0.22)                                                     
float           Ro           =  10;                 //Ro is initialized to 10 kilo ohms

uint8_t DHTPIN = 19;

OneWire ds(4);
DallasTemperature sensors(&ds);
DHT_Unified dht(DHTPIN, DHTTYPE);

String URL = "http://192.168.68.112/sensor_project/test.php";

const char* ssid = "NAME"; 
const char* password = "PASSWORD"; 

float temperature = 0;
float humidity    = 0;
float co          = 0;
float smoke       = 0;

/***************** MQ2 calibration *****************/

float MQCalibration(int mq_pin){
  int i;
  float val=0;

  for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {            
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBARAION_SAMPLE_TIMES;
  val = val/RO_CLEAN_AIR_FACTOR;

  return val; 
}

float MQResistanceCalculation(int raw_adc){
  return ( ((float)RL_VALUE*(4095-raw_adc)/raw_adc));
}

/***************** Functions for getting samples ****************/
float MQRead(int mq_pin){
  int i;
  float rs=0;

  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }
  rs = rs/READ_SAMPLE_TIMES;
  return rs;  
}

int MQGetGasPercentage(float rs_ro_ratio, int gas_id){
  if ( gas_id == GAS_CO ){
     return MQGetPercentage(rs_ro_ratio,COCurve);
  } else if ( gas_id == GAS_SMOKE ) {
     return MQGetPercentage(rs_ro_ratio,SmokeCurve);
  }
  return 0;
}

int  MQGetPercentage(float rs_ro_ratio, float *pcurve){
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
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

void setup(){      
  Ro = MQCalibration(MQ_PIN);
  sensors.begin();
  dht.begin();
  analogReadResolution(12);
}

void loop(){
  bool meas = true;

  while(meas){
    LoadData();
    if(temperature > 40 || humidity < 30 || smoke > 1000){
      meas = false;
    }
    delay(30000);
  }
  

  if(WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  String postData = "temperature=" + String(temperature) + "&humidity=" + String(humidity) +
                    "&co=" + String(co) +"&smoke=" + String(smoke);
  HTTPClient http;
  http.begin(URL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  int httpCode = http.POST(postData);
  String payload = "";

  if(httpCode > 0) {
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
    }
  }
  
  http.end();
  WiFi.mode(WIFI_OFF);
  delay(500);
}


void LoadData(){
  temperature = temp();
  humidity    = hum();
  co          = MQGetGasPercentage(MQRead(14)/Ro,GAS_CO);
  smoke       = MQGetGasPercentage(MQRead(14)/Ro,GAS_SMOKE);
  
  if (isnan(temperature) || isnan(humidity)) {
    temperature = 0;
    humidity = 0;
  }
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
