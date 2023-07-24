#ifndef EXPCEPTIONS_H
#define EXPCEPTIONS_H
#include "time.h"
#include "pins.h"
#include "wheels.h"
#include <Arduino.h>

extern uint8_t state;
extern double position;
extern int32_t pid;
extern double kp, ki, kd;
extern uint32_t speed;
uint32_t timeFilter = 100;

static void IRAM_ATTR change_state(){
    if((millis() - timeFilter) > 200){
        if(state < 4) { // avança o estado
            state++; 
        }else{         // reseta a esp
            Serial.println("Esp reset - state 4");
            applyPWM(&wheelLeft, 0);
            applyPWM(&wheelRight, 0);
            ESP.restart();    
        }
        timeFilter = millis();
    }
}

void recebeDados(String * texto){
    // recebe as chars e soma em um texto
    char a = SerialBT.read();
    *texto += a;

    // separa as constantes quando recebe o texto todo
    if(a == '}'){
        // Use sscanf para ler os valores da string formatada
        //Serial.println(*texto);
        int count = sscanf((*texto).c_str(), "{%lf,%lf,%lf,%d}", &kp, &ki, &kd, &speed);

        // Verifica se todos os quatro valores foram lidos corretamente
        /*if (count != 4) {
            // Algo deu errado na leitura da string
            SerialBT.println("Erro ao ler os valores da string.");
        }else{
            SerialBT.printf("Constantes alteradas.\nkp: %.3f\nki: %.3f\nkd: %.3f\nspeed: %d\n", kp, ki, kd, speed);
        }*/
    }

    // reseta a variavel
    if(a == '\r' || a == '\n'){
        *texto = "";
    }
}

void IRAM_ATTR interrupt(void * param){
    attachInterrupt(digitalPinToInterrupt(start), change_state, HIGH);
    static String texto = "";
    while(1){        
        // caso ele receba algum dado ele altera as constantes 
        if(SerialBT.available()){
            recebeDados(&texto);
        }else{
            // senao ele envia os dados para montar o grafico
            delay(10);
            // caso queirm mudar oq ele envia mude aqui
            //SerialBT.printf("{%d}\n", pid);
        } 
        delay(10);
    }
    //vTaskDelete(NULL);
}


#endif