#pragma once // substitui o indef endif
#include <cstdint>

class LineSensor {
    public:
        // calcula onde a linha está 
        uint32_t searchLine();
        // coleta os pontos referentes ao escuro e claro de cada sensor
        void calibration(uint8_t mode);
        // verifica se os valores de claro e escuro são validos
        void isValid();
        // le o valor que um sensor está capitando
        uint32_t read(uint8_t index, bool lineWite);
        // inicia o sensor frontal dizendo quantos sensores ele possui,
        //seus respectivos pinos e se a linha é branca ou preta
        void begin(uint8_t sensorCount, uint8_t sensorPins[8], bool lineWhite);
        // caso queira mudar as medidas padrões dos calculos
        void setTrackCharacteristics(uint16_t line, uint16_t rug, uint16_t lineTolerance);
    private:
        // calcula os pontos maximos e minimos com o sensor estatico
        void setMaxAndMinAv();
        // calcula os pontos maximos e minimos com o sensor em movimento
        void setMaxAndMinEx();
        // coleta os pontos maximos
        void getMax(); 
        // coleta os pontos minimos
        void getMin();
        // contem os pontos maximos e minimos de cada sensor
        int16_t maximum[8], minimum[8];
        // quantidade e pinos sensores
        uint8_t sensorCount, sensorPins[8];
        // valor do branco, do preto e o quanto ainda é considerado branco 
        uint16_t line, rug, lineTolerance;
        // salva se a linha é branca ou preta
        bool lineWhite;
        // ultima posicao da linha
        int lastPosition;  
};