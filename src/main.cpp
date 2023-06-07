#include <Arduino.h>
#include <lineSensor.h>

#define IR 34
#define left 39
#define right 36

#define enc1 23
#define enc2 22
#define enc3 5
#define enc4 18
#define start 19

#define divTensao 13

uint8_t pinos[] = {35, 32, 33, 25, 26, 27, 14, 12},
        pinCount = 8;
lineSensor ls(pinCount, pinos, true);

void setup() {
  Serial.begin(115200);

  pinMode(left, INPUT);
  pinMode(right, INPUT);
  pinMode(enc1, INPUT_PULLDOWN);
  pinMode(enc2, INPUT_PULLDOWN);
  pinMode(enc3, INPUT_PULLDOWN);
  pinMode(enc4, INPUT_PULLDOWN);
  pinMode(start, INPUT_PULLDOWN);
  pinMode(divTensao, INPUT);

  ls.begin();

  //ls.setVerb(true);
  ls.calibration(DYNAMIC);
  ls.printConfig();
  delay(1000);
}

void loop() {
  double position = ls.searchLine() - 3500.0;
  //sensores
  Serial.printf("ls: %.2f\tleft: %d\tright: %d\tdivTen: %d\t", position, analogRead(left), analogRead(right), analogRead(divTensao));
  //encoders
  Serial.printf("enc1: %d\tenc2: %d\tenc3: %d\tenc4: %d\tstart: %d\n", digitalRead(enc1), digitalRead(enc2), digitalRead(enc3), digitalRead(enc4), digitalRead(start));
  delay(100);
}