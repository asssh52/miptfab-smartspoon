#include "HX711.h"

const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;
const int TILT_PIN = 5;

HX711 scale;

unsigned long lastUpTime = 0;
bool          timerStarted = 0;
volatile bool tiltDetected = false;
int tiltCount = 0;

void setup() {
  Serial.begin(9600);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println("калибровка тензодатчика...");
  scale.set_scale(16560); //17269 - high //16560 - small
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
    Serial.print("  Оборотов: ");
    Serial.print(tiltCount);
  }
  else if (tiltState == LOW) {
    tiltDetected = false;
  }

  if (!tiltState && !timerStarted) {
    lastUpTime = millis();
    timerStarted = 1;
  }
  else if (tiltState){
    timerStarted = 0;
  }

  if (!tiltState && timerStarted) {
    unsigned long now = millis();
    unsigned long time = now - lastUpTime;

    if (time > 60000) {
      Serial.println();
      Serial.print("СБРООООООС через 10 секунд, не трогайте ложку");
      for (int i = 0; i < 10; i++){
        Serial.print(".");
        delay(1000);
      }
      tiltCount = 0;
      scale.tare();
      timerStarted = 0;
    }
  }

  //Serial.print((millis() - lastUpTime) / 1000);
  Serial.println();
  delay(100);
}
