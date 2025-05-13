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
#define SCL_PIN 22
#define DHTPIN 23

#define SW1_PIN 34  
#define SW2_PIN 35  
#define LED_PIN 25    

#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

uint32_t delayMS;
int umbral = 25;  // Umbral inicial en grados Celsius
int temperaturaActual = 0;

enum Estado {
  PANTALLA_1,
  PANTALLA_2
};

Estado estadoActual = PANTALLA_1;

unsigned long tiempoAnterior = 0;
unsigned long intervalo = 500; 
unsigned long tiempoBotonesAnterior = 0;
unsigned long intervaloBotones = 50; 

bool sw1Anterior = HIGH;
bool sw2Anterior = HIGH;

void setup() {
  Serial.begin(9600);

  dht.begin();
  pinMode(SW1_PIN, INPUT_PULLUP);
  pinMode(SW2_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
}

void loop() {
  unsigned long tiempoActual = millis();
  
  if (tiempoActual - tiempoAnterior >= intervalo) {
    temperaturaActual = obtenerTemperatura();
    tiempoAnterior = tiempoActual;  
  }

  if (tiempoActual - tiempoBotonesAnterior >= intervaloBotones) {
    bool sw1 = digitalRead(SW1_PIN);
    bool sw2 = digitalRead(SW2_PIN);

    // Verificar si ambos botones están presionados
    if (sw1 == LOW && sw2 == LOW) {
      // Verificar si uno de los botones fue previamente presionado y ahora está suelto
      if (sw1Anterior == HIGH || sw2Anterior == HIGH) {
        // Cambiar el estado de la pantalla entre PANTALLA_1 y PANTALLA_2
        if (estadoActual == PANTALLA_1) {
          estadoActual = PANTALLA_2;
        } else {
          estadoActual = PANTALLA_1;
        }
      }
    }

    // En PANTALLA_2, modificar el umbral al presionar SW1 o SW2
    if (estadoActual == PANTALLA_2) {
      // Verificar si el botón SW1 fue presionado (se soltó de LOW a HIGH)
      if (sw1Anterior == LOW && sw1 == HIGH) {
        umbral++;  // Aumentar el umbral
      }
      // Verificar si el botón SW2 fue presionado (se soltó de LOW a HIGH)
      if (sw2Anterior == LOW && sw2 == HIGH) {
        umbral--;  // Disminuir el umbral
      }
    }

    // Guardar el estado anterior de los botones
    sw1Anterior = sw1;
    sw2Anterior = sw2;

    // Actualizar el tiempo de los botones para evitar rebotes
    tiempoBotonesAnterior = tiempoActual;
  }

  // Mostrar la pantalla según el estado actual
  switch (estadoActual) {
    case PANTALLA_1:
      mostrarPantalla1();
      break;
    case PANTALLA_2:
      mostrarPantalla2();
      break;
  }

  // Controlar el LED en función de la temperatura
  if (temperaturaActual > umbral) {
    digitalWrite(LED_PIN, HIGH);  // LED encendido si temperatura supera el umbral
  } else {
    digitalWrite(LED_PIN, LOW);   // LED apagado si temperatura no supera el umbral
  }
}

int obtenerTemperatura() {
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  
  if (isnan(event.temperature)) {
    return 0;
  }
  
  return event.temperature;
}

void mostrarPantalla1() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  
  display.println("Temp Actual:");
  display.print(temperaturaActual);
  display.println(" C");
  
  display.print("Valor Umbral: ");
  display.print(umbral);
  display.println(" C");

  display.display();
}

void mostrarPantalla2() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  
  display.println("Modificar Umbral:");
  display.print("Umbral: ");
  display.print(umbral);
  display.println(" C");

  display.println("SW1: Aumentar");
  display.println("SW2: Disminuir");

  display.display();
}
