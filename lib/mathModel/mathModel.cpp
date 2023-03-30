#include "mathModel.h"
#include <cstdint>
#include <cmath>
#include <Arduino.h>

void mathModel::begin(int8_t carVector[3][2], uint32_t actingTime){
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 2; j++)
            this->carVector[i][j] = carVector[i][j];
    }
    this->actingTime = actingTime;
}

uint32_t* mathModel::calculateSetPoints(int8_t lineDistance){
    // atan2 da distancia da linha com a distancia do sensor frontal
    double peta = atan2(lineDistance, carVector[2][1]);

    // calculamos as velocidade totais do carrinho
    omegaWheels = peta/actingTime;
    translationalSpeed = carVector[2][1]/actingTime;

    // demembramos a velocidade translacional em componentes XY
    tSpeed[0] = sin(peta) * translationalSpeed;
    tSpeed[1] = cos(peta) * translationalSpeed;

    // calculamos a velocidade em y de cada roda]
    // a componente em x nao usamos pois a roda so gira pra frente
    //wheelsSpeed[0] = tSpeed[0] + (carVector[0][0] * omega * cos(peta));
}

