// autor Alison
// email AlisonTristao@hotmail.com

#pragma once // substitui o indef endif
#include <cstdint>

class mathModel {
    public:
        /**
         * inicia a biblioteca com os dados do carrinho
        */
        void begin(int8_t carVector[3][2], uint32_t actingTime);
        /**
         * define o setPoint de velocidade de cada roda de 
         * acordo com a distancia do sensor em relação
         * a linha
        */
        uint32_t* calculateSetPoints(int8_t lineDistance);
    private:
        /**
         * cordenas X Y
         * linha 0 distancia da roda esquerda
         * linha 1 distancia da roda direita
         * linha 2 distancia da linha de sensores
        */
        int8_t carVector[3][2];
        /**
         * setPoint de velocidade de cada roda
         * linha 0 roda esquerda
         * linha 1 roda direita
        */
        uint32_t wheelsSpeed[2];
        /** 
         * velocidade angular, velocidade translacional
         * e a velocidade tranlacional em xy do carrinho
        */
        float omegaWheels, translationalSpeed, tSpeed[2];
        /**
         * tempo definido para que o carrinho alcace a medida calculada
         * (vai resultar a velocidade media translacional)
        */
        uint32_t actingTime;
};