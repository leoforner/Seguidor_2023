#include <Arduino.h>
#include <curveFitting.h>
#define in 33
#define out 5

double x[410];
double t[410];
char buf[100];
int order = 3;

void setup() {
    Serial.begin(115200);
    pinMode(in, INPUT);
    pinMode(out, OUTPUT);
    
    ledcSetup(0, 5000, 12); // canal para esquerdo
    ledcAttachPin(out, 0);
}

void loop() {
    double leitura = 0;
    // descarrega o capacitor
    ledcWrite(0, 0);
    delay(2000);

    for(int j = 0; j < 409; j ++){
        // aumenta de 10 em 10 o valor analogico
        ledcWrite(0, j*10);

        // calcula a media de 100 mmedidas
        for(int i =0; i < 100; i++){
            leitura += analogRead(in);
            delay(10);
        } leitura /= 100;

        // printa o resutado de cada media
        delay(10);
        Serial.print("Write: ");
        Serial.print(j*10);
        Serial.print(" Read: "); 
        Serial.println(leitura, 6);

        // salva os dados
        x[j] = j*10;
        t[j] = leitura;
    }

    // calcula o polinomio
    double coeffs[order+1];
    int ret = fitCurve(order, sizeof(x)/sizeof(double), t, x, sizeof(coeffs)/sizeof(double), coeffs);
    if (ret == 0){ //Returned value is 0 if no error
        uint8_t c = 'a';
        Serial.println("Coefficients are");
        for (int i = 0; i < sizeof(coeffs)/sizeof(double); i++){
        snprintf(buf, 100, "%c=",c++);
        Serial.print(buf);
        Serial.print(coeffs[i], 6);
        Serial.print('\t');
        }
    }
}