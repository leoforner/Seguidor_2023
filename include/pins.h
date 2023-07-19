#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

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
#define ain2 2
#define ain1 15
// motor 2
#define pwmb 17
#define bin1 16
#define bin2 4 
// sensores frontais
#define IR 34
extern uint8_t pinos[], pinCount;

// led
#define led 2

void definePins(){
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW); // desliga o led
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

   /*
   #ifdef IR
        pinMode(IR, OUTPUT);
    #endif
   */
}

#endif