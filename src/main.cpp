// bibliotecas
#include <BluetoothSerial.h>
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

// serial bluetooth
BluetoothSerial SerialBT;
double position;

uint8_t state = 0;        // estados do carrinho 
// 0 desligado
// 1 calibrando
// 2 e 3 pista
// 4 final da pista

// sensores frontais (defina o carrinho em pins.h)
#ifdef FRANK
    uint8_t pins[] = {13, 12, 14, 27, 26, 25, 33, 32}, pinCount = 8;
    // {32, 33, 25, 26, 27, 14, 12, 13}
    // {13, 12, 14, 27, 26, 25, 33, 32}
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
double wheelsRadius = 1.5, actingTime = 0.8;
double* wheelsSetPoint = new double[2];
mathModel carModel(carVector, wheelsRadius, actingTime, wheelsSetPoint);                 

// controle do carrinho
PD control(1, 1, 1, 1);

// contadores do encoder
EncoderCounter encoderLeft(enc3, PCNT_UNIT_0, 140, 1000);
EncoderCounter encoderRight(enc1, PCNT_UNIT_1, 140, 1000);

// rodas
wheels wheelLeft, wheelRiht;

// canais pwm (precisa definir para definir em pins.h)
uint8_t channelLeft = 0, channelRight = 1;

void setup(){
    Serial.begin(115200);
    definePins();
    SerialBT.begin("FRANK");

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

    // aplica o pwm nos motores
    applyPWM(&wheelLeft, 0);
    applyPWM(&wheelRiht, 0);

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
    while(state < 2) delay(10);

    // sinalização piscando led
    delay(100);
    for(uint8_t i = 0; i < 4; i++){
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(500);
    }
}

void loop(){
    // calcula a posição da linha
    position = (forwardSensor.searchLine(&state)/100.0) - (5.70/2.0);

    // calcula o setPoint de cada roda em cm/s
    carModel.calculateSetPoints(position);

    // velocidade dos motores
    getVelocity(&wheelLeft);
    getVelocity(&wheelRiht);

    // pi para as rodas
    int32_t pwmLeft = control.leftPI(50, 10, wheelsSetPoint[0] - wheelLeft.velocity, 4095);
    int32_t pwmRight = control.rightPI(50, 10, wheelsSetPoint[1] - wheelRiht.velocity, 4095);

    // calcula pwm max (correspondente a 6v)
    float tensaoBateria = (analogicoParaTensao(analogRead(divTensao)))*7.6/1.92; //7.6v viram 1.92v (divisor de tensão)
    int pwm_6volts = (6.0*4095)/tensaoBateria;
    if(pwm_6volts > 4095) pwm_6volts = 4095;

    // monitor serial
    //Serial.printf("position: %.3f\twheelsSetPoints: %.3f - %.3f\tvel rigth: %.3f\t vel left: %.3f\tpwms: %d - %d\n", 
    //                    position, wheelsSetPoint[1], wheelsSetPoint[0], wheelRiht.velocity, wheelLeft.velocity, pwmRight, pwmLeft);

    // aplica o pwm nos motores
    applyPWM(&wheelLeft, pwmLeft);
    applyPWM(&wheelRiht, pwmRight);

    // plotagem
    Serial.printf("%.2f, %.2f, %.2f, %.2f\n", position, wheelsSetPoint[1], wheelLeft.velocity,  wheelRiht.velocity);

    delay(1);
}