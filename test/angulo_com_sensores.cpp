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
       d = 5, 
       d1 = -2.5, 
       d2 = 2.5, 
       ve,
       vd,
       R;


void setup(){
    Serial.begin(115200);
    ls.beginMultiplex(8, 3, pinos, 15, true);

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
    // distancia entre o sensor e a linha em cm
    p = (ls.searchLine()/100) - 5.7/2;

    // tempo para calcular velocidades
    int8_t tempo = map(analogRead(p2), 0, 4095, 0, 100);

    // angulo entre o eixo do carrinho e a linha
    peta = atan2(p, d);

    //Serial.printf("p: %.2f\tpeta: %.2f\n", p, peta);

    omega = peta/tempo;
    vel = d/tempo;
    vx = sin(peta) * vel;
    vy = cos(peta) * vel;

    // velocidade em XY de cada roda
    v1[0] = vx + (d1 * omega * cos(peta));
    v1[1] = vy + (d1 * omega * sin(peta));

    v2[0] = vx + (d2 * omega * cos(peta));
    v2[1] = vy + (d2 * omega * sin(peta));

    Serial.printf("%03d\t%.3f\t", tempo, peta);

    // calcula a velocidade vetorial do centro das rodas
    ve = sqrt(v1[0]*v1[0] + v1[1]*v1[1]);
    vd = sqrt(v2[0]*v2[0] + v2[1]*v2[1]);

    // calcula o raio que o carrinho esta fazendo
    R = (r/2) * (ve + vd)/(vd - ve);
    
    Serial.printf("v1: %.3f, %.3f \tv2: %.3f, %.3f\tve: %.3f\tvd: %.3f\tR: %.3f\n", 
    v1[0], v1[1], v2[0], v2[1], ve, vd, R);

    delay(100);
}