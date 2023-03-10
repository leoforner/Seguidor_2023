#include <Arduino.h>
#include <LineSensor.h>
#include <cmath>

#define p2 4

LineSensor ls;
uint8_t pinos[3] = {5, 18, 19};
float pesos[8]; 

double tempo, 
       omega,
       distan,
       peta, 
       vel, 
       vx, 
       vy, 
       v1[2], 
       v2[2], 
       r = 4.0, 
       p, 
       d = 10, 
       d1 = -5, 
       d2 = 5, 
       ve,
       vd,
       R;


void setup(){
    Serial.begin(115200);
    ls.beginMultiplex(3, pinos, 15, true);

    //ls.setVerb(true);

    for(int i = 0; i < 8; i++)
        pesos[i] = i * 5.7/7;

    // alteranis o peso padrao para calcular a distancia da linha
    ls.setweights(pesos);

    ls.setTrackCharacteristics(100, 0, 30);
    ls.calibration(ESTATICO);
    ls.printConfig();
}

void loop(){
    // distancia entre o sensor e a linha
    p = ls.searchLine() - (5.7/2);

    // angulo entre o eixo do carrinho e a linha
    //peta = 

    delay(100);
}