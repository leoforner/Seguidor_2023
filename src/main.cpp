#include <Arduino.h>
#include <lineSensor.h>

uint8_t pinos[6] = {12, 14, 27, 26, 25, 33}, pinCount = 6;
lineSensor ls(pinCount, pinos, true);

void setup() {
    Serial.begin(115200);

    Serial.println("espera");
    delay(3000);

    ls.begin();
    ls.setLed(2);
    ls.setVerb(true);
    ls.calibration(STATIC);
    ls.printConfig();
}

void loop() {
    //uint32_t start = millis();
    ls.searchLine();
    //Serial.printf("Medida = %d \t Tempo: %dms \n", ls.searchLine(), millis() - start);
    //ls.searchLine();
    delay(100);
}