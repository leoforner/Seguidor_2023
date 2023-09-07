// bibliotecas
#include <BluetoothSerial.h>
#include <lineSensor.h>
#include <mathModel.h>
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
uint16_t whiteLeft = 50000, whiteRight = 50000;

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

EncoderCounter encoderLeft(enc1, PCNT_UNIT_0, 140, 1000);
EncoderCounter encoderRight(enc3, PCNT_UNIT_1, 140, 1000);

uint32_t    lastTime1 = 0,
            lastTime2 = 0;;

double  tPID = 0, 
        rPID = 0, 
        lastInt1 = 0, 
        lastInt2 = 0, 
        lastError1 = 0, 
        lastError2 = 0;

                            // X    Y
double carVector[3][2] =   {{-6.7, -2.5},    // roda direita
                            {+6.7, -2.5},    // roda esquerda
                            {+0.0, +7.0}};   // linha de sensores
double wheelsRadius = 1.5, actingTime = 10;
double * wheelsSpeed = new double[2];
mathModel mm(carVector, wheelsRadius, actingTime, wheelsSpeed); 

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

void rotaPID(double kp, double ki, double kd, double error){

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

    // a linha tem 5.7 centimetros com 8 sensores
    for(int i = 0; i < 8; i++)
        pesos[i] = i * 5.7/7; 
    // altera o peso padrao para calcular a distancia da linha
    forwardSensor.setweights(pesos);

    // dessa forma a saida esta em nanometro
    // divindo por 100 a medida passa para centimetros
    forwardSensor.setTrackCharacteristics(100, 0, 30);

    
    forwardSensor.calibration(STATIC);

    /*
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
    // distancia entre o sensor e a linha em cm
    double position = (forwardSensor.searchLine()/100) - 5.7/2;

    // velocidade media
    double vm = (encoderLeft.getRPS() + encoderRight.getRPS())/2;

    // velocidade de rotação
    double rot = (encoderLeft.getRPS() + encoderRight.getRPS())/(2*6.7);
    
    // calcula o setPoint de cada roda em cm/s
    mm.calculateSetPoints(position);

    double setPointVM = (wheelsSpeed[0] + wheelsSpeed[1])/2;
    double setPointROT = (wheelsSpeed[0] + wheelsSpeed[1])/(2*6.7);

    transPID(2, 2, 2, setPointVM - vm);
    rotaPID(2, 2, 2, setPointROT - rot);

    applyPWM(&wheelLeft, tPID + rPID);
    applyPWM(&wheelRight, tPID - rPID);
    delay(10);
}