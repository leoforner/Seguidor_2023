#include <Arduino.h>
#include <EnconderCounter.h>

EnconderCounter encoder(23, PCNT_UNIT_0, 100);

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
    Serial.println(encoder.getPulses());
    delay(100);
}