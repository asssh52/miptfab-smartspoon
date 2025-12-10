#include "HX711.h"

const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;
const int TILT_PIN = 5;

HX711 scale;

unsigned long lastUpTime = 0;
volatile bool tiltDetected = false;
int tiltCount = 0;

void setup() {
  Serial.begin(9600);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println("калибровка тензодатчика...");
  scale.set_scale(8137);
  scale.tare();

  pinMode(TILT_PIN, INPUT_PULLUP);

  Serial.println("готово!");
}

void loop() {
  if (scale.is_ready()) {
    float weight = scale.get_units(10);
    if (weight < 0) weight = -weight;
    Serial.print("Вес: ");
    Serial.print(weight, 3);
    Serial.print(" г");
  } else {
    Serial.print("Вес: ошибка HX711");
  }

  int tiltState = digitalRead(TILT_PIN);
  if (tiltState == HIGH && !tiltDetected) {
    tiltDetected = true;
    tiltCount++;
    lastUpTime = millis();
    Serial.print("  Оборотов: ");
    Serial.print(tiltCount);
  } else if (tiltState == LOW) {
    tiltDetected = false;
  }

  Serial.println();
  delay(500);
}
