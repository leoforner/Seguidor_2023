#include "mathModel.h"
#include <cmath>
#include <Arduino.h>

mathModel::mathModel(double carVector[3][2], double wheelsRadius, double actingTime, double * wheelsSetPoint){
    this->wheelsSpeed = wheelsSetPoint;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 2; j++)
            this->carVector[i][j] = carVector[i][j];
    }
    this->wheelsRadius = wheelsRadius;
    this->actingTime = actingTime;
    verb = false;
}

mathModel::~mathModel(){}

void mathModel::setVerb(bool verb){
    this-> verb = verb;
}

void mathModel::calculateSetPoints(double lineDistance){
    // atan2 da distancia da linha com a distancia do sensor frontal
    // para calcularmos o angulo entre o eixo do carrinho e a linha
    double peta = (atan2(lineDistance, carVector[2][1]))*2;

    // calculamos as velocidade totais do carrinho
    omegaCar = peta/actingTime;
    translationalSpeed = carVector[2][1]/actingTime;

    // calculamos a velocidade em y de cada roda
    // a componente em x nao usamos pois a roda so gira pra frente
    wheelsSpeed[0] = translationalSpeed 
    + (carVector[0][0] * omegaCar * sin(abs(peta)))
    + (carVector[0][1] * omegaCar * cos(abs(peta)));

    wheelsSpeed[1] = translationalSpeed
    + (carVector[1][0] * omegaCar * sin(abs(peta)))
    - (carVector[1][1] * omegaCar * cos(abs(peta)));

    // convertemos a velocidade tangencial adquirida para omega rad/s
    // e depois para rps
    wheelsSpeed[0] /= wheelsRadius*2*PI;
    wheelsSpeed[1] /= wheelsRadius*2*PI;

    if(verb) Serial.printf("peta: %f\tomega: %f\tVm: %f\t", peta, omegaCar, translationalSpeed);
}