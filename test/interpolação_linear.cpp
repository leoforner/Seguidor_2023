#include <Arduino.h>

#define out 12
#define in 4
#define pot 35

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

void setup() {
    Serial.begin(115200);
    pinMode(out, OUTPUT);
    pinMode(pot, OUTPUT);
    pinMode(in, INPUT);

    ledcSetup(0, 5000, 12); 
    ledcAttachPin(out, 0);
}

void loop() {
    // le o potenciometro
    int potenciometro = analogRead(pot);
    // converte o potenciometro para tensao
    float leitura = (potenciometro*3.28)/4095;
    // calcula qual o sinal analogico correspondente para a entrada
    float saida = (leitura*4095)/3.28;

    ledcWrite(0, saida);

    float avg = 0;
    for (int i = 0; i < 10; i++) {
        avg += analogRead(in);
        delay(10);
    }
    avg /= 10.0;

    Serial.println(analogicoParaTensao(avg));
    delay(100);
}