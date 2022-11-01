// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "TMPL42N-irNO"
#define BLYNK_DEVICE_NAME "ESLproject"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_WROVER_BOARD
//#define USE_TTGO_T7

#include "BlynkEdgent.h"

#include <Wire.h>
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define irPin 34
#define redLed 15
#define greenLed 19
int ir_val=0;
float temp_value = 0;
boolean temp_check = false;

BLYNK_CONNECTED() {
  Blynk.syncAll();
}

void setup()
{
  pinMode(irPin,INPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  Serial.begin(115200);
  mlx.begin();
  delay(100);

  BlynkEdgent.begin();

  lcd.begin();
  //lcd.noBacklight();   // ปิด backlight
  lcd.backlight();       // เปิด backlight 
}

void loop() {
  BlynkEdgent.run();
  sendSensor();
  i2c_display();
  ir_sensor();
  temp_compare();
}

float temp_avg(){
  float temp_in = 0;
  for(int i =0;i<10;i++){
    temp_in += mlx.readObjectTempC();
  }
  return (temp_in / 10.0)+1;
}

void sendSensor(){ 
  Serial.println("***DEBUG TEMP***");
  Serial.print("Temperature : ");
  Serial.print(mlx.readObjectTempC()); 
  Serial.println(" Celsius");
 
  Serial.print("Temperature : "); 
  Serial.print(mlx.readObjectTempF());
  Serial.println(" Fahrenheit");

  Serial.print("TemperatureConvert : "); 
  Serial.print(((mlx.readObjectTempF())-32)/1.8);
  Serial.println(" Celsius");

  Serial.println();

  if(ir_val == 1){
    temp_value = temp_avg();
    if(temp_value > 33){
      temp_check = true;
      Blynk.virtualWrite(V5, temp_value);
    }
  }
  else{
    Blynk.virtualWrite(V5, 0);
  }
}

void i2c_display(){
  
  lcd.home();
  lcd.print("Wash your hand!");
  lcd.setCursor(0, 1);
  lcd.print("TempC:           ");
  lcd.setCursor(6, 1);
  if(temp_check){
    lcd.print(temp_value);
    temp_check = false;
  }
  else if(ir_val==1 && !temp_check){
    lcd.setCursor(6, 1);
    lcd.print("STAND BY        ");
  }
}


void ir_sensor(){
 int ir_in = analogRead(irPin);
  if(ir_in >= 4000){
    ir_val = 0; 
  }
  else{
    ir_val = 1;
  }
  Serial.print("VIR_in: ");
  Serial.println(ir_in);
  Serial.print("VIR_out: ");
  Serial.println(ir_val);
}

void temp_compare(){
  if(ir_val == 0){
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
  }
  else{
    if(temp_value <= 37.5){
      digitalWrite(redLed, HIGH);
      digitalWrite(greenLed, LOW);
    }
    else {
      digitalWrite(redLed, LOW);
      digitalWrite(greenLed, HIGH);      
    }
  }

}
