// bibliotecas
#include <BluetoothSerial.h>
#include <lineSensor.h>
#include <Arduino.h>
#include <PD.h>

// includes
#include "pins.h"
#include "times.h"
#include "wheels.h"
#include "exceptions.h"

// //SerialBT bluetooth
BluetoothSerial SerialBT;
double position;
int32_t pid = 0;
uint32_t speed = 2000;

double kp = 10, ki = 10, kd = 10;

uint8_t state = 0;        // estados do carrinho 
// 0 desligado
// 1 calibrando
// 2 e 3 pista
// 4 final da pista

// sensores frontais (defina o carrinho em pins.h)
#ifdef FRANK
    uint8_t pins[] = {13, 12, 14, 27, 26, 25, 33, 32}, pinCount = 8;
    // {32, 33, 25, 26, 27, 14, 12, 13}
    // {13, 12, 14, 27, 26, 25, 33, 32}
#endif

#ifdef FOMINHA
    uint8_t pins[] = {35, 32, 33, 25, 26, 27, 14, 12}, pinCount = 8;
#endif

lineSensor forwardSensor(pinCount, pins, true);             

// controle do carrinho
PD control(1, 1, 1, 1);

// contadores do encoder
EncoderCounter encoderLeft(enc3, PCNT_UNIT_0, 140, 1000);
EncoderCounter encoderRight(enc1, PCNT_UNIT_1, 140, 1000);

// rodas
wheels wheelLeft, wheelRight;

// canais pwm (precisa definir para definir em pins.h)
uint8_t channelLeft = 0, channelRight = 1;

void setup(){
    Serial.begin(115200);
    definePins();
    SerialBT.begin("FOMINHA");

    // define as propriedades das rodas
    wheelLeft.enc = &encoderLeft;
    wheelLeft.mov = STOPPED;
    wheelLeft.velocity = 0.0;
    wheelLeft.l1 = bin1;
    wheelLeft.l2 = bin2;
    wheelLeft.channelPWM = channelLeft;

    wheelRight.enc = &encoderRight;
    wheelRight.mov = STOPPED;
    wheelRight.velocity = 0.0;
    wheelRight.l1 = ain1;
    wheelRight.l2 = ain2;
    wheelRight.channelPWM = channelRight;

    // aplica o pwm nos motores
    applyPWM(&wheelLeft, 0);
    applyPWM(&wheelRight, 0);

    // filtro passa baixa 
    encoderLeft.setFiltroCostant(0.01);
    encoderRight.setFiltroCostant(0.01);

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

    //SerialBT.println("Carrinho ligado, pressione o botao para iniciar calibração");
    while(state < 1) delay(10);

    //SerialBT.println("Calibrando...");
    forwardSensor.begin();
    forwardSensor.setLed(led);
    forwardSensor.calibration(DYNAMIC);

    //SerialBT.println("Sensor calibrado, pressione o botao para iniciar trajeto");
    while(state < 2) delay(10);

    // sinalização piscando led
    delay(100);
    for(uint8_t i = 0; i < 4; i++){
        digitalWrite(led, HIGH);
        delay(100);
        digitalWrite(led, LOW);
        delay(500);
    }
}

void loop(){
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

    // aplica o pwm
    applyPWM(&wheelRight, velRight); 
    applyPWM(&wheelLeft, velLeft);  

    SerialBT.printf("%.2f\n%d\t%d\t%d\n", position, pid, velRight, velLeft);

    delay(5);
}