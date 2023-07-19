// autor Alison
// email AlisonTristao@hotmail.com

#include "PD.h"
#include <Arduino.h>
    
PD::PD(double kpr, double kdr, double kpt, double kdt){
    this->kpr = kpr; 
    this->kdr = kdr; 
    this->kpt = kpt; 
    this->kdt = kdt; 
}

PD::~PD(){
}

void PD::setConst(double kpr, double kdr, double kpt, double kdt){
    this->kpr = kpr; 
    this->kdr = kdr; 
    this->kpt = kpt; 
    this->kdt = kdt; 
}

void PD::setConstPiMotors(double kple, double kile, double kpri, double kiri){
    this->kple = kple; 
    this->kile = kile; 
    this->kpri = kpri; 
    this->kiri = kiri; 
}

void PD::setVerb(bool verb){
    this->verb = verb; 
}

void PD::printConfig(){
    Serial.printf("Constantes rotacionais: \nkp = %f\nkd = %f \n\n", kdr, kdr);
    Serial.printf("Constantes translacionais: \nkp = %f\nkd = %f \n\n", kdt, kdt);
}

int32_t PD::simplePI1(double kp, double ki, double error, int32_t limit){
    // calcula quanto tempo passou desde o ultimo calculo
    float t = (millis() - lastTime[1])/1000.0;
    lastTime[1] = millis();

    // proporcional
    double P = kp * error;
    
    // integral 
    double I = (sumError[0] * ki) * t;

    // garante que a integral nao sature
    if((I >= limit) && (error >= 0)){
        I = limit;
    }else if((I <= -limit) && (error <= 0)){
        I = -limit;
    }else{
        sumError[0] += error;
    }

    // soma tudo 
    int32_t PI_ = P + I;

    // garante que o PID nao sature
    if(PI_ > limit){
        PI_ = limit;
    }else if(PI_ < -limit){
        PI_ = -limit;
    }

    if(verb){
        Serial.printf("Tempo: %f \t", t);
        Serial.printf("P: %f \t I: %f\t", P, I);
        Serial.printf("PI: %d \t", PI_);
        Serial.printf("Erro somado: %d \t Erro passado: %d, \t Erro: %d \n", sumError, lastError, error);
    }

    // salva o ultimo erro
    lastError[0] = error;

    return PI_;
}
int32_t PD::simplePI2(double kp, double ki, double error, int32_t limit){
    // calcula quanto tempo passou desde o ultimo calculo
    float t = (millis() - lastTime[2])/1000.0;
    lastTime[2] = millis();

    // proporcional
    double P = kp * error;
    
    // integral 
    double I = (sumError[1] * ki) * t;

    // garante que a integral nao sature
    if((I >= limit) && (error >= 0)){
        I = limit;
    }else if((I <= -limit) && (error <= 0)){
        I = -limit;
    }else{
        sumError[1] += error;
    }

    // soma tudo 
    int32_t PI_ = P + I;

    // garante que o PID nao sature
    if(PI_ > limit){
        PI_ = limit;
    }else if(PI_ < -limit){
        PI_ = -limit;
    }

    if(verb){
        Serial.printf("Tempo: %f \t", t);
        Serial.printf("P: %f \t I: %f\t", P, I);
        Serial.printf("PI: %d \t", PI_);
        Serial.printf("Erro somado: %d \t Erro passado: %d, \t Erro: %d \n", sumError, lastError, error);
    }

    // salva o ultimo erro
    lastError[1] = error;

    return PI_;
}

int32_t PD::simplePD1(double kp, double kd, int32_t error, int32_t limit){
    // calcula quanto tempo passou desde o ultimo calculo
    float t = (millis() - lastTime[0])/1000.0;
    lastTime[0] = millis();

    // proporcional
    double P = kp * error;

    // derivativa
    double D = (error - lastError[0])/t * kd ;

    // soma tudo 
    int32_t P_D = P + D;

    // garante que o PD nao sature
    if(P_D > limit){
        P_D = limit;
    }else if(P_D < -limit){
        P_D = -limit;
    }

    if(verb){
        Serial.printf("Tempo: %f \t", t);
        Serial.printf("P: %f \t D: %f \t", P, D);
        Serial.printf("PD: %d \t", P_D);
        Serial.printf("Erro somado: %d \t Erro passado: %d, \t Erro: %d \n", sumError, lastError, error);
    }

    // salva o ultimo erro
    lastError[0] = error;

    return P_D;
}

int32_t PD::simplePD2(double kp, double kd, int32_t error, int32_t limit){
    // calcula quanto tempo passou desde o ultimo calculo
    float t = (millis() - lastTime[3])/1000.0;
    lastTime[3] = millis();

    // proporcional
    double P = kp * error;

    // derivativa
    double D = (error - lastError[3])/t * kd ;

    // soma tudo 
    int32_t P_D = P + D;

    // garante que o PD nao sature
    if(P_D > limit){
        P_D = limit;
    }else if(P_D < -limit){
        P_D = -limit;
    }

    if(verb){
        Serial.printf("Tempo: %f \t", t);
        Serial.printf("P: %f \t D: %f \t", P, D);
        Serial.printf("PD: %d \t", P_D);
        Serial.printf("Erro somado: %d \t Erro passado: %d, \t Erro: %d \n", sumError, lastError, error);
    }

    // salva o ultimo erro
    lastError[3] = error;

    return P_D;
}

int32_t PD::calcPD(uint8_t mode, int32_t error, int32_t limit){
    switch (mode){
    case ROTACIONAL:
        return simplePD1(kpr, kdr, error, limit);
    case TRANSLACIONAL:
        return simplePD2(kpt, kdt, error, limit);

    default:
        Serial.println("Modo não encontrado!"); 
        return 0;
    }
}

int32_t PD::calcPI(uint8_t mode, int32_t error, int32_t limit){
    switch (mode){
    case MOTOR_LEFT:
        return simplePI1(kple, kile, error, limit);
    case MOTOR2_RIGHT:
        return simplePI2(kpri, kiri, error, limit);

    default:
        Serial.println("Modo não encontrado!"); 
        return 0;
    }
}

