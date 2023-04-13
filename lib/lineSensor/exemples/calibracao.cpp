#include <Arduino.h>
#include <lineSensor.h>

lineSensor ls;
uint8_t pinos[6] = {34, 35, 32, 33, 25, 26},
        pinCount = 6;

void setup() {
    Serial.begin(115200);
    ls.begin(pinCount, pinos, true);
    //ls.setVerb(true);
    ls.calibration(ESTATICO);
    ls.printConfig();
}

void loop() {
    uint32_t start = millis();
    Serial.printf("Medida = %d \t Tempo: %dms \n", ls.searchLine(), millis() - start);
    //ls.searchLine();
    delay(100);
}