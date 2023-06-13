#include "EnconderCounter.h"
#include <PID.h>

PID pid1;
PID pid2;

#define divTensao 36

#define PWMA 19
#define PWMB 21
#define esq1 15
#define esq2 4
#define dir1 5
#define dir2 2

EnconderCounter * encoder1;
EnconderCounter * encoder2;

float interpolacaoLinear(float analog, float vMin, float vMax,
                float adc_min, float adc_max) {
  return vMin +
         (analog - adc_min) * (vMax - vMin) / (adc_max - adc_min);
}

float analogicoParaTensao(float analog) {
  if (analog < 22)
    return interpolacaoLinear(analog, 0.0, 0.17, 0, 22);
  else if (analog >= 22 && analog < 1832)
    return interpolacaoLinear(analog, 0.17, 1.66, 22, 1832);
  else if (analog >= 1832 && analog < 3138)
    return interpolacaoLinear(analog, 1.66, 2.69, 1832, 3138);
  else if (analog >= 3138 && analog < 4095)
    return interpolacaoLinear(analog, 2.69, 3.12, 3138, 4095);
  else
    return 3.2;
}

void beginEnc(void * xTaskParameters){
    encoder1 = new EnconderCounter(17, PCNT_UNIT_0, 140, 1000);
    encoder2 = new EnconderCounter(16, PCNT_UNIT_1, 140, 1000);
    
    vTaskDelete(NULL);
}

void setup() {
    Serial.begin(115200);

    pinMode(esq1, OUTPUT);
    pinMode(esq2, OUTPUT);
    pinMode(dir1, OUTPUT);
    pinMode(dir2, OUTPUT);

    pinMode(divTensao, INPUT);

    pinMode(PWMA, OUTPUT);
    pinMode(PWMB, OUTPUT);
    ledcSetup(0, 5000, 12); // canal para esquerdo
    ledcSetup(1, 5000, 12); // canal para o direito
    ledcAttachPin(PWMA, 1);
    ledcAttachPin(PWMB, 0);

    xTaskCreate(beginEnc, "qualquercoisaai", 10000, NULL, 1, NULL);
    delay(1000);
}

void loop() {
    float tensaoBat = (analogicoParaTensao(analogRead(divTensao)))*7.08/1.78;
    int vMax_6v = (6.0*4095)/tensaoBat;
    if(vMax_6v > 4095) vMax_6v = 4095;

    double vel1 = encoder1->getRPS();
    double erro1 = (10.0) - vel1;
    uint32_t pwmsaida1 = pid1.simplePI(150, 300, erro1, 4095);

    double vel2 = encoder2->getRPS();
    double erro2 = (10.0) - vel2;
    uint32_t pwmsaida2 = pid2.simplePI(150, 300, erro2, 4095);

    digitalWrite(dir1, HIGH);
    digitalWrite(dir2, LOW);
    digitalWrite(esq1, HIGH);
    digitalWrite(esq2, LOW);

    pwmsaida1 = map(pwmsaida1, 0, 4096, 0, vMax_6v);
    pwmsaida2 = map(pwmsaida2, 0, 4096, 0, vMax_6v);

    ledcWrite(0, pwmsaida1);
    ledcWrite(1, pwmsaida2);

    Serial.printf("Erro: %.3f\tvel: %.3f\tpwm: %d\t--\t", erro1, vel1, pwmsaida1);
    Serial.printf("Erro: %.3f\tvel: %.3f\tpwm: %d\t", erro2, vel2, pwmsaida2);
    Serial.printf("bateria: %.2f\n", tensaoBat);    
    delay(100);
}