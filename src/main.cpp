#include <Arduino.h>
#include <EnconderCounter.h>
EnconderCounter encoder(23, PCNT_UNIT_0, 140, 1000);

// divisor tensao 
#define in 36
// bateria 
#define pwmb 5
#define pwma 18

double medidas[300]; 


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
    delay(3000);
    Serial.begin(115200);
    pinMode(pwmb, OUTPUT);
    pinMode(pwma, OUTPUT);
    pinMode(in, INPUT);

    ledcSetup(0, 5000, 12);
    ledcAttachPin(pwmb, 0); 
    ledcSetup(1, 5000, 12);
    ledcAttachPin(pwma, 1);  
}

bool dale = false;
void loop() {
    // aplicar pwm duarante tempo determinado
    for(int i = 0; i < 200; i++){
      // le a tensao da bateria
        float x = analogRead(in);
        x = (analogicoParaTensao(x))*4.03;
        float saida = (4.0*4095)/x;

        // apllica o pwm correspondente
        ledcWrite(0, saida);
        ledcWrite(1, saida);

        // salva a velocidade  
        medidas[i] = encoder.getRPS();
        delay(10);
        Serial.println(x);
    }
    dale = true;
    ledcWrite(0, 0);
    ledcWrite(1, 0);

    // printa o resultado
    while(dale){
      Serial.println("-");
        for(int i = 0; i < 200; i++){
            Serial.printf("%.3f;\n", medidas[i]);
        }
        delay(2000);
    }
}