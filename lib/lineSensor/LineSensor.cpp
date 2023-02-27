#include "LineSensor.h"
#include <cstdint>
#include <Arduino.h>

void LineSensor::begin(uint8_t sensorCount, uint8_t sensorPins[8], bool lineWhite){
    this->sensorCount = sensorCount;
    for(int i = 0; i < sensorCount; i++)
        this->sensorPins[i] = sensorPins[i];
    this->lineWhite = lineWhite;
    // configrações padroes
    pinMode(2, OUTPUT);
    verb = false;
    line = 1000;
    rug = 0;
    lineTolerance = 0.3 * line;

    // deixa os valores maximos e minimos diferentes de null
    for(int i = 0; i < sensorCount; i++){
        maximum[i] = 0;
        minimum[i] = 4095;
    }
}

void LineSensor::printConfig(){
    Serial.print("\nAs configurações do sensor são:\n");
    Serial.printf("Nº de sensores: %d \n", sensorCount);
    for(int i = 0; i < sensorCount; i++){
        Serial.printf("Pin %d = %d \t", i+1, sensorPins[i]);
        Serial.printf("Max: %d \t Min: %d \n", maximum[i], minimum[i]) ;
    }
    Serial.printf("Linha branca: %s \t", (lineWhite ? "True" : "False"));
    Serial.printf("Linha: %d \t Pista: %d \t Toletancia da linha: %d \n \n", line, rug, lineTolerance);
}

void LineSensor::setVerb(bool verb){
    this->verb = verb;
}

void LineSensor::setTrackCharacteristics(uint16_t line, uint16_t rug, uint16_t lineTolerance){
    this->line = line;
    this->rug = rug;
    this->lineTolerance = lineTolerance;
}

void LineSensor::getValues(uint16_t * array){
    // calcula uma media de 10 medidas
    for(uint8_t i = 0; i < sensorCount; i++){
        int sum = 0;
        for(uint8_t j = 0; j < 10; j++){
            sum += analogRead(sensorPins[i]);
            delay(5);
        }
        array[i] = (uint8_t) sum/10; // salva a media
    }
}

void LineSensor::isValid(){
    // caso os valores de maximos e minimos nao fazerem sentido 
    // não é possivel calcular a posição da linha

    if (verb){
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
    }

    for(uint8_t i = 0; i < sensorCount; i++){
        if(minimum[i] >= maximum[i]){
            if(verb)
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
    if(verb)
        Serial.println("Medindo a itensidade da luz para o preto...");
    delay(2000);
    digitalWrite(2, LOW);
    getValues(maximum);

    // pisca o led
    delay(100);
    digitalWrite(2, HIGH);
    delay(50);
    digitalWrite(2, LOW);
    delay(1000);

    // mede o quanto está lendo na parte clara da pista
    digitalWrite(2, HIGH);
    if(verb)
        Serial.println("Medindo a itensidade da luz para o branco...");
    delay(2000);
    digitalWrite(2, LOW);
    getValues(minimum);

    // pisca o led
    delay(100);
    digitalWrite(2, HIGH);
    delay(50);
    digitalWrite(2, LOW);
    delay(100);

    // verifica se as medidas sao validas
    isValid();
}

void LineSensor::setMaxAndMinEx(){
    // salva os pontos maximos e minimos de cada sensor
    uint32_t start = millis();
    digitalWrite(2, HIGH);
    if(verb)
        Serial.println("Passe o sensor sobre a linha e o tapete enquanto o led está acesso para definir o branco e preto de cada sensor.");
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
    if(verb)
        Serial.println("Calibração começou.");

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

uint32_t LineSensor::read(uint8_t index){
    int value = analogRead(sensorPins[index]);

    if(!lineWhite){
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
    bool inLine = false;
    for(uint8_t i = 0; i < sensorCount; i++){
        int x = read(i);
        if(verb)
            Serial.printf("%d \t", x);
        sum += x * line * i+1;  // soma da media multiplicado pelo peso do sensor
        measuraments += x;      // soma das medidas
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
    if(verb)
        Serial.printf("%d \n", lastPosition);
    return lastPosition;
}