#include <Arduino.h>
#include <lineSensor.h>

uint8_t pinos[6] = {12, 14, 27, 26, 25, 33}, pinCount = 6;
lineSensor ls(pinCount, pinos, true);

void setup() {
    Serial.begin(115200);

    // gambiarra para funcionar com a esp stand alone
    Serial.println("espera");
    delay(3000);

    ls.setLed(2);
    ls.begin();
    ls.setVerb(true);
    ls.calibration(STATIC);
    ls.printConfig();
}

uint8_t state = 0; // define intersecção
void loop() {
    //uint32_t start = millis();
    //ls.searchLine();
    //Serial.printf("Medida = %d \t Tempo: %dms \n", ls.searchLine(), millis() - start);
    ls.searchLine(&state);
    delay(100);
}