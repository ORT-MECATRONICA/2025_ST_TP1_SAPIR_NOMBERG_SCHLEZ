//Nomberg, Sapir, Schlez
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SDA_PIN 21
#define DHTPIN 23
#define SCL_PIN 22

#define DHTTYPE DHT11  
DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(9600);

  dht.begin();
  
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);

  delayMS = sensor.min_delay / 1000;

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  delay(2000);
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Lectura de temperatura:");
  display.display();
}

void loop() {
  delay(delayMS);

  sensors_event_t event;
  dht.temperature().getEvent(&event);

  if (isnan(event.temperature)) {
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("Error leyendo temperatura");
  }
  else {
    display.clearDisplay();
    display.setCursor(0, 10);
    display.print("Temp: ");
    display.print(event.temperature);
    display.println(" C");
  }

  display.display();
}
