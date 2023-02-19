#include <Arduino.h>
#include <LineSensor.h>

LineSensor ls;
uint8_t pinos[8] = {39, 34, 35, 32, 33, 25, 26, 27};

void setup() {
    Serial.begin(115200);
    ls.begin((uint8_t) 8, pinos, true);

    ls.calibration(0);
}

void loop() {
    Serial.println(ls.searchLine());
    delay(100);
}