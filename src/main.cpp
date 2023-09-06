// bibliotecas
#include <BluetoothSerial.h>
#include <lineSensor.h>
#include <Arduino.h>

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
    float pesos[8]; 
#endif

// sensor frontal
lineSensor forwardSensor(pinCount, pins, true);             

// rodas
wheels wheelLeft, wheelRight;

// canais pwm (precisa definir aqui para definir em pins.h)
uint8_t channelLeft = 0, channelRight = 1;

EncoderCounter encLeft(enc1);

uint32_t    lastTime1 = 0,
            lastTime2 = 0;;

double  tPID = 0, 
        rPID = 0, 
        lastInt1 = 0, 
        lastInt2 = 0, 
        lastError1 = 0, 
        lastError2 = 0;

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

void transPID(double kp, double ki, double kd, double error){

    // tempo passado
    uint32_t deltaTime = (millis() - lastTime1)/1000;

    // proporcional
    double propo = kp * error * deltaTime;

    // intergral
    double inter = lastInt1;
    if(abs(lastInt1) < speed/2){   
        lastInt1 += (ki * error * deltaTime);
        inter = lastInt1;
    }

    // derivativa
    double deriv = kd * (error - lastError1)/deltaTime;

    // soma tudo
    tPID += abs(propo + inter + deriv) > speed ? 0 : propo + inter + deriv;
    
    // atualizo as variaveis globais
    lastError1 = error;
    lastTime1 = millis();
}

void transPID(double kp, double ki, double kd, double error){

    // tempo passado
    uint32_t deltaTime = (millis() - lastTime2)/1000;

    // proporcional
    double propo = kp * error * deltaTime;

    // intergral
    double inter = lastInt2;
    if(abs(lastInt2) < speed/2){   
        lastInt2 += (ki * error * deltaTime);
        inter = lastInt2;
    }

    // derivativa
    double deriv = kd * (error - lastError2)/deltaTime;

    // soma tudo
    rPID += abs(propo + inter + deriv) > speed ? 0 : propo + inter + deriv;
    
    // atualizo as variaveis globais
    lastError2 = error;
    lastTime2 = millis();
}

void transPID(double kp, double ki, double kd, double error){

    uint32_t deltaTime = (millis() - lastTime1)/1000;

    double propo = kp * error * deltaTime;

    double inter = lastInt1;
    if(abs(lastInt1) < speed/2){   
        lastInt1 += (ki * error * deltaTime);
        inter = lastInt1;
    }

    double deriv = kd * (error - lastError1)/deltaTime;

    tPID += abs(propo + inter + deriv) > speed ? 0 : propo + inter + deriv;
    
    lastError1 = error;
    lastTime1 = millis();
}

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

    for(int i = 0; i < 8; i++)
        pesos[i] = i * 5.7/7;
    forwardSensor.setweights(pesos);
    forwardSensor.setTrackCharacteristics(100, 0, 30);

    /*
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

    // sinalização piscando led para começar a pista
    for(uint8_t i = 0; i < 4; i++){
        digitalWrite(led, HIGH);
        delay(100);
        digitalWrite(led, LOW);
        delay(500);
    }*/
}

void loop(){
    double // distancia entre o sensor e a linha em cm
    p = (forwardSensor.searchLine()/100) - 5.7/2;

    // tempo para calcular velocidades
    int8_t tempo = 1;

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

    double vm = 
}