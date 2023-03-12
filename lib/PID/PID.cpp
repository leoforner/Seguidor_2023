// autor Alison
// email AlisonTristao@hotmail.com

#include "PID.h"
#include <Arduino.h>

void
PID::begin(double kpr, double kir, double kdr, int speed){
    this->kpr = kpr; 
    this->kir = kir; 
    this->kdr = kdr; 
    this->speed = speed;
    verb = false;
    lastTime = 0;
    sumError = 0;
}

void
PID::begin(double kpr, double kir, double kdr,
            double kpt, double kit, double kdt, int speed){
    this->kpr = kpr; 
    this->kir = kir; 
    this->kdr = kdr; 
    this->kpt = kpt; 
    this->kit = kit; 
    this->kdt = kdt; 
    this->speed = speed;
    verb = false;
    lastTime = 0;
    sumError = 0;
}

void
PID::setConst(double kpr, double kir, double kdr){
    this->kpr = kpr; 
    this->kir = kir; 
    this->kdr = kdr; 
}

void
PID::setConst(double kpr, double kir, double kdr,
            double kpt, double kit, double kdt){
    this->kpr = kpr; 
    this->kir = kir; 
    this->kdr = kdr; 
    this->kpt = kpt; 
    this->kit = kit; 
    this->kdt = kdt; 
}

void
PID::setSpeed(int speed){
    this->speed = speed; 
}

void
PID::setVerb(bool verb){
    this->verb = verb; 
}

void
PID::printConfig(){
    Serial.printf("Constantes rotacionais: \n kp = %f\n ki = %f \n kd = %f \n\n", kdr, kir, kdr);
    Serial.printf("Constantes translacionais: \n kp = %f\n ki = %f \n kd = %f \n\n", kdt, kit, kdt);
    Serial.printf("Velocidade: %d \n\n", speed);
}

int32_t
PID::simplePID(double kp, double ki, double kd, int32_t error){
    // calcula quanto tempo passou desde o ultimo calculo
    float t = (millis() - lastTime)/1000.0;
    lastTime = millis();

    // proporcional
    double P = kp * error;
    
    // integral 
    double I = (sumError * ki) * t;

    // derivativa
    double D = (error - lastError) * kd * t;

    // garante que a integral nao sature
    if((I >= speed) && (error >= 0)){
        I = speed;
    }else if((I <= -speed/2) && (error <= 0)){
        I = -speed;
    }else{
        sumError += error;
    }

    // soma tudo 
    int32_t PID = P + I + D;

    // garante que o PID nao sature
    if(PID > speed){
        PID = speed;
    }else if(PID < -speed){
        PID = -speed;
    }

    if(verb){
        Serial.printf("Tempo: %f \t", t);
        Serial.printf("P: %f \t I: %f, \t D: %f \t", P, I, D);
        Serial.printf("PID: %d \t", PID);
        Serial.printf("Erro somado: %d \t Erro passado: %d, \t Erro: %d \n", sumError, lastError, error);
    }

    // salva o ultimo erro
    lastError = error;

    return PID;
}

int32_t 
PID::calcPID(int32_t error){
    return simplePID(kpr, kir, kdr, error);
}

int32_t 
PID::calcPID(int32_t error, uint8_t mode){
    switch (mode){
    case ROTACIONAL:
        return simplePID(kpr, kir, kdr, error);
    case TRANSLACIONAL:
        return simplePID(kpt, kit, kdt, error);

    default:
        Serial.println("Modo não encontrado!"); break;
    }
}


