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
uint32_t speed = 2800;

// valores maximos dos sensores laterais
uint16_t whiteLeft = 0, whiteRight = 0;

uint8_t contadorIntersec = 0;

// constantes do PID
double kp = 400, ki = 10, kd = 45;

// inicia o estado do carrinho como off
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

// controle do carrinho (usamos apenas simplePID)
PD control(1, 1, 1, 1);

// rodas
wheels wheelLeft, wheelRight;

// canais pwm (precisa definir aqui para definir em pins.h)
uint8_t channelLeft = 0, channelRight = 1;

//
int sinalizacao_atual = 0;
bool printFinal = false;

void setup(){
    Serial.begin(115200);
    definePins();
    SerialBT.begin("FOMINHA");

    // define as propriedades das rodas
    wheelLeft.mov = STOPPED;    // inicia como parada
    wheelLeft.l1 = bin1;
    wheelLeft.l2 = bin2;
    wheelLeft.channelPWM = channelLeft;

    wheelRight.mov = STOPPED;   // inicia como parada
    wheelRight.l1 = ain1;
    wheelRight.l2 = ain2;
    wheelRight.channelPWM = channelRight;

    // manda pwm zero nos motores (quando a esp reseta estava salvando o ultimo pwm aplicado)
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

    SerialBT.println("Carrinho ligado, pressione o botao para iniciar calibração do sensor frontal");
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
    }
    
    digitalWrite(led, LOW);

    SerialBT.println("Sensores calibrados, pressione o botao para iniciar trajeto");
    while(state < 3) delay(10);

    // debug
    SerialBT.printf("kp: %.3f\nki: %.3f\nkd: %.3f\nspeed: %d\n", kp, ki, kd, speed);
    SerialBT.println((analogicoParaTensao(analogRead(divTensao)))*3.96); // 7.6/1.92 7.6v viram 1.92v (divisor de tensão));

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
        // freia as rodas para parar inercia do carrinho
      //  brake(&wheelLeft);
      //  brake(&wheelRight);
      //  if(!printFinal)
     //   {
       // printFinal = true;
        SerialBT.println(sinalizacao_atual);
        state = PISTA;
        sinalizacao_atual = 0;

     //   }
       // return;
    }

    // calcula a posição da linha (pinCout * 1000)/2 = 3500 (index 0 nao soma em search line)
    position = (forwardSensor.searchLine(&state) - 3500)/100;

    // calcula o pid
    pid = control.simplePID(kp, ki, kd, position, 2*speed);

    // velocidade toral das rodas
    int16_t velLeft = speed;
    int16_t velRight = speed;

    // aplica o pid nas rodas
    if(pid > 0) velRight = speed - pid;
    else        velLeft = speed + pid;

    /*acabamos tirando pois estava diminuindo o torque das rodas e 
    / o robo nao conseguia fazer curvas fechadas apos retas longas*/

    // calcula pwm max (correspondente a 6v)
    /*float tensaoBateria = (analogicoParaTensao(analogRead(divTensao)))* 3.96; // 7.6/1.92; //7.6v viram 1.92v (divisor de tensão)
    if(tensaoBateria < 7.7) state == OFF; // desliga
    int pwm_6volts = (8.0*4095)/tensaoBateria;
    // if(pwm_6volts > 4095) pwm_6volts = 4095; (na fonte de bancada para testar, use isso)

    // pwm correspondente ao ponto que a roda nao gira
    uint8_t pMorto = 0.2; // 20% do pwm maximo

    // mapeia o pwm resultante para a faixa de pwm q queremos 
    if(velRight >= 0)   velRight = map(velRight, 0, speed, pMorto*pwm_6volts, pwm_6volts);
    else                velRight = map(velRight, -speed, 0, -pwm_6volts, -pMorto*pwm_6volts);

    if(velLeft >= 0)    velLeft = map(velLeft, 0, speed, pMorto*pwm_6volts, pwm_6volts);
    else                velLeft = map(velLeft, -speed, 0, -pwm_6volts, pMorto*pwm_6volts*-1);*/


    // aplica o pwm
    applyPWM(&wheelRight, velRight);
    applyPWM(&wheelLeft, velLeft);  

    delay(2);
}