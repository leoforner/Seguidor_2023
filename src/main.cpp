// bibliotecas
#include <EncoderCounter.h>
#include <lineSensor.h>
#include <mathModel.h>
#include <Arduino.h>
#include <PD.h>

// includes
#include "adc.h"
#include "pins.h"
#include "times.h"
#include "wheels.h"
#include "exceptions.h"

uint8_t state = 0;        // estados do carrinho 
// 0 desligado
// 1 calibrando
// 2 e 3 pista
// 4 final da pista

// sensores frontais (defina o carrinho em pins.h)
#ifdef FRANK
    uint8_t pins[] = {32, 33, 25, 26, 27, 14, 12, 13}, pinCount = 8;
#endif

#ifdef FOMINHA
    uint8_t pins[] = {35, 32, 33, 25, 26, 27, 14, 12}, pinCount = 8;
#endif

lineSensor forwardSensor(pinCount, pins, true);

// modelo matematico do carrinho
                             // X    Y
double carVector[3][2] =   {{+7.5, -2.0},    // roda direita
                            {-7.5, -2.0},    // roda esquerda
                            {+0.0, 11.0}};   // linha de sensores
double wheelsRadius = 1.5, actingTime = 1;
double* wheelsSetPoint = new double[2];
mathModel carModel(carVector, wheelsRadius, actingTime, wheelsSetPoint);                 

// controle do carrinho
PD control(1, 1, 1, 1);

// contadores do encoder
EncoderCounter encoderLeft(enc3, PCNT_UNIT_0, 140, 1000);
EncoderCounter encoderRight(enc1, PCNT_UNIT_1, 140, 1000);

// rodas
wheels wheelLeft;
wheels wheelRiht;

// canais pwm (precisa definir para definir em pins.h)
uint8_t channelLeft = 0;
uint8_t channelRight = 1;

void setup(){
    Serial.begin(115200);
    definePins();

    // filtro passa baixa 
    encoderLeft.setFiltroCostant(0.01);
    encoderRight.setFiltroCostant(0.01);

    // define as propriedades das rodas
    wheelLeft.enc = &encoderLeft;
    wheelLeft.mov = STOPPED;
    wheelLeft.velocity = 0.0;
    wheelLeft.l1 = bin1;
    wheelLeft.l2 = bin2;
    wheelLeft.channelPWM = channelLeft;

    wheelRiht.enc = &encoderRight;
    wheelRiht.mov = STOPPED;
    wheelRiht.velocity = 0.0;
    wheelRiht.l1 = ain1;
    wheelRiht.l2 = ain2;
    wheelRiht.channelPWM = channelRight;

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

    /*Serial.println("Calibrando...");
    forwardSensor.begin();
    forwardSensor.setLed(led);

    // a linha tem 5.7 centimetros com 8 sensores
    // altera o peso padrao para calcular a distancia da linha
    // dessa forma a saida esta em nanometro
    // divindo por 100 a medida passa para centimetros
    float pesos[8];
    for(int i = 0; i < 8; i++) pesos[i] = i * 5.7/7; 
    forwardSensor.setweights(pesos);
    forwardSensor.setTrackCharacteristics(100, 0, 30);
    
    forwardSensor.calibration(DYNAMIC);
    forwardSensor.printConfig();

    Serial.println("Sensor calibrado, pressione o botao para iniciar trajeto");
    while(state < 2) delay(10);*/

    // sinalização piscando led
    delay(100);
    for(uint8_t i = 0; i < 4; i++){
    digitalWrite(led, HIGH);

    delay(100);
    digitalWrite(led, LOW);
    delay(800);
    }
}

void loop(){
    double distanciaLinha = (forwardSensor.searchLine(&state)/100.0) - 5.70/2.0;

    // calcula o setPoint de cada roda em cm/s
    carModel.calculateSetPoints(distanciaLinha);

    // velocidade dos motores
    double velRight = encoderRight.getRPS();
    double velLeft = encoderLeft.getRPS();

    float erro1 = wheelsSetPoint[0] - velRight;
    float erro2 = wheelsSetPoint[1] - velLeft;

    // pid para a roda
    int32_t pwmSaida1 = control.rightPI(50, 1, erro1, 4095);
    int32_t pwmSaida2 = control.leftPI(50, 1, erro2, 4095);

    // calcula pwm max (correspondente a 6v)
    float tensaoBateria = (analogicoParaTensao(analogRead(divTensao)))*7.6/1.92; //7.6v viram 1.92v (divisor de tensão)
    int pwm_6volts = (6.0*4095)/tensaoBateria;
    if(pwm_6volts > 4095) pwm_6volts = 4095;

    // monitor serial
    Serial.printf("Left: %.4f\tRight: %.4f\tsetpoints: %.4f - %.4f\terros: %.4f - %.4f\tpwms: %d - %d\n", 
                    velLeft, velRight, wheelsSetPoint[0], wheelsSetPoint[1], erro2, erro1, pwmSaida2, pwmSaida1);
    // plotagem
    //Serial.printf("%.2f, %.2f, %.2f, %.2f\n", wheelsSetPoint[0], wheelsSetPoint[1], velLeft, velRight);
    
    delay(50);
}