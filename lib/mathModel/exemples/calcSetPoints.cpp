#include <Arduino.h>
#include <lineSensor.h>
#include <mathModel.h>

lineSensor ls;
uint8_t pinos[8] = {34, 35, 32, 33, 25, 26, 27, 14}, pinsCount = 8;
float pesos[8];

mathModel mm;               // X    Y
double carVector[3][2] =   {{-3.5, -2.0},    // roda direita
                            {+3.5, -2.0},    // roda esquerda
                            {+0.0, +8.0}};   // linha de sensores
double wheelsRadius = 2.0, actingTime = 0.5;

void setup() {
    Serial.begin(115200);
    ls.begin(pinsCount, pinos, true);
    mm.begin(carVector, wheelsRadius, actingTime);

    // a linha tem 5.7 centimetros com 8 sensores
    for(int i = 0; i < 8; i++)
        pesos[i] = i * 5.7/7; 
    // altera o peso padrao para calcular a distancia da linha
    ls.setweights(pesos);

    // dessa forma a saida esta em nanometro
    // divindo por 100 a medida passa para centimetros
    ls.setTrackCharacteristics(100, 0, 30);

    // calibramos o sensor
    ls.calibration(ESTATICO);

    // printamos o resultado da calibração e configuração do sensor
    ls.printConfig();
}

void loop() {
    uint32_t timer = 0;
    timer = millis(); // salva o tempo

    // distancia entre o sensor e a linha em cm
    double lineDistance = (ls.searchLine()/100.0) - 5.70/2.0;

    // calcula o setPoint de cada roda em cm/s
    double* wheelsSetPoint = mm.calculateSetPoints(lineDistance);

    // resultados
    Serial.printf("erro: %.2f\tr1: %.3frps\tr2: %.3frps\ttempo: %.2fms\n", 
                                lineDistance, wheelsSetPoint[0], wheelsSetPoint[1], millis() - timer);
    delay(100);
}