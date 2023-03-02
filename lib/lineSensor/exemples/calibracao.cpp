#include <Arduino.h>
#include <LineSensor.h>

LineSensor ls;
uint8_t pinos[8] = {39, 34, 35, 32, 33, 25, 26, 27};

void setup() {
    Serial.begin(115200);
    ls.begin(pinos, true);
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