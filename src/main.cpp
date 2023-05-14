#include <Arduino.h>
#include <EnconderCounter.h>

EnconderCounter encoder(23, PCNT_UNIT_0, 100);

void setup() {
    Serial.begin(115200);
}

void loop() {
    Serial.println(encoder.getPulses());
    delay(100);
}