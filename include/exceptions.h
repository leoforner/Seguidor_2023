#ifndef EXPCEPTIONS_H
#define EXPCEPTIONS_H
#include "time.h"
#include "pins.h"
#include <Arduino.h>

extern uint8_t state;
uint32_t timeFilter = 100;

void IRAM_ATTR change_state(){
    if((millis() - timeFilter) > 200){
        if(state < 4) { // avança o estado
            state++; 
            Serial.println("next"); 
        }else{         // reseta a esp
            Serial.println("Esp reset - state 4");
            ESP.restart();    
        }
        timeFilter = millis();
    }
}

void IRAM_ATTR interrupt(void * param){
    attachInterrupt(digitalPinToInterrupt(start), change_state, HIGH);
    //attachInterrupt(digitalPinToInterrupt(right), change_state, LOW);
    //attachInterrupt(digitalPinToInterrupt(left), change_state, LOW);

    // pino 36 e 39 nao suportam interrupt
    // ficamos verificando os senores laterais sempre
    while(1){
    if(!analogRead(right) && state > 1){
        // delay de 100 millis para esperar o loop principal subtrair 1,
        // senao somamos antes do loop considerar intersec.
        // podemos mudar esses 100 millis para menos, porem depende do tempo total de cada loop
        while(!analogRead(right)) delay(100);
        change_state();
    }
    delay(5);
    }
    //vTaskDelete(NULL);
}

#endif