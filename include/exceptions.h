#ifndef EXPCEPTIONS_H
#define EXPCEPTIONS_H
#include "time.h"
#include "pins.h"
#include "wheels.h"
#include <Arduino.h>
#include "mapeamento.h"
#ifndef MEU_ENUM_H
#define MEU_ENUM_H

enum stt {
    OFF = -1,
    CONNECT = 0,
    CALIBR1 = 1,
    CALIBR2 = 2,
    INTERSEC = 3,
    PISTA = 4,
    FINAL = 5
};

#endif

extern uint16_t whiteRight, whiteLeft;
extern stt state;
extern double position;
extern int32_t pid;
extern double kp, ki, kd;
extern uint32_t speed;
uint32_t timeFilter = 0, delayPlot = 0;
extern int sinalizacao_atual;
bool encoderStarted=false;
extern EncoderCounter encoder;
static void IRAM_ATTR change_state(){
    if((millis() - timeFilter) > 200){
        // avança o estado
        state = static_cast<stt>(static_cast<int>(state) + 1);   
        // off
        if(state == 5) {                
            applyPWM(&wheelLeft, 0);
            applyPWM(&wheelRight, 0);  
        // reseta a esp
        }else if(state > 5){            
            //Serial.println("Esp reset");
            ESP.restart();  
        }
        timeFilter = millis();
    }
}

void recebeDados(String * texto){
    // recebe as chars e soma em um texto
    char a = SerialBT.read();
    *texto += a;

    // recebeu a mensagem completa
    if(a == '}'){
        // filtra a string
        int count = sscanf((*texto).c_str(), "{%lf,%lf,%lf,%d}", &kp, &ki, &kd, &speed);

        // debug
        //SerialBT.printf("kp: %.3f\nki: %.3f\nkd: %.3f\nspeed: %d\n", kp, ki, kd, speed);
    }

    // reseta a variavel
    if(a == '\r' || a == '\n' || a == '}'){
        *texto = "";
    }
}

void IRAM_ATTR interrupt(void * param){
    attachInterrupt(digitalPinToInterrupt(start), change_state, HIGH);
    static String texto = "";
    bool MarcouBranco = false;
    while(1){ 

        //COMEÇAR A CONTAGEM DO ENCODER
        if(state == INTERSEC && encoderStarted == false)
        {
        //encoder.resumeCounting();
        encoderStarted = true;
        }

        // caso ele receba algum dado ele altera as constantes 
        if(SerialBT.available()) recebeDados(&texto);

        bool direito_on = false, esquerdo_on = false;
        // caso ele detecte marcação
        if(analogRead(right) > (whiteRight - (0.3*whiteRight)) && state > 2){
            if(state == PISTA){ 
                state = FINAL;
                vTaskDelete(NULL);  
            }
            else{
                state = PISTA;
            }
            //digitalWrite(led, !digitalRead(led));         // apaga o led
            delay(200);
            direito_on = true;
        }

        bool MarcandoBranco = false;
        
        if(!direito_on)
        {
        if(analogRead(left) > (whiteLeft - (0.2*whiteLeft)) && state >= INTERSEC){ //marcando branco
            MarcandoBranco = true;
            if(MarcandoBranco != MarcouBranco)
            {
                sinalizacao_atual++;
                digitalWrite(led, !digitalRead(led));
                MarcouBranco = true;
            }  
        }
        }
        else
        {
            MarcandoBranco = false;
            MarcouBranco = false;
        }
        
        
        // envia o erro para plotagem
        /*if(millis() - delayPlot > 100 && (state == PISTA || state == INTERSEC)){
            SerialBT.print("{" + String(pid) + "}");
            delayPlot = millis();
        }*/

        delay(1);
    }
    //vTaskDelete(NULL);
}


#endif