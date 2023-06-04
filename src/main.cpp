#include <Arduino.h>
#include <lineSensor.h>

uint8_t sensores[] = {34, 35, 32, 33, 25, 26, 27, 14},
        qtdSensores = 8;
lineSensor ls(qtdSensores, sensores, true);

void setup() {
  Serial.begin(115200);
    ls.setLed(2);
    ls.begin();

    for(uint8_t i = 0; i < 8; i++){
      pinMode(sensores[i], INPUT);
    }

    //ls.setVerb(true);
    //ls.calibration(STATIC);
    //ls.printConfig();
}

void loop() {
    //uint32_t start = millis();
    //ls.searchLine();
    //Serial.printf("Medida = %d \t Tempo: %dms \n", ls.searchLine(), millis() - start);
    //ls.searchLine();
    for(uint8_t i = 0; i < 8; i++){
      Serial.printf("%d\t", analogRead(sensores[i]));
      delay(5);
    }
    Serial.println();
    delay(100);
}