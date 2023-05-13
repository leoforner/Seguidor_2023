#include <Arduino.h>
#include <cmath>

#define p1 15
#define p2 4

const double pi = cos(-1);
double tempo, 
       omega,
       distan, 
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

float grausForRad(int8_t graus){
    return (graus*3.14159265)/180.0;
}

void setup(){
    Serial.begin(115200);
}

void loop(){
    int8_t tempo = map(analogRead(p1), 0, 4095, 0, 100);
    int8_t angulo = map(analogRead(p2), 0, 4095, -90, 90);

    // variaveis para as contas
    p = tan(grausForRad(angulo)) * d;
    omega = grausForRad(angulo)/tempo;
    vel = d/tempo;
    vx = sin(grausForRad(angulo)) * vel;
    vy = cos(grausForRad(angulo)) * vel;

    // velocidade em XY de cada roda
    // contando que o carrinho vai girar no centro do eixo das 
    // rodas
    v1[0] = vx + (d1 * omega * cos(grausForRad(angulo)));
    v1[1] = vy + (d1 * omega * sin(grausForRad(angulo)));

    v2[0] = vx + (d2 * omega * cos(grausForRad(angulo)));
    v2[1] = vy + (d2 * omega * sin(grausForRad(angulo)));

    // contando que gira no centro de massa
    /*v1[0] = vx + (d1 * omega * cos(grausForRad(angulo))) + (dcm * omega sin(grausForRad(angulo)));
    v1[1] = vy + (d1 * omega * sin(grausForRad(angulo))) - (dcm * omega cos(grausForRad(angulo)));

    v2[0] = vx + (d2 * omega * cos(grausForRad(angulo))) + (dcm * omega sin(grausForRad(angulo)));
    v2[1] = vy + (d2 * omega * sin(grausForRad(angulo))) - (dcm * omega cos(grausForRad(angulo)));*/


    Serial.printf("%03d\t%03d\t", tempo, (angulo));

    // calcula a velocidade vetorial do centro das rodas
    ve = sqrt(v1[0]*v1[0] + v1[1]*v1[1]);
    vd = sqrt(v2[0]*v2[0] + v2[1]*v2[1]);

    // calcula o raio que o carrinho esta fazendo
    R = (r/2) * (ve + vd)/(vd - ve);
    
    Serial.printf("v1: %.3f, %.3f \tv2: %.3f, %.3f\tve: %.3f\tvd: %.3f\tR: %.3f\n", 
    v1[0], v1[1], v2[0], v2[1], ve, vd, R);

    delay(250);
}