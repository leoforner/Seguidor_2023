#include <Arduino.h>
#include <lineSensor.h>

// analogicos
#define left 39
#define right 36
#define divTensao 13

// botoes
#define enc1 23
#define enc2 22
#define enc3 5
#define enc4 18
#define start 19

// motor 1
#define pwma 15
#define ain2 2
#define ain1 0

// motor 2
#define pwmb 17
#define bin1 4
#define bin2 16 

// sensores frontais
#define IR 34
uint8_t pinos[] = {35, 32, 33, 25, 26, 27, 14, 12},
        pinCount = 8;

lineSensor ls(pinCount, pinos, true);

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

  pinMode(left, INPUT);
  pinMode(right, INPUT);
  pinMode(enc1, INPUT_PULLDOWN);
  pinMode(enc2, INPUT_PULLDOWN);
  pinMode(enc3, INPUT_PULLDOWN);
  pinMode(enc4, INPUT_PULLDOWN);
  pinMode(start, INPUT_PULLDOWN);
  pinMode(divTensao, INPUT);
  pinMode(ain1, OUTPUT);
  pinMode(ain2, OUTPUT);
  pinMode(bin1, OUTPUT);
  pinMode(bin2, OUTPUT);
  pinMode(pwma, OUTPUT);
  pinMode(pwmb, OUTPUT);

  // biblioteca dos sensores
  ls.begin();
  //ls.setVerb(true);
  ls.calibration(DYNAMIC);
  ls.printConfig();

  // ponte h config 
  pinMode(pwma, OUTPUT);
  pinMode(pwmb, OUTPUT);
  ledcSetup(0, 5000, 12); // canal para esquerdo
  ledcSetup(1, 5000, 12); // canal para o direito
  ledcAttachPin(pwma, 1);
  ledcAttachPin(pwmb, 0);

  delay(1000);
}

uint16_t contador = 0;
bool vira = true;
void loop() {
  // calcula onde esta a linha
  double position = ls.searchLine() - 3500.0;

  float bat = (analogicoParaTensao(analogRead(divTensao)))*7.08/1.78;

  int t6 = (6.0*4095)/bat;

  if(t6 > 4095) t6 = 4095;

  ledcWrite(0, t6);
  ledcWrite(1, t6);
  
  //sensores
  Serial.printf("ls: %.2f\tleft: %d\tright: %d\tdivTen: %.2f\t", position, analogRead(left), analogRead(right), bat);
  
  //encoders
  Serial.printf("enc1: %d\tenc2: %d\tenc3: %d\tenc4: %d\tstart: %d\t", digitalRead(enc1), digitalRead(enc2), digitalRead(enc3), digitalRead(enc4), digitalRead(start));
  
  digitalWrite(ain1, !vira);
  digitalWrite(ain2, vira);
  digitalWrite(bin1, !vira);
  digitalWrite(bin2, vira);

  Serial.printf("lado: %s\tpwm: %d\n", vira ? "frente" : "contrario", t6);
  delay(100);
}