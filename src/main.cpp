<<<<<<< HEAD
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
=======
// bibliotecas
#include <PD.h>
#include <EnconderCounter.h>

// includes
#include "pins.h"
#include "adc.h"

PD controle(1, 1, 1, 1);

EnconderCounter encoderLeft(enc3, PCNT_UNIT_0, 140, 1000);
EnconderCounter encoderRight(enc1, PCNT_UNIT_1, 140, 1000);

const int potPin = 35;  // Pino analógico conectado ao potenciômetro
float setPoint = 0;

void IRAM_ATTR PIDJOHNSON(void * parans){
    while(1){
        // calcula valocidade das rodas
        double velRight = encoderRight.getRPS();
        double velLeft = encoderLeft.getRPS();

        /*for(uint8_t i = 0; i < 100; i++){
            velRight += encoderRight.getRPS();
            velLeft += encoderLeft.getRPS();
            delay(1);
        }            
        velRight /= 100;
        velLeft /= 100;*/

        float erro1 = setPoint - velRight;
        float erro2 = setPoint - velLeft;

        // pid para a roda
        uint32_t pwmSaida1 = controle.simplePI1(0, 100, erro1, 4095);
        uint32_t pwmSaida2 = controle.simplePI2(0, 100, erro2, 4095);

        // calcula pwm max (correspondente a 6v)
        float tensaoBateria = (analogicoParaTensao(analogRead(divTensao)))*7.6/1.92; //7.6v viram 1.92v (divisor de tensão)
        int pwm_6volts = (6.0*4095)/tensaoBateria;
        if(pwm_6volts > 4095) pwm_6volts = 4095;

        // saida
        ledcWrite(1, pwmSaida1);
        ledcWrite(0, pwmSaida2);

        // monitor serial
        //Serial.printf("setPoint: %.2f\tpwma: %d\tVelRight: %.2f\t", setPoint, pwmSaida1, velRight);
        //Serial.printf("pwmb: %d\tvelLeft: %.2f\n", pwmSaida2, velLeft);

        // plotagem
        Serial.printf("%.2f, %.2f, %.2f\n", setPoint, velLeft, velRight);

        delay(20);
    }
}

void setup(){
    Serial.begin(115200);
    definePins();
    controle.printConfig();

    encoderLeft.setFiltroCostant(0.9);
    encoderRight.setFiltroCostant(0.9);

    pinMode(potPin, INPUT);

    // right
    pinMode(pwma, OUTPUT);
    ledcSetup(1, 5000, 12); // canal para esquerdo
    ledcAttachPin(pwma, 1);
    digitalWrite(ain1, LOW);
    digitalWrite(ain2, HIGH);

    // left
    pinMode(pwmb, OUTPUT);
    ledcSetup(0, 5000, 12); // canal para esquerdo
    ledcAttachPin(pwmb, 0);
    digitalWrite(bin1, LOW);
    digitalWrite(bin2, HIGH);

    xTaskCreatePinnedToCore(
        PIDJOHNSON,
        "pid motores",
        2048,
        NULL,
        2,
        NULL,
        0
    );
}

void loop(){
    // calcula setPoint
    int potValue = analogRead(potPin);
    float local = 0;
    for(uint8_t i = 0; i < 100; i++){
        local += (map(potValue, 0, 4095, 0, 1000))/100.0;
        delay(1);
    } setPoint = local/100;
>>>>>>> AlisonTristao
}