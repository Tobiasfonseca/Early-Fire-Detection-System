  #include <MQ2.h>
  #include <Wire.h> 
 // #include <LiquidCrystal_I2C.h>
  //I2C pins declaration
  //LiquidCrystal_I2C lcd(0x27, 16, 2);
int Analog_Input = 4;
double lpg, co, smoke;

MQ2 mq2(Analog_Input);

void setup(){
  Serial.begin(9600);
  mq2.begin();
}
void loop(){
  float* values= mq2.read(true); //set it false if you don't want to print the values in the Serial
  //lpg = values[0];
  lpg = mq2.readLPG();
  //co = values[1];
  co = mq2.readCO();
  //smoke = values[2];
  smoke = mq2.readSmoke();
  delay(100);
}