#include <Arduino.h>

#include "EnconderCounter.h"


// pinos da ponte H
#define PWMA 19
#define PWMB 21
#define esq1 15
#define esq2 4
#define dir1 5
#define dir2 17

// variaveis para execoes
#define intercessao 4000
#define botao 18
#define sensoresquerdo 13
#define sensordireito 36
EnconderCounter encoder_esquerdo(39, PCNT_UNIT_0, 140);
EnconderCounter encoder_direito(13, PCNT_UNIT_1, 140);

void setup() {
  Serial.begin(115200);
  pinMode(sensordireito, INPUT);
  pinMode(sensoresquerdo, INPUT);
  pinMode(botao, INPUT);
  pinMode(esq1, OUTPUT);
  pinMode(esq2, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(2, OUTPUT);

  // config ponte H
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  ledcSetup(0, 5000, 12); // canal para esquerdo
  ledcSetup(1, 5000, 12); // canal para o direito
  ledcAttachPin(PWMA, 1);
  ledcAttachPin(PWMB, 0);
}


void loop() {
//ledcWrite(0, 4095);
ledcWrite(1, 4095);
}