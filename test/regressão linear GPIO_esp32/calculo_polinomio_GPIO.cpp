#include <Arduino.h>
#include <curveFitting.h>
#define in 33
#define out 12

double x[300]; 
double t[600];
char buf[100];
int order = 3; // polinomio de ordem 3

void setup() {
    Serial.begin(115200);
    pinMode(in, INPUT);
    pinMode(out, OUTPUT);
        
    ledcSetup(0, 5000, 12); 
    ledcAttachPin(out, 0);
}

void loop() {
    double leitura = 0;
    // descarrega o capacitor
    ledcWrite(0, 100);
    delay(1000);

    // calcula o erro subindo
    for(int j = 100; j < 300; j++){
        // aumenta de 10 em 10 o valor analogico
        leitura = 0;
        ledcWrite(0, j*10);
        delay(100);

        // calcula a media de 100 mmedidas
        for(int i = 0; i < 100; i++){
            leitura += analogRead(in);
            delayMicroseconds(20);
        } leitura /= 100.0;
        
        // copie os dados printados e cole em csv para plotar no excel com separador ';'
        Serial.printf("out =;%d;in =;%.f\n", (j*10), leitura);

        // salva os dados
        x[j] = j*10;
        t[j] = leitura;
    }

    // calcula o erro descendo 
    /*for(int j = 300; j > 100; j--){
        // aumenta de 10 em 10 o valor analogico
        leitura = 0;
        ledcWrite(0, j*10);
        delay(100);

        // calcula a media de 100 mmedidas
        for(int i = 0; i < 100; i++){
            leitura += analogRead(in);
            delayMicroseconds(20);
        } leitura /= 100.0;
        
        // copie os dados printados e cole em csv para plotar no excel com separador ';'
        Serial.printf("out =;%d;in =;%.f\n", (j*10), leitura);

        // salva os dados
        x[j] = j*10;
        t[j+300] = leitura;
    }*/

    // calcula o polinomio
    double coeffs[order+1];
    int ret = fitCurve(order, sizeof(x)/sizeof(double), t, x, sizeof(coeffs)/sizeof(double), coeffs);
    if (ret == 0){ //Returned value is 0 if no error
        uint8_t c = 'a';
        Serial.println("Coefficients are");
        for (int i = 0; i < sizeof(coeffs)/sizeof(double); i++){
        snprintf(buf, 100, "%c=",c++);
        Serial.print(buf);
        Serial.print(coeffs[i], 10);
        Serial.print('\t');
        }
    }
}