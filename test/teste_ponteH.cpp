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

    // sinalização piscando led
    delay(100);
    for(uint8_t i = 0; i < 2; i++){
    digitalWrite(led, HIGH);

    delay(100);
    digitalWrite(led, LOW);
    delay(800);
    }
}

void loop(){
    uint16_t pot = analogRead(IR);

    // Mapeia o valor do potenciômetro para o intervalo desejado
    int16_t mappedValue = map(pot, 0, 4095, -4095, 4095);

    float sentido = 1.0;

    // muda o sentido da rotação
    if(mappedValue > 0){
        if(wheelLeft.mov != FORWARD)    changeDirection(&wheelLeft, FORWARD);
        if(wheelRiht.mov != FORWARD)    changeDirection(&wheelRiht, FORWARD);
    }else if(mappedValue < 0){
        if(wheelLeft.mov != BACKWARD)   changeDirection(&wheelLeft, BACKWARD);
        if(wheelRiht.mov != BACKWARD)   changeDirection(&wheelRiht, BACKWARD);
        mappedValue *= -1;
        sentido = -1.0;
    }else{
        if(wheelLeft.mov != STOPPED)    changeDirection(&wheelLeft, STOPPED);
        if(wheelRiht.mov != STOPPED)    changeDirection(&wheelRiht, STOPPED);
    }

    Serial.printf("%.3f\t%.3f\n", sentido * wheelLeft.enc->getRPS(), sentido * wheelRiht.enc->getRPS());

    // aplica o pwm
    ledcWrite(wheelRiht.channelPWM, mappedValue);
    ledcWrite(wheelLeft.channelPWM, mappedValue);

    delay(50);
}