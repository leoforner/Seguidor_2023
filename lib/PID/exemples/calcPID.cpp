#include <Arduino.h>
#include <lineSensor.h>
#include <PID.h>

lineSensor ls;
uint8_t pinos[8] = {34, 35, 32, 33, 25, 26, 27, 14};

PID pid;

void setup() {
    Serial.begin(115200);

    // inicia a placa de sensores
    ls.begin(8, pinos, true);
    ls.calibration(STATIC);

    // inicia o PID
    pid.begin(1.0, 1.0, 1.0, 4095);
    // pode definir variaveis para 2 PIDs
    //pid.begin(1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 4095);
    //pid.setVerb(true);
    pid.printConfig();
}

void loop() {
    uint32_t start = micros(), t1, t2;

    uint32_t erro = ls.searchLine() - 3500;
    t1 = micros()-start;

    start = micros();
    int32_t pid1 = pid.calcPID(erro);
    t2 = micros()-start;

    Serial.printf("PID: %d \t Tempo: %d micros\n", pid1, t2);

    /*
    caso queira usar os 2 PID definidos basta selecionar qual o modo

    int32_t pid1 = pid.calcPID(erro, ROTACIONAL);

    int32_t pid2 = pid.calcPID(enconders, TRANSLACIONAL);

    Serial.printf("PID rotacional: %d\t PID translacional: %d\n", pid1, pid2);*/

    /*
    pode tambem calcular um PI e um PD simples passando o erro e as constantes assim:

    pid1 = pid.simplePD(kp, kp, erro);
    pid1 = pid.simplePI(kp, ki, erro);
    */

    delay(100);
}