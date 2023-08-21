// bibliotecas
#include <Arduino.h>

// includes
#include "adc.h"
#include "pins.h"
#include "wheels.h"
#include "RincPoster.h"

const char *ssid = "MinhaRede";
const char *psw = "12345679";
const char *scriptId = "AKfycbyVfyGF-wV-MGB_alEFjItjiu34cxwPXVKAwT62WlXTDncyurrE-w4QFaX-dOSzaBS5mA";

RincPoster poster(ssid, psw);

uint32_t timeStart = 0;

double medidas[4][200];

DynamicJsonDocument json(1000);

EncoderCounter encoderLeft(enc3, PCNT_UNIT_0, 140, 1000);
EncoderCounter encoderRight(enc2, PCNT_UNIT_1, 140, 1000);

// rodas
wheels wheelLeft, wheelRight;

// canais pwm (precisa definir aqui para definir em pins.h)
uint8_t channelLeft = 0, channelRight = 1;

uint8_t counter = 0;
void updateMsg()
{
  json["sheet_name"] = "teste1";
  json["time"] = medidas[2][counter];
  json["vel_left"] = medidas[0][counter];
  json["vel_right"] = medidas[1][counter];
  json["bateria"] = medidas[3][counter];
}

void setup(){
    Serial.begin(115200);
    definePins();

    // define as propriedades das rodas
    wheelLeft.mov = STOPPED;    // inicia como parada
    wheelLeft.l1 = bin1;
    wheelLeft.l2 = bin2;
    wheelLeft.channelPWM = channelLeft;

    wheelRight.mov = STOPPED;   // inicia como parada
    wheelRight.l1 = ain1;
    wheelRight.l2 = ain2;
    wheelRight.channelPWM = channelRight;

    // manda pwm zero nos motores (quando a esp reseta estava salvando o ultimo pwm aplicado)
    applyPWM(&wheelLeft, 0);
    applyPWM(&wheelRight, 0);

    delay(3000);

    encoderLeft.limpaCounter();
    encoderRight.limpaCounter();
    timeStart = millis();
}

void loop(){
    while(counter < 80){
    // calcula pwm max (correspondente a 6v)
        float tensaoBateria = (analogicoParaTensao(analogRead(divTensao)))* 3.96; // 7.6/1.92; //7.6v viram 1.92v (divisor de tensão)
        int pwm_tensao = (7.05*4095)/tensaoBateria;

        // aplica o pwm
        //applyPWM(&wheelRight, pwm_tensao);
        applyPWM(&wheelLeft, pwm_tensao);  

        // freia a outra roda
        brake(&wheelRight);

        medidas[0][counter] = encoderLeft.getPulses();//encoderLeft.getRPS();
        encoderLeft.limpaCounter();
        medidas[1][counter] = encoderRight.getPulses();// encoderRight.getRPS();
        encoderRight.limpaCounter();

        medidas[2][counter] = millis() - timeStart;
        medidas[3][counter] = tensaoBateria;

        counter++;
        delay(50);
    }

    brake(&wheelLeft);
    brake(&wheelRight);

    counter = 0;

    if (!poster.begin(GOOGLE_SCRIPT, scriptId)){
    //Serial.printf("Error in poster initialization\n");
    return;
    }

    for(uint8_t i = 0; i< 80; i++){
        updateMsg();
        if (!poster.post(json)) Serial.printf("Message %d not delivered\n", counter);
        counter++;
    }

    delay(10000);
}