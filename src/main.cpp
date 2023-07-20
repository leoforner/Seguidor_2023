// bibliotecas
#include <Arduino.h>
#include <lineSensor.h>
#include <EncoderCounter.h>
#include <mathModel.h>
#include <PD.h>

// includes
#include "pins.h"
#include "adc.h"
#include "times.h"
#include "exceptions.h"

uint8_t state = 0;        // estados do carrinho 
// 0 desligado
// 1 calibrando
// 2 e 3 pista
// 4 final da pista

// sensores frontais
uint8_t pinos[] = {32, 33, 25, 26, 27, 14, 12, 13}, pinCount = 8;
lineSensor sensorFrontal(pinCount, pinos, true);

// modelo matematico do carrinho
                             // X    Y
double carVector[3][2] =   {{+7.5, -2.0},    // roda direita
                            {-7.5, -2.0},    // roda esquerda
                            {+0.0, 11.0}};   // linha de sensores
double wheelsRadius = 1.5, actingTime = 1;
double* wheelsSetPoint = new double[2];
mathModel modeloMatematico(carVector, wheelsRadius, actingTime, wheelsSetPoint);                 

// controle do carrinho
PD controle(1, 1, 1, 1);

// contadores do encoder
EncoderCounter encoderLeft(enc3, PCNT_UNIT_0, 140, 1000);
EncoderCounter encoderRight(enc1, PCNT_UNIT_1, 140, 1000);

void setup(){
    Serial.begin(115200);
    definePins();

    // filtro passa baixa 
    encoderLeft.setFiltroCostant(0.01);
    encoderRight.setFiltroCostant(0.01);

    // anexa as interrupcoes ao segundo nucleo
    xTaskCreatePinnedToCore(
        interrupt,
        "interrupt",
        2048,
        NULL,
        2,
        NULL,
        0
    );

    Serial.println("Carrinho ligado, pressione o botao para iniciar calibração");
    while(state < 1) delay(10);

    Serial.println("Calibrando...");
    sensorFrontal.begin();
    sensorFrontal.setLed(led);

    // a linha tem 5.7 centimetros com 8 sensores
    // altera o peso padrao para calcular a distancia da linha
    // dessa forma a saida esta em nanometro
    // divindo por 100 a medida passa para centimetros
    float pesos[8];
    for(int i = 0; i < 8; i++) pesos[i] = i * 5.7/7; 
    sensorFrontal.setweights(pesos);
    sensorFrontal.setTrackCharacteristics(100, 0, 30);
    
    sensorFrontal.calibration(STATIC);
    sensorFrontal.printConfig();

    Serial.println("Sensor calibrado, pressione o botao para iniciar trajeto");
    while(state < 2) delay(10);

    // sinalização piscando led
    delay(100);
    for(uint8_t i = 0; i < 4; i++){
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(800);
    }

    digitalWrite(ain1, HIGH);
    digitalWrite(ain2, LOW);
    digitalWrite(bin1, HIGH);
    digitalWrite(bin2, LOW);
}

void loop(){
    // distancia entre o sensor e a linha em cm
    double distanciaLinha = (sensorFrontal.searchLine(&state)/100.0) - 5.70/2.0;

    // calcula o setPoint de cada roda em cm/s
    modeloMatematico.calculateSetPoints(distanciaLinha);

    // resultados
    //Serial.printf("erro: %.2f\tr1: %.3frps\tr2: %.3frps\t", distanciaLinha, wheelsSetPoint[0], wheelsSetPoint[1]);

    // velocidade dos motores
    double velRight = encoderRight.getRPS();
    double velLeft = encoderLeft.getRPS();
    //Serial.printf("Right: %.2frps\tLeft: %.2frps\t", velRight, velLeft);

    float erro1 = 10 - velRight;//wheelsSetPoint[0] - velRight;
    float erro2 = 5 - velLeft;//wheelsSetPoint[1] - velLeft;

    // pid para a roda
    int32_t pwmSaida1 = controle.rightPI(50, 1, erro1, 4095);
    int32_t pwmSaida2 = controle.leftPI(50, 1, erro2, 4095);

    // calcula pwm max (correspondente a 6v)
    float tensaoBateria = (analogicoParaTensao(analogRead(divTensao)))*7.6/1.92; //7.6v viram 1.92v (divisor de tensão)
    int pwm_6volts = (6.0*4095)/tensaoBateria;
    if(pwm_6volts > 4095) pwm_6volts = 4095;

    Serial.println(analogRead(divTensao));

    // monitor serial
    //Serial.printf("right: %.2f\tleft: %.2f\tpwma: %d\tVelRight: %.2f\t", wheelsSetPoint[1], wheelsSetPoint[0], pwmSaida1, velRight);
    //Serial.printf("pwmb: %d\tvelLeft: %.2f\n", pwmSaida2, velLeft);


    // plotagem
    //Serial.printf("%.2f, %.2f, %.2f, %.2f\n", wheelsSetPoint[0], wheelsSetPoint[1], velLeft, velRight);

    //ledcWrite(0, pwmSaida1);
    //ledcWrite(1, pwmSaida2);

    // controle entra aqui
    delay(1);
}