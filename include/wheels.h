#ifndef WHEELS_H
#define WHEELS_H

#include <EncoderCounter.h>

// sentido da rotação das todas
enum movement {
    FORWARD,
    BACKWARD,
    STOPPED
};

// roda objeto
struct wheels {
    movement mov;
    float velocity;
    EncoderCounter* enc;
    uint8_t l1;
    uint8_t l2;
    uint8_t channelPWM;
};

// define a rotação da roda 
void changeDirection(wheels * wheel, movement mov){
    wheel->enc->limpaCounter();
    wheel->mov = mov;

    // define a rotação
    switch (wheel->mov){
    case FORWARD:
        digitalWrite(wheel->l1, HIGH);
        digitalWrite(wheel->l2, LOW);
        break;

    case BACKWARD:
        digitalWrite(wheel->l1, LOW);
        digitalWrite(wheel->l2, HIGH);
        break;

    case STOPPED:
        digitalWrite(wheel->l1, LOW);
        digitalWrite(wheel->l2, LOW);
        break;
    
    default:
        break;
    }
    //wheel.velocity = wheel.enc->getRPS();
}

void applyPWM(wheels * wheel, int32_t pwm){
    // define a direção
    movement mov = STOPPED;
    if(pwm > 0) mov = FORWARD;
    if(pwm < 0) mov = BACKWARD;

    // muda a direção
    if(wheel->mov != mov) changeDirection(wheel, mov);

    // aplica o pwm
    ledcWrite(wheel->channelPWM, abs(pwm));
}

void getVelocity(wheels * wheel){
    wheel->velocity = wheel->enc->getRPS();
}

#endif