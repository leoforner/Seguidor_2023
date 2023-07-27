// bibliotecas
#include <BluetoothSerial.h>
#include <lineSensor.h>
#include <Arduino.h>
#include <PD.h>

// includes
#include "adc.h"
#include "pins.h"
#include "times.h"
#include "wheels.h"
#include "exceptions.h"

// SerialBT bluetooth
BluetoothSerial SerialBT;
double position;
int32_t pid = 0;
uint32_t speed = 3500;

// valores maximos dos sensores laterais
uint16_t whiteLeft = 0, whiteRight = 0;

// constantes do PID
double kp = 100, ki = 2, kd = 60;

stt state = OFF;

// sensores frontais (defina o carrinho em pins.h)
#ifdef FRANK
    uint8_t pins[] = {13, 12, 14, 27, 26, 25, 33, 32}, pinCount = 8;
    // {32, 33, 25, 26, 27, 14, 12, 13}
    // {13, 12, 14, 27, 26, 25, 33, 32}
#endif

#ifdef FOMINHA
    uint8_t pins[] = {35, 32, 33, 25, 26, 27, 14, 12}, pinCount = 8;
#endif

// sensor frontal
lineSensor forwardSensor(pinCount, pins, true);             

// controle do carrinho
PD control(1, 1, 1, 1);

// rodas
wheels wheelLeft, wheelRight;

// canais pwm (precisa definir para definir em pins.h)
uint8_t channelLeft = 0, channelRight = 1;

void setup(){
    Serial.begin(115200);
    definePins();
    SerialBT.begin("FOMINHA");

    // define as propriedades das rodas
    wheelLeft.mov = STOPPED;
    wheelLeft.l1 = bin1;
    wheelLeft.l2 = bin2;
    wheelLeft.channelPWM = channelLeft;

    wheelRight.mov = STOPPED;
    wheelRight.l1 = ain1;
    wheelRight.l2 = ain2;
    wheelRight.channelPWM = channelRight;

    // aplica o pwm nos motores
    applyPWM(&wheelLeft, 0);
    applyPWM(&wheelRight, 0);

    // anexa as interrupcoes ao segundo nucleo
    xTaskCreatePinnedToCore(
        interrupt,
        "interrupt",
        2048,
        NULL,
        2,
        NULL,
        0
    );

    // espera algum dispositivo conectar
    while(state < CONNECT) {
        for(uint8_t i = 0; i < 4; i++){
            digitalWrite(led, HIGH);
            delay(100);
            digitalWrite(led, LOW);
            delay(100);
        }
        delay(1000);
    }

    //SerialBT.println("Carrinho ligado, pressione o botao para iniciar calibração do sensor frontal");
    while(state < 1) delay(10);

    // calibra o sensor frontal
    SerialBT.println("Calibrando...");
    forwardSensor.begin();
    forwardSensor.setLed(led);
    forwardSensor.calibration(STATIC);

    SerialBT.println("Pressione o botao para iniciar calibração dos sensores laterais");
    while(state < 2) delay(10);

    // calibra os sensores laterias
    uint32_t time = millis(); 
    while((millis() - time) < 1500){
        if(analogRead(left) > whiteLeft)   whiteLeft = analogRead(left);
        if(analogRead(right) > whiteRight) whiteRight = analogRead(right);  
    }digitalWrite(led, LOW);

    SerialBT.println("Sensores calibrados, pressione o botao para iniciar trajeto");
    while(state < 3) delay(10);

    // debug
    SerialBT.printf("kp: %.3f\nki: %.3f\nkd: %.3f\nspeed: %d\n", kp, ki, kd, speed);

    // sinalização piscando led
    for(uint8_t i = 0; i < 4; i++){
        digitalWrite(led, HIGH);
        delay(100);
        digitalWrite(led, LOW);
        delay(500);
    }
}

void loop(){
    // final da pista
    if(state == FINAL || state == OFF){
        // para
        brake(&wheelLeft);
        brake(&wheelRight);
        return;
    }
    // calcula a posição da linha
    position = (forwardSensor.searchLine(&state) - 3500)/100;

    // calcula o pid
    pid = control.simplePID(kp, ki, kd, position, 2*speed);

    // velocidade das rodas
    int16_t velLeft = speed;
    int16_t velRight = speed;

    // aplica nas rodas
    if(pid > 0) velRight = speed - pid;
    else        velLeft = speed + pid;

    // calcula pwm max (correspondente a 6v)
    float tensaoBateria = (analogicoParaTensao(analogRead(divTensao)))*7.6/1.92; //7.6v viram 1.92v (divisor de tensão)
    if(tensaoBateria < 7.3) state == OFF; // desliga
    int pwm_6volts = (6.0*4095)/tensaoBateria;
    if(pwm_6volts > 4095) pwm_6volts = 4095;

    // pwm correspondente a 6v
    velRight = map(velRight, -speed, speed, -pwm_6volts, pwm_6volts);
    velLeft = map(velLeft, -speed, speed, -pwm_6volts, pwm_6volts);

    // aplica o pwm
    applyPWM(&wheelRight, velRight); 
    applyPWM(&wheelLeft, velLeft);  
    
    // debug
    //SerialBT.printf("p: %.2f\t - PID: %d\t - pwm1: %d\t - pwm2: %d\n", position, pid, velRight, velLeft);

    delay(5);
}