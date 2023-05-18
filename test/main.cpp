#define BOTAO 5
#include "EnconderCounter.h"

EnconderCounter encoder(23, PCNT_UNIT_0, 140, 1000);
void setup() {
    Serial.begin(115200);
    pinMode(5, INPUT);
    //Serial.println(encoder.getRPS());
}
//unsigned long pastTime = 0;
void loop() {
  double velocity = encoder.getRPS();
  if(digitalRead(BOTAO) == HIGH)
  {
 Serial.println(velocity);
  }
  
 

 
  //system("cls");
  //pastTime = micros();
  delay(10);
}