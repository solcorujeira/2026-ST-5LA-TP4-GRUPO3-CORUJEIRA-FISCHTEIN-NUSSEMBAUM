//Grupo 3: Sol Corujeira, Benjamín Fischtein y Lara Nussembaum

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <U8g2lib.h>
#include <EEPROM.h>

#define EEPROM_SIZE 4

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void imprimirTemp(int temperatura);
void imprimirUmbral(int umbral);

#define DHTPIN 23  // Pin del sensor de temperatura
#define DHTTYPE DHT11
#define BOTON_1 35
#define BOTON_2 34

enum Estados {
  PANTALLA_1,
  ESPERA_1,
  PANTALLA_2,
  SUMA,
  ESPERA_2
};
Estados estado = PANTALLA_1;
unsigned long int tiempoAnterior = 0;
int umbral = 24;
int umbralGuardado;

DHT_Unified dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(BOTON_1, INPUT);
  pinMode(BOTON_2, INPUT);
  Serial.begin(9600);
  Serial.println(F("OLED test"));
  u8g2.begin();
  // Initialize device.
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("Pantalla 1");

  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(0, umbral);
  umbralGuardado = umbral;
}

void loop() {
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  switch (estado) {
    case PANTALLA_1:
      imprimirTemp(event.temperature);
      if (digitalRead(BOTON_1) == LOW) {
        estado = ESPERA_1;
        tiempoAnterior = millis();
        Serial.println("Espera 1");
      }
      break;

    case ESPERA_1:
      if (digitalRead(BOTON_1) == LOW && millis() - tiempoAnterior >= 5000) {
        estado = PANTALLA_2;
        tiempoAnterior = millis();
        Serial.println("Pantalla 2");
      }

      else if (digitalRead(BOTON_1) == HIGH) {
        estado = PANTALLA_1;
        Serial.println("Pantalla 1");
      }

      break;

    case PANTALLA_2:
      u8g2.clearBuffer();
      imprimirUmbral(umbral);
      if (digitalRead(BOTON_1) == LOW && millis() - tiempoAnterior >= 2000){
        estado = SUMA;
        Serial.println("Suma");
      }
      if (digitalRead(BOTON_2) == LOW) {
        tiempoAnterior = millis();
        estado = ESPERA_2;
        Serial.println("Resta");
      }
      break;

    case SUMA:
      if (digitalRead(BOTON_1) == HIGH) {
        estado = PANTALLA_2;
        umbral++;

        Serial.println("Pantalla 2");
      }
      if (digitalRead(BOTON_2) == LOW) {
        estado = ESPERA_2;
        tiempoAnterior = millis();
        Serial.println("Espera 2");
      }
      break;

    case ESPERA_2:
      if (digitalRead(BOTON_2) == LOW && millis() - tiempoAnterior >= 5000) {
        tiempoAnterior = millis();

        if (umbral < 0 || umbral > 100) {
          umbral = 24;
        }

        if (umbral != umbralGuardado) {
          EEPROM.put(0, umbral);
          EEPROM.commit();
          umbralGuardado = umbral;
        }

        estado = PANTALLA_1;
        Serial.println("Pantalla 1");
      }

      else if (digitalRead(BOTON_2) == HIGH) {
        estado = PANTALLA_2;
        umbral--;
        Serial.println("Pantalla 2");
      }
      break;
  }
}

void imprimirTemp(int temperatura) {
  u8g2.clearBuffer();  // clear the internal memory
  char stemp[10];
  sprintf(stemp, "%d", temperatura);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(10, 30, "VA: ");
  u8g2.drawStr(30, 30, stemp);
  u8g2.drawStr(50, 30, "°C");
  imprimirUmbral(umbral);
  u8g2.sendBuffer();
}

void imprimirUmbral(int tempUmbral) {
  char sumb[10];
  sprintf(sumb, "%d", tempUmbral);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(10, 50, "VU: ");
  u8g2.drawStr(30, 50, sumb);
  u8g2.drawStr(50, 50, "°C");
  u8g2.sendBuffer();
}