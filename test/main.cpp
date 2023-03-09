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
       d = 3, 
       d1 = -2, 
       d2 = 2, 
       ve,
       vd;

float grausForRad(int8_t graus){
    return (graus*3.14159265)/180.0;
}

void setup(){
    Serial.begin(115200);
}

void loop(){
    int8_t tempo = map(analogRead(p1), 0, 4095, 0, 100);
    int8_t angulo = map(analogRead(p2), 0, 4095, -90, 90);

    p = tan(grausForRad(angulo)) * d;
    omega = grausForRad(angulo)/tempo;
    vel = d/tempo;
    vx = sin(grausForRad(angulo)) * vel;
    vy = cos(grausForRad(angulo)) * vel;

    v1[0] = vx + (d1 * omega * cos(grausForRad(angulo)));
    v1[1] = vy + (d1 * omega * sin(grausForRad(angulo)));

    v2[0] = vx + (d2 * omega * cos(grausForRad(angulo)));
    v2[1] = vy + (d2 * omega * sin(grausForRad(angulo)));

    /*Serial.printf("t: %03ds\tpeta: %03d°\tw: %.3frad/s\t (cm/s) -> vx: %.3f\tvy: %.3f\t", 
    tempo, (angulo), omega, vx, vy);

    Serial.printf("v1:{%.3f, %.3f} \t v2:{%.3f, %.3f}\tv1:{%.3f, %.3f}\tv2:{%.3f, %.3f}\n", 
    v1[0], v1[1], v2[0], v2[1], v1g[0], v1g[1], v2g[0], v2g[1]);*/

    Serial.printf("%03d\t%03d\t", tempo, (angulo));

    ve = sqrt(v1[0]*v1[0] + v1[1]*v1[1]);
    vd = sqrt(v2[0]*v2[0] + v2[1]*v2[1]);
    
    Serial.printf("v1: %.3f, %.3f \tv2: %.3f, %.3f\tve: %.3f\tvd: %.3f\n", 
    v1[0], v1[1], v2[0], v2[1], ve, vd);

    delay(250);
}