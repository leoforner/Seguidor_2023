#include <Arduino.h>

#define in 33

double converteAnalogParaTensao(int analog){
    if(analog > 1030 && analog < 2700){ // faixa precisa
        double reg = 
                0.0000000788*(pow(analog, 3)) - // x³
                0.0003832376*(pow(analog, 2)) + // x²
                1.5860951797*analog +           // x
                28.787348508;

        return reg*7/2225; // regra de 3 para converter para 7v
    }else{
        Serial.println("tensão fora da margem precisa da esp32.");
        return 0;
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(in, INPUT);
}

void loop() {
    double tensao = 0;
    // calcula a media de 100 medidas
    for(int i = 0; i < 100; i++){
        int analog = analogRead(in);
        tensao += converteAnalogParaTensao(analog);
        delay(5);
    } tensao /= 100;

    Serial.printf("Tensão da bateria: %.2f\n", tensao);
    delay(100);
}