#include <Arduino.h>
#include <cmath>

#define p1 15
#define p2 4

const double pi = cos(-1);
double tempo, 
       omega,
       psi, 
       distan, 
       vel, 
       vx, 
       vy, 
       v1[2], 
       v2[2], 
       v1g[2], 
       v2g[2], 
       r = 4.0, 
       p, 
       d = 3, 
       d1 = -2, 
       d2 = 2, 
       ve1,
       vd1,
       ve2,
       vd2,
       phi1, 
       phi2;

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
    psi = (2*p)/((p*p) + (d*d));
    distan = sqrt(p*p + d*d);
    omega = grausForRad(angulo)/tempo;
    vel = distan/tempo;
    vx = sin(grausForRad(angulo)) * vel;
    vy = cos(grausForRad(angulo)) * vel;

    v1[0] = vx + (d1 * omega * cos(grausForRad(angulo)));
    v1[1] = vy + (d1 * omega * sin(grausForRad(angulo)));

    v2[0] = vx + (d2 * omega * cos(grausForRad(angulo)));
    v2[1] = vy + (d2 * omega * sin(grausForRad(angulo)));

    v1g[0] = vx * (1 + (r * psi));
    v1g[1] = vy * (1 + (r * psi));

    v2g[0] = vx * (1 - (r * psi));
    v2g[1] = vy * (1 - (r * psi));

    /*Serial.printf("t: %03ds\tpeta: %03d°\tw: %.3frad/s\t (cm/s) -> vx: %.3f\tvy: %.3f\t", 
    tempo, (angulo), omega, vx, vy);

    Serial.printf("v1:{%.3f, %.3f} \t v2:{%.3f, %.3f}\tv1:{%.3f, %.3f}\tv2:{%.3f, %.3f}\n", 
    v1[0], v1[1], v2[0], v2[1], v1g[0], v1g[1], v2g[0], v2g[1]);*/

    Serial.printf("%03d\t%03d\t", tempo, (angulo));

    ve1 = sqrt(v1[0]*v1[0] + v1[1]*v1[1]);
    vd1 = sqrt(v2[0]*v2[0] + v2[1]*v2[1]);

    ve2 = sqrt(v1g[0]*v1g[0] + v1g[1]*v1g[1]);
    vd2 = sqrt(v2g[0]*v2g[0] + v2g[1]*v2g[1]);  

    Serial.printf("v1: %.3f, %.3f \tv2: %.3f, %.3f\tv1: %.3f, %.3f\tv2: %.3f, %.3f\t%.3f\t%.3f\t%.3f\t%.3f\t", 
    v1[0], v1[1], v2[0], v2[1], v1g[0], v1g[1], v2g[0], v2g[1], ve1, vd1, ve2, vd2);

    phi1 = 1.57 - atan2(r, (vd1 - ve1));
    phi2 = atan2((vd2 - ve2), r) - 1.57;

    Serial.printf("r1: %.3f\tr2: %.3f\n", ve1/sin(phi1), ve2/sin(phi2));

    delay(250);
}