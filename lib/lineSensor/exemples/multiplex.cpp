#include <Arduino.h>
#include <lineSensor.h>

lineSensor ls;
uint8_t pinsOut[3] = {5, 18, 19}, 
        pinMult = 3, 
        pinCount = 8, // assim tera até 8 sensores
        pinIn = 15;
/*
    Caso vá usar menos sensores do que o multiplex permite 
    apenas troque pinCount para a quantidade que vai usar 
    e entao o multiplex irá ler somente até o index da qtd
    forncecida.

    ex: se deixar com 6 sensores o multiplex irá somente até ]
    o index 5
*/

void setup() {
    Serial.begin(115200);
    ls.beginMultiplex(pinCount, pinMult, pinsOut, pinIn, true);
    //ls.setVerb(true);
    ls.calibration(STATIC);
    ls.printConfig();
}

void loop() {
    uint32_t start = millis();
    Serial.printf("Medida = %d \t Tempo: %dms \n", ls.searchLine(), millis() - start);
    //ls.searchLine();
    delay(250);
}