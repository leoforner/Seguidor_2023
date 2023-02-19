#include "LineSensor.h"
#include <cstdint>
#include <Arduino.h>

void LineSensor::begin(uint8_t sensorCount, uint8_t sensorPins[8], bool lineWite){
    this->sensorCount = sensorCount;
    this->sensorPins[8] = sensorPins[8];
    this->lineWhite = lineWhite;
    // configrações padroes
    pinMode(2, OUTPUT);
    line = 1000;
    rug = 0;
    lineTolerance = 0.3 * line;

    for(int i = 0; i < sensorCount; i++){
        pinMode(sensorPins[i], INPUT);
        Serial.println(sensorPins[i]);
    }
}

void LineSensor::setTrackCharacteristics(uint16_t line, uint16_t rug, uint16_t lineTolerance){
    this->line = line;
    this->rug = rug;
    this->lineTolerance = lineTolerance;
}

void LineSensor::getMax(){
    // calcula uma media de 10 medidas
    for(uint8_t i = 0; i < sensorCount; i++){
        int sum = 0;
        for(uint8_t j = 0; j < 10; j++){
            sum += analogRead(sensorPins[i]);
            delay(5);
        }
        maximum[i] = sum/10; // salva a media
    }
}

void LineSensor::getMin(){
    // calcula uma media de 10 medidas
    for(uint8_t i = 0; i < sensorCount; i++){
        int sum = 0;
        for(int j = 0; j < 10; j++){
            sum += analogRead(sensorPins[i]);
            delay(5);
        }
        minimum[i] = sum/10; // salva a media
    }
}

void LineSensor::isValid(){
    // caso os valores de maximos e minimos nao fazerem sentido 
    // não é possivel calcular a posição da linha

    Serial.println("O resultado da medida foi:");
    Serial.print("Minimos: \t");
    for(int i = 0; i < sensorCount; i++){
        Serial.print(minimum[i]);
        Serial.print("\t");
    }
    Serial.println();
    Serial.print("Maximos: \t");
    for(int i = 0; i < sensorCount; i++){
        Serial.print(maximum[i]);
        Serial.print("\t"); 
    }
    Serial.println();

    for(uint8_t i = 0; i < sensorCount; i++){
        Serial.println(minimum[i]);
        if(minimum[i] >= maximum[i]){
            Serial.println("Erro. Refaça as medidas!");
            while(true){ // prende em um loop com o led piscando para sinalizar
                digitalWrite(2, HIGH);
                delay(250);
                digitalWrite(2, LOW);
                delay(250);
            }
        }
    }
}

void LineSensor::setMaxAndMinAv(){
    // mede o quanto está lendo na parte escura da pista
    digitalWrite(2, HIGH);
    Serial.println("Medindo a itensidade da luz para o preto...");
    delay(2000);
    digitalWrite(2, LOW);
    getMax();

    delay(100);

    // mede o quanto está lendo na parte clara da pista
    digitalWrite(2, HIGH);
    Serial.println("Medindo a itensidade da luz para o branco...");
    delay(2000);
    digitalWrite(2, LOW);
    getMin();

    // verifica se as medidas sao validas
    isValid();
}

void LineSensor::setMaxAndMinEx(){
    // salva os pontos maximos e minimos de cada sensor
    // lidos em 5 segundos
    int start = millis();
    digitalWrite(2, HIGH);
    while((millis() - start) < 5000){
        for(uint8_t i = 0; i < sensorCount; i++){
            int x = analogRead(sensorPins[i]);
            if(x > maximum[i])
                maximum[i] = x;
            if(x < minimum[i])
                minimum[i] = x;
        }
    }
    digitalWrite(2, LOW);

    // verifica se as medidas sao validas
    isValid();
}

void LineSensor::calibration(uint8_t mode){
    switch (mode){
    case 0:
        setMaxAndMinAv();
        break;
    case 1:
        setMaxAndMinEx();
        break;

    default:
        Serial.printf("Erro em calibration!");
        break;
    }
}

uint32_t LineSensor::read(uint8_t index, bool lineWite){
    int value = analogRead(sensorPins[index]);

    if(lineWhite){
        value = map(value, minimum[index], maximum[index], rug, line);
    }else{
        value = map(value, minimum[index], maximum[index], line, rug);
    }

    // garante que nenhum valor vai estrapolar os limites
    if (value < rug)
        value = rug;

    if (value > line)
        value = line;

    return value;
}

uint32_t LineSensor::searchLine(){
    // calcula onde a linha esta
    long int sum = 0, measuraments = 0;
    bool inLine = lineWhite;
    for(uint8_t i = 0; i < sensorCount; i++){
        int x = read(i, lineWhite);
        sum += x * line * i+1;  // soma da media multiplicado pelo peso do sensor
        measuraments += x;           // soma das medidas
        if(x > lineTolerance) 
            inLine = true;
    }
    if(inLine){
        lastPosition = sum/measuraments; // media ponderada
    }else{ 
        // caso nao detecte a linha ele satura pro ultimo lado visto
        if(lastPosition < ((sensorCount-1)*line)/2){
            lastPosition = 0;
        }else{
            lastPosition = (sensorCount-1) * line;
        }
    }
    return lastPosition;
}