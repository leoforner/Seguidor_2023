#include <Arduino.h>
#define pwmb 5

void setup() {
    Serial.begin(115200);
    ledcSetup(0, 5000, 12);
    ledcAttachPin(pwmb, 0);
}

void loop() {
    Serial.println("netto?");
    ledcWrite(0, 4095);

    //tomatomatoma 
}