// autor Alison
// email AlisonTristao@hotmail.com

#include "PID.h"
#include <Arduino.h>

PID::PID(){
    this->kpr = 1; 
    this->kir = 1; 
    this->kdr = 1; 
    this->limit = 1;
    verb = false;
    lastTime = 0;
    sumError = 0;
}

PID::PID(double kpr, double kir, double kdr, int limit){
    this->kpr = kpr; 
    this->kir = kir; 
    this->kdr = kdr; 
    this->limit = limit;
    verb = false;
    lastTime = 0;
    sumError = 0;
}
    
PID::PID(double kpr, double kir, double kdr,
            double kpt, double kit, double kdt, int limit){
    this->kpr = kpr; 
    this->kir = kir; 
    this->kdr = kdr; 
    this->kpt = kpt; 
    this->kit = kit; 
    this->kdt = kdt; 
    this->limit = limit;
    verb = false;
    lastTime = 0;
    sumError = 0;
}

PID::~PID(){
}

void PID::setConst(double kpr, double kir, double kdr){
    this->kpr = kpr; 
    this->kir = kir; 
    this->kdr = kdr; 
}

void PID::setConst(double kpr, double kir, double kdr,
            double kpt, double kit, double kdt){
    this->kpr = kpr; 
    this->kir = kir; 
    this->kdr = kdr; 
    this->kpt = kpt; 
    this->kit = kit; 
    this->kdt = kdt; 
}

void PID::setLimit(int limit){
    this->limit = limit; 
}

void PID::setVerb(bool verb){
    this->verb = verb; 
}

void PID::printConfig(){
    Serial.printf("Constantes rotacionais: \n kp = %f\n ki = %f \n kd = %f \n\n", kdr, kir, kdr);
    Serial.printf("Constantes translacionais: \n kp = %f\n ki = %f \n kd = %f \n\n", kdt, kit, kdt);
    Serial.printf("Velocidade: %d \n\n", limit);
}

int32_t PID::simplePID(double kp, double ki, double kd, int32_t error){
    // calcula quanto tempo passou desde o ultimo calculo
    float t = (millis() - lastTime)/1000.0;
    lastTime = millis();

    // proporcional
    double P = kp * error;
    
    // integral 
    double I = (sumError * ki) * t;

    // derivativa
    double D = (error - lastError)/t * kd;

    // garante que a integral nao sature
    if((I >= limit) && (error >= 0)){
        I = limit;
    }else if((I <= -limit) && (error <= 0)){
        I = -limit;
    }else{
        sumError += error;
    }

    // soma tudo 
    int32_t PID = P + I + D;

    // garante que o PID nao sature
    if(PID > limit){
        PID = limit;
    }else if(PID < -limit){
        PID = -limit;
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

int32_t PID::simplePI(double kp, double ki, double error, int32_t localLimit){
    // calcula quanto tempo passou desde o ultimo calculo
    float t = (millis() - lastTime)/1000.0;
    lastTime = millis();

    // proporcional
    double P = kp * error;
    
    // integral 
    double I = (sumError * ki) * t;

    // garante que a integral nao sature
    if((I >= localLimit) && (error >= 0)){
        I = localLimit;
    }else if((I <= -localLimit) && (error <= 0)){
        I = -localLimit;
    }else{
        sumError += error;
    }

    // soma tudo 
    int32_t PI_ = P + I;

    // garante que o PID nao sature
    if(PI_ > localLimit){
        PI_ = localLimit;
    }else if(PI_ < -localLimit){
        PI_ = -localLimit;
    }

    if(verb){
        Serial.printf("Tempo: %f \t", t);
        Serial.printf("P: %f \t I: %f\t", P, I);
        Serial.printf("PID: %d \t", PI_);
        Serial.printf("Erro somado: %d \t Erro passado: %d, \t Erro: %d \n", sumError, lastError, error);
    }

    // salva o ultimo erro
    lastError = error;

    return PI_;
}

int32_t PID::simplePD(double kp, double kd, int32_t error){
    // calcula quanto tempo passou desde o ultimo calculo
    float t = (millis() - lastTime)/1000.0;
    lastTime = millis();

    // proporcional
    double P = kp * error;

    // derivativa
    double D = (error - lastError)/t * kd ;

    // soma tudo 
    int32_t P_D = P + D;

    // garante que o PID nao sature
    if(P_D > limit){
        P_D = limit;
    }else if(P_D < -limit){
        P_D = -limit;
    }

    if(verb){
        Serial.printf("Tempo: %f \t", t);
        Serial.printf("P: %f \t D: %f \t", P, D);
        Serial.printf("PID: %d \t", P_D);
        Serial.printf("Erro somado: %d \t Erro passado: %d, \t Erro: %d \n", sumError, lastError, error);
    }

    // salva o ultimo erro
    lastError = error;

    return P_D;
}

int32_t PID::calcPID(int32_t error){
    return simplePID(kpr, kir, kdr, error);
}

int32_t PID::calcPID(int32_t error, uint8_t mode){
    switch (mode){
    case ROTACIONAL:
        return simplePID(kpr, kir, kdr, error);
    case TRANSLACIONAL:
        return simplePID(kpt, kit, kdt, error);

    default:
        Serial.println("Modo não encontrado!"); 
        return 0;
    }
}


