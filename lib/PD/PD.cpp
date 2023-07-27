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

int32_t PD::simplePID(double kp, double ki, double kd, double error, int32_t limit){
    float t = (millis() - lastTime[4])/1000.0;
    lastTime[4] = millis();

    // proporcional
    double P = kp * error;
    
    // integral 
    double I = I_ant + (error * ki * t);

    // derivativa
    double D = (error - lastError[4])/t * kd;

    // garante que a integral nao sature
    if(I >= limit/2)        I = limit/2;
    else if (I <= -limit/2) I = -limit/2;

    // salva o ultimo erro
    lastError[4] = error;

    I_ant = I;

    // soma tudo
    pid = (P + I + D);

    // garante que nao sature
    if(pid >= limit)
        pid = limit;
    else if(pid <= -limit)
        pid = -limit;
    
    if(verb){
        Serial.printf("Tempo: %.2f\t", t);
        Serial.printf("P: %.2f\t I: %.2f\tD: %.2f\t", P, I, D);
        Serial.printf("PI: %d\t", pid);
        Serial.printf("Erro passado: %.2f,\t Erro: %.2f\n", lastError[4], error);
    }

    return pid;
}   

int32_t PD::rightPI(double kp, double ki, double error, int32_t limit){
    // calcula quanto tempo passou desde o ultimo calculo
    float t = (millis() - lastTime[1])/1000.0;
    lastTime[1] = millis();

    // proporcional
    double P = kp * error;
    
    // integral 
    double I = ((error - lastError[1]) * ki) * t;

    // garante que a integral nao sature
    if(I >= limit/2)        I = limit/2;
    else if (I <= -limit/2) I = -limit/2;
    //else                    sumError[0] += error;

    // salva o ultimo erro
    lastError[1] = error;

    pi1 += (P + I);

    if(pi1 >= limit)
        pi1 = limit;
    else if(pi1 <= -limit)
        pi1 = -limit;

    if(verb){
        Serial.printf("Tempo: %.2f\t", t);
        Serial.printf("P: %.2f\t I: %.2f\t", P, I);
        Serial.printf("PI: %d\t", pi1);
        Serial.printf("Erro passado: %.2f,\t Erro: %.2f\n", lastError[1], error);
    }

    return pi1;
}

int32_t PD::leftPI(double kp, double ki, double error, int32_t limit){
    // calcula quanto tempo passou desde o ultimo calculo
    float t = (millis() - lastTime[2])/1000.0;
    lastTime[2] = millis();

    // proporcional
    double P = kp * error;
    
    // integral 
    double I = ((error - lastError[2]) * ki) * t;

    // garante que a integral nao sature
    if(I >= limit/2)        I = limit/2;
    else if (I <= -limit/2) I = -limit/2;
    //else                    sumError[1] = lastError;

    // salva o ultimo erro
    lastError[2] = error;
    
    pi2 += (P + I);

    if(pi2 >= limit)
        pi2 = limit;
    else if(pi2 <= -limit)
        pi2 = -limit;

    if(verb){
        Serial.printf("Tempo: %.2f\t", t);
        Serial.printf("P: %.2f \t I: %.2f\t", P, I);
        Serial.printf("PI: %d\t", pi2);
        Serial.printf("Erro passado: %.2f,\t Erro: %.2f\n", lastError[2], error);
    }

    return pi2;
}

int32_t PD::simplePD1(double kp, double kd, int32_t error, int32_t limit){
    // calcula quanto tempo passou desde o ultimo calculo
    float t = (millis() - lastTime[0])/1000.0;
    lastTime[0] = millis();

    // proporcional 
    double P = kp * error;

    // derivativa
    double D = (error - lastError[0])/t * kd;

    // soma tudo 
    pd1 += (P + D);

    // garante que o PD nao sature
    if(pd1 >= limit)
        pd1 = limit;
    else if(pd1 <= -limit)
        pd1 = -limit;

    if(verb){
        Serial.printf("Tempo: %.2f \t", t);
        Serial.printf("P: %f \t D: %f \t", P, D);
        Serial.printf("PD: %.3f \t", pd1);
        Serial.printf("Erro passado: %.3f, \t Erro: %d \n", lastError[0], error);
    }

    // salva o ultimo erro
    lastError[0] = error;

    return pd1;
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
        Serial.printf("Erro passado: %d, \t Erro: %d \n", lastError[3], error);
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
    case LEFT:
        return rightPI(kple, kile, error, limit);
    case RIGHT:
        return leftPI(kpri, kiri, error, limit);

    default:
        Serial.println("Modo não encontrado!"); 
        return 0;
    }
}

