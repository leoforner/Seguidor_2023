#include <Arduino.h>
#include <LineSensor.h>

LineSensor ls;
uint8_t pinos[3] = {5, 18, 19};

void setup() {
    Serial.begin(115200);
    ls.beginMultiplex(pinos, 15, true);
    //ls.setVerb(true);
    ls.calibration(ESTATICO);
    ls.printConfig();
}

void loop() {
    uint32_t start = millis();
    Serial.printf("Medida = %d \t Tempo: %dms \n", ls.searchLine(), millis() - start);
    //ls.searchLine();
}