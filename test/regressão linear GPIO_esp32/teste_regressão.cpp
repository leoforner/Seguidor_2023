#include <Arduino.h>

#define in 33
#define out 12
#define pot 35

void setup() {
    Serial.begin(115200);
    pinMode(in, INPUT);
    pinMode(out, OUTPUT);
    pinMode(pot, INPUT);

    ledcSetup(0, 5000, 12); 
    ledcAttachPin(out, 0);
}

void loop() {
    // le o potenciometro
    int potenciometro = analogRead(pot);
    // converte o potenciometro para tensao
    float leitura = (potenciometro*3.28)/4095;
    // calcula qual o sinal analogico correspondente para a entrada
    float saida = (leitura*4095)/3.28;

    ledcWrite(0, saida);
    int entrada = 0;
    // calcula a media do sinal recebido
    for(int i = 0; i < 10; i++){
        entrada += analogRead(in);
        delay(5);
    }entrada /= 10;

    /*
        Altere esses valores de acordo com seu resultado
    */
    // calcula o valor real
    double reg = 
            0.0000000469*(pow(entrada, 3)) - // ax³
            0.0002613563*(pow(entrada, 2)) + // bx²
            1.4905079297*entrada +           // cx
            0.1072729599;                    // d
    float tensaoV = (reg*2.4)/3095;

    // a melhor precisao esta entre 1.0 e 2.4v

    Serial.printf("saida: %.2f\tentrada: %d\t regressão: %.0f\treal: %.2f\n", leitura, entrada, reg, tensaoV);
    delay(100);
}