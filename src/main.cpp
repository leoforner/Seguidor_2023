#include <Arduino.h>

#define in 12
#define out 33

void setup(){
    Serial.begin(115200);
    pinMode(in, INPUT);
    pinMode(out, OUTPUT);

    ledcSetup(0, 5000, 12); // canal para esquerdo
    ledcAttachPin(out, 0);
}

void loop(){
    double leitura = 0;

    /* TESTES
    
    ledcWrite(0, 1500);

    // calcula a media de 100 mmedidas
    for(int i =0; i < 100; i++){
        leitura += analogRead(in);
        delay(10);
    } leitura /= 100;

    leitura = pow(leitura, 2) * 0.000038 + 1.044 * leitura + 127;

    Serial.print(leitura);
    Serial.print(" - ");
    
    leitura = (leitura * 3)/3850;
    Serial.println(leitura);/*/

    delay(100);
}