#include <Arduino.h>
#include <lineSensor.h>

uint8_t pinos[6] = {34, 35, 32, 33, 25, 26}, pinCount = 6;
lineSensor ls(pinCount, pinos, true);

void setup() {
    Serial.begin(115200);
    ls.begin();
    //ls.setVerb(true);
    ls.calibration(STATIC);
    ls.printConfig();
}

void loop() {
    uint32_t start = millis();
    Serial.printf("Medida = %d \t Tempo: %dms \n", ls.searchLine(), millis() - start);
    //ls.searchLine();
    delay(100);
}