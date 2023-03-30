#include "mathModel.h"
#include <cmath>
#include <Arduino.h>

void mathModel::begin(double carVector[3][2], double wheelsRadius, double actingTime){
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 2; j++)
            this->carVector[i][j] = carVector[i][j];
    }
    this->wheelsRadius = wheelsRadius;
    this->actingTime = actingTime;
}

double* mathModel::calculateSetPoints(double lineDistance){
    // atan2 da distancia da linha com a distancia do sensor frontal
    // para calcularmos o angulo entre o eixo do carrinho e a linha
    double peta = atan2(lineDistance, carVector[2][1]);

    // calculamos as velocidade totais do carrinho
    omegaCar = peta/actingTime;
    translationalSpeed = carVector[2][1]/actingTime;

    /*
        FAZER TESTES COM VELOCIDADE DA HIPOTENUSA ENTRE
        CARVECTOR[2][1] E LINEDISTANCE
    */

    // calculamos a velocidade em y de cada roda
    // a componente em x nao usamos pois a roda so gira pra frente
    wheelsSpeed[0] = translationalSpeed + 
    (carVector[0][0] * omegaCar * cos(peta)) +
    (carVector[0][1] * omegaCar * sin(peta));

    wheelsSpeed[1] = translationalSpeed + 
    (carVector[1][0] * omegaCar * cos(peta)) +
    (carVector[1][1] * omegaCar * sin(peta));

    // convertemos a velocidade tangencial adquirida para omega
    wheelsSpeed[0] *= wheelsRadius;
    wheelsSpeed[1] *= wheelsRadius;

    return wheelsSpeed;
}

