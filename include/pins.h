#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

#define FRANK

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
    #define led 21 // cuidado pino IR 
#endif

    extern uint8_t pinos[], pinCount;
    extern uint8_t channelLeft, channelRight;

void definePins(){
    pinMode(led, OUTPUT);
    pinMode(left, INPUT);
    pinMode(right, INPUT);
    pinMode(enc1, INPUT);
    pinMode(enc2, INPUT);
    pinMode(enc3, INPUT);
    pinMode(enc4, INPUT);
    pinMode(start, INPUT);
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
   
   #ifdef IR
        pinMode(IR, INPUT);
    #endif
}

#endif