#include <Wire.h>;
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <BH1750.h>
LiquidCrystal_I2C lcd(0x3F,16,2); //SCL A5 SDA A4
int LED[3] = {5,6,7} ;
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
unsigned long currentTime;
unsigned long lastTime;
BH1750 lightMeter;
void setup() 
{
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  delay(100);
  lcd.clear();
  dht.begin();
    for (int i = 0; i < 3; i++) {
    pinMode(LED[i], OUTPUT);
    digitalWrite(LED[i], LOW);
  }
  lightMeter.begin();
}
void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  float lux = lightMeter.readLightLevel();
  currentTime = millis();
  digitalWrite(LED[0],1);                               //den xanh bat sang khi hoan tat khoi dong
  lcd.display();
  if ((currentTime - lastTime) >= 1000) {
    lcdDisplay(temperature, humidity,lux);              //hien thi thong tin nhiet do, do am, anh sang len lcd
    lastTime = currentTime;
  }
  delay(1000);
  if(lux<60) 
    { 
      digitalWrite(LED[1],1);                               //dieu kien sang den trang khi anh sang thap
      delay(2000);
    }
  else digitalWrite(LED[1],0);
  if(humidity>7) digitalWrite(LED[2],1);                //dieu kien sang den vang khi do am cao
  else digitalWrite(LED[2],0);
  delay(1000);
}
void lcdDisplay(float t, float h,float l) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("temp/humid/lux: ");
  lcd.setCursor(0, 1);
  lcd.print(round(t));
  lcd.print("  /");
  lcd.setCursor(5, 1);
  lcd.print(round(h));
  lcd.print("    /");
  lcd.setCursor(11, 1);
  lcd.print(round(l));
}