#include "EnconderCounter.h"
EnconderCounter encoder(23, PCNT_UNIT_0, 100);
void setup() {
    Serial.begin(115200);
    //Serial.println(encoder.getRPS());
}
unsigned long pastTime = 0;
void loop() {
  encoder.getRPS(pastTime);
  pastTime = micros();
  delay(1000);
  //Serial.print("oi");
}