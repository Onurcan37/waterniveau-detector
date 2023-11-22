#include <Adafruit_ST7735.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_ST77xx.h>

#define TFT_CS        10
  #define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
  #define TFT_DC         8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int sensorPin = A0; 
int sensorValue = 0;

void setup()
{
  Serial.begin(9600);
  Serial.print(F("Hello! ST77xx TFT Test"));
  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

  tft.setRotation(-45);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
}

void loop() 
{
  sensorValue = analogRead(sensorPin);
  tft.fillScreen(ST77XX_BLACK);

  tft.setCursor(0, 0);
  tft.println("Waterniveau");
  tft.println(sensorValue);

  Serial.println(sensorValue);

  if (sensorValue > 670)
  {
    tft.println("Zeer hoog");
  }
  else if (sensorValue > 664) 
  {
    tft.println("Hoog");
  }
  else if (sensorValue > 660)
  {
  tft.println("Gemiddeld");
  }
  else if (sensorValue > 650)
  {
  tft.println("Laag");
  }
  else
  {
  tft.println("Droog");
  }
  Serial.println(sensorValue);

  delay(100);
}
