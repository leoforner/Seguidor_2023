#ifndef PINS_H
#define PINS_H
#include <BluetoothSerial.h>
#include <Arduino.h>
#include "wheels.h"

#define FOMINHA

#ifdef FRANK 
    // analogicos
    #define left 34
    #define right 36
    #define divTensao 39
    // botoes
    #define enc1 18
    #define enc2 19
    #define enc3 21
    #define enc4 22
    #define start 23
    // motor 1
    #define pwma 5
    #define ain2 15
    #define ain1 2
    // motor 2
    #define pwmb 17
    #define bin1 4
    #define bin2 16 
    // sensores frontais
    #define IR 35
    // led
    #define led 2
#endif

#ifdef FOMINHA
    // analogicos
    #define divTensao 13

    // botoes
    #define enc1 23
    #define enc2 22
    #define enc3 5
    #define enc4 18

    // motor 1
    #define pwma 15
    #define ain2 2
    #define ain1 0

    // motor 2
    #define pwmb 17
    #define bin1 16
    #define bin2 4 
    #define led 21 
#endif
    // sensor frontal
    extern uint8_t pinos[], pinCount;

    // canais pwm
    extern uint8_t channelLeft, channelRight;

    // rodas
    extern wheels wheelLeft, wheelRight;

    // serial bt
    extern BluetoothSerial SerialBT;

void definePins(){
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);
    pinMode(enc1, INPUT);
    pinMode(enc2, INPUT);
    pinMode(enc3, INPUT);
    pinMode(enc4, INPUT);
    pinMode(divTensao, INPUT);
    pinMode(ain1, OUTPUT);
    pinMode(ain2, OUTPUT);
    pinMode(bin1, OUTPUT);
    pinMode(bin2, OUTPUT);
    pinMode(pwma, OUTPUT);
    pinMode(pwmb, OUTPUT);

    // right
    pinMode(pwma, OUTPUT);
    ledcSetup(channelRight, 5000, 12); // canal para esquerdo
    ledcAttachPin(pwma, channelRight);

    // left
    pinMode(pwmb, OUTPUT);
    ledcSetup(channelLeft, 5000, 12); // canal para esquerdo
    ledcAttachPin(pwmb, channelLeft);
}

#endif