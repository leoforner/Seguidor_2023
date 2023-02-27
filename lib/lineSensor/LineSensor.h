// autor Alison
// email AlisonTristao@hotmail.com

#pragma once // substitui o indef endif
#include <cstdint>

class LineSensor {
    public:
        // calcula onde a linha está 
        uint32_t searchLine();
        /**
         * @brief
            Define os pontos maximos e minimos de cada sensor
            modos 0 ou 1
            @param
            0: mede o preto e apos 2 segundos mede o branco
            com o carrinho estatico nos 2 pontos.
            @param
            1: salva os pontos maximos e minimos capitado 
            com o carrinho passando sobre a linha
        */
        void calibration(uint8_t mode); 
        // verifica se os valores de claro e escuro são validos
        void isValid();
        /**
            le o valor que um sensor está capitando
            @param
            index = numero do sensor
        */
        uint32_t read(uint8_t index);
        /*  
            inicia o sensor frontal dizendo quantos sensores ele possui,
            seus respectivos pinos e se a linha é branca ou preta
        */
        void begin(uint8_t sensorCount, uint8_t sensorPins[8], bool lineWhite);
        // caso queira mudar as medidas padrões dos calculos
        void setTrackCharacteristics(uint16_t line, uint16_t rug, uint16_t lineTolerance);
        // busca os dados do sensor
        void printConfig();
        // define se vai printar comentarios das funcoes
        void setVerb(bool verb);
    private:
        // Usar ou nao os prints das funções
        bool verb;
        // calcula os pontos maximos e minimos com o sensor estatico
        void setMaxAndMinAv();
        // calcula os pontos maximos e minimos com o sensor em movimento
        void setMaxAndMinEx();
        // coleta as medições do sensord
        void getValues(uint16_t * array); 
        // contem os pontos maximos e minimos de cada sensor
        uint16_t maximum[8], minimum[8];
        // quantidade e pinos sensores
        uint8_t sensorCount, sensorPins[8];
        // valor do branco, do preto e o quanto ainda é considerado branco 
        uint16_t line, rug, lineTolerance;
        // salva se a linha é branca ou preta
        bool lineWhite;
        // ultima posicao da linha
        int lastPosition;  
};