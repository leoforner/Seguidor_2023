// autor Alison
// email AlisonTristao@hotmail.com

#pragma once // substitui o indef endif
#include <cstdint>

typedef enum{
    ESTATICO,
    DINAMICO
}calibracao;

class LineSensor {
    public:
        // calcula onde a linha está 
        uint32_t searchLine();
        /**
         * @brief
            Define os pontos maximos e minimos de cada sensor
            modos 0 ou 1
            @param
            ESTATICO: mede o preto e apos 2 segundos mede o branco
            com o carrinho estatico nos 2 pontos.
            @param
            DINAMICO: salva os pontos maximos e minimos capitado 
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
        uint32_t readNormalized(uint8_t index);
        /*  
            inicia o sensor frontal dizendo quantos sensores ele possui,
            seus respectivos pinos e se a linha é branca ou preta
        */
        void begin(uint8_t sensorCount, uint8_t sensorPins[], bool lineWhite);
        void beginMultiplex(uint8_t pinCount, uint8_t pinos[], uint8_t multiplexOut, bool lineWhite);
        // caso queira mudar as medidas padrões dos calculos
        void setTrackCharacteristics(uint16_t line, uint16_t rug, uint16_t lineTolerance);
        // busca os dados do sensor
        void printConfig();
        // define se vai printar comentarios das funcoes
        
        void setVerb(bool verb);
    private:
        // le um pino pelo multiplex 
        uint32_t read(uint8_t index);
        // Usar ou nao os prints das funções
        bool verb;
        // calcula os pontos maximos e minimos com o sensor estatico
        void setMaxAndMinEs();
        // calcula os pontos maximos e minimos com o sensor em movimento
        void setMaxAndMinDi();
        // coleta as medições do sensord
        void getValues(uint16_t * array); 
        // contem os pontos maximos e minimos de cada sensor
        uint16_t* maximum,* minimum;
        // quantidade e pinos sensores
        uint8_t* sensorPins;
        // valor do branco, do preto e o quanto ainda é considerado branco 
        uint16_t line, rug, lineTolerance;
        // pinos do multiplexador
        uint8_t* pinos, multiplexOut, sensorCount, pinsCount;
        // salva se a linha é branca ou preta
        bool lineWhite, multiplex;
        // ultima posicao da linha
        int lastPosition;  
};