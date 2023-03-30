#include <Arduino.h>
#include <lineSensor.h>
#include <PID.h>

lineSensor ls;
uint8_t pinos[8] = {39, 34, 35, 32, 33, 25, 26, 27};

PID pid;

void setup() {
    Serial.begin(115200);

    // inicia a placa de sensores
    ls.begin(8, pinos, 4095);
    ls.calibration(ESTATICO);

    // inicia o PID
    pid.begin(1.0, 1.0, 1.0, 4095);
    //pid.begin(1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 4095);
    //pid.setVerb(true);
    pid.printConfig();
}

void loop() {
    uint32_t start = millis();

    int32_t pid1 = pid.calcPID((ls.searchLine() - 3500));
    uint32_t t1 = millis()-start;

    Serial.printf("PID: %d \t Tempo: %dms \n", pid1, t1);

    /*uint32_t start = millis();

    int32_t pid1 = pid.calcPID((ls.searchLine() - 3500), ROTACIONAL);
    uint32_t t1 = millis()-start;

    int32_t pid2 = pid.calcPID(enconders, TRANSLACIONAL);
    uint32_t t2 = millis()-start;

    Serial.printf("PID rotacional: %d \t tempo: %dms \t PID translacional: %d \t tempo: %dms \n", pid1, t1, pid2, t2);*/

    delay(100);
}