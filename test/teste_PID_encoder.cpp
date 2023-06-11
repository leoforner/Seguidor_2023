#include "EnconderCounter.h"
#include <PID.h>

PID pid;

#define pwmb 5
#define bin1 21
#define bin2 19

EnconderCounter encoder(23, PCNT_UNIT_0, 140, 1000);
void setup() {
    Serial.begin(115200);
    pid.begin();
    //pid.setVerb(true);
    //Serial.println(encoder.getRPS());
    pinMode(pwmb, OUTPUT);
    pinMode(bin1, OUTPUT);
    pinMode(bin2, OUTPUT);

    ledcSetup(0, 5000, 12);
    ledcAttachPin(pwmb, 0);

}

void loop() {
  double vel = encoder.getRPS();
  double erro = (10.0) - vel;

  uint32_t pwmsaida = pid.simplePI(300, 300, erro, 4095);

  if(pwmsaida > 0){
    digitalWrite(bin2, HIGH);
    digitalWrite(bin1, LOW);
  }else{
    digitalWrite(bin2, LOW);
    digitalWrite(bin1, HIGH);
    pwmsaida *= -1;
  }

  ledcWrite(0, pwmsaida);

  Serial.printf("Erro: %.3f\tvel: %.3f\tpwm: %d\n", erro, vel, pwmsaida);
  
  delay(100);
}