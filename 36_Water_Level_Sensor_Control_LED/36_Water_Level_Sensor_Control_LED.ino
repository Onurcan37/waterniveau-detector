#include <Wire.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_ST77xx.h>

#define SERIAL Serial

#define TFT_CS        10
#define TFT_RST        9 // Of instellen op -1 en verbinden met de Arduino RESET-pin
#define TFT_DC         8

// Arrays om ontvangen gegevens op te slaan
unsigned char low_data[8] = {0};
unsigned char high_data[12] = {0};

#define NO_TOUCH       0xFE
#define THRESHOLD      100
#define ATTINY1_HIGH_ADDR   0x78
#define ATTINY2_LOW_ADDR   0x77

// TFT display initialisatie
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Functie om 12 secties van gegevens van ATTINY1_HIGH_ADDR te verkrijgen
void getHigh12SectionValue(void)
{
  // Reset van high_data array
  memset(high_data, 0, sizeof(high_data));
  // Verzoek om 12 bytes van ATTINY1_HIGH_ADDR
  Wire.requestFrom(ATTINY1_HIGH_ADDR, 12);
  while (12 != Wire.available());

  // Ontvangen van de 12 bytes en opslaan in high_data array
  for (int i = 0; i < 12; i++) {
    high_data[i] = Wire.read();
  }
  delay(10);
}

// Functie om 8 secties van gegevens van ATTINY2_LOW_ADDR te verkrijgen
void getLow8SectionValue(void)
{
  // Reset van low_data array
  memset(low_data, 0, sizeof(low_data));
  // Verzoek om 8 bytes van ATTINY2_LOW_ADDR
  Wire.requestFrom(ATTINY2_LOW_ADDR, 8);
  while (8 != Wire.available());

  // Ontvangen van de 8 bytes en opslaan in low_data array
  for (int i = 0; i < 8 ; i++) {
    low_data[i] = Wire.read();
  }
  delay(10);
}

// Functie voor het analyseren van de ontvangen gegevens
void check()
{
  int sensorvalue_min = 250;
  int sensorvalue_max = 255;
  int low_count = 0;
  int high_count = 0;
  
  while (1)
  {
    uint32_t touch_val = 0;
    uint8_t trig_section = 0;
    low_count = 0;
    high_count = 0;
    
    // Verkrijgen van gegevens van beide adressen
    getLow8SectionValue();
    getHigh12SectionValue();

    // Analyseren van low_data
    Serial.println("low 8 sections value = ");
    for (int i = 0; i < 8; i++)
    {
      Serial.print(low_data[i]);
      Serial.print(".");
      
      // Controleren op sensorwaarden binnen de drempel
      if (low_data[i] >= sensorvalue_min && low_data[i] <= sensorvalue_max)
      {
        low_count++;
      }
      
      // Controleren op alle waarden binnen de drempel
      if (low_count == 8)
      {
        Serial.print("      ");
        Serial.print("PASS");
      }
    }
    Serial.println("  ");
    Serial.println("  ");
    
    // Analyseren van high_data
    Serial.println("high 12 sections value = ");
    for (int i = 0; i < 12; i++)
    {
      Serial.print(high_data[i]);
      Serial.print(".");
      
      // Controleren op sensorwaarden binnen de drempel
      if (high_data[i] >= sensorvalue_min && high_data[i] <= sensorvalue_max)
      {
        high_count++;
      }
      
      // Controleren op alle waarden binnen de drempel
      if (high_count == 12)
      {
        Serial.print("      ");
        Serial.print("PASS");
      }
    }

    Serial.println("  ");
    Serial.println("  ");

    // Berekenen van touch_val op basis van ontvangen gegevens
    for (int i = 0 ; i < 8; i++) {
      if (low_data[i] > THRESHOLD) {
        touch_val |= 1 << i;
      }
    }
    for (int i = 0 ; i < 12; i++) {
      if (high_data[i] > THRESHOLD) {
        touch_val |= (uint32_t)1 << (8 + i);
      }
    }

    // Bepalen van trig_section voor weergave op TFT
    while (touch_val & 0x01)
    {
      trig_section++;
      touch_val >>= 1;
    }

    // TFT-display bijwerken met waterstandgegevens
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.println("Waterniveau: ");

    tft.print(trig_section * 5);
    tft.println(" millimeter");

    delay(1000); // Wachten voor volgende update
  }
}

void setup() {
  SERIAL.begin(115200);
  Wire.begin();

  tft.initR(INITR_BLACKTAB);  // Initialiseren van ST7735S chip, black tab
  tft.setRotation(-45); // Rotatie van het scherm
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
}

void loop()
{
  check();
} // Einde van de loop
