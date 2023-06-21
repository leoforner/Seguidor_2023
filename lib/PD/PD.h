// autor Alison
// email AlisonTristao@hotmail.com

#pragma once // substitui o indef endif
#include <cstdint>
#include <Arduino.h>

class PD{
    public: 
        //construtor
        PD(double kpr, double kdr, double kpt, double kdt);
        ~PD();
        // atualiza todas constantes
        void setConst(double kpr, double kdr, double kpt, double kdt); 
        void setConstPiMotors(double kple, double kile, double kpri, double kiri); 
        void setVerb(bool verb);
        /**
         * escolhemos qual modo do PID usar 
         * @param
         * Erro calculado com a biblioteca linesensor
         * @param
         * mode Rotacional ou Translacional
         */
        int32_t calcPD(uint8_t mode, int32_t error, int32_t limit);
        int32_t calcPI(uint8_t mode, int32_t error, int32_t limit);
        int simplePI1(double kp, double ki, double error, int32_t limit);
        int simplePI2(double kp, double ki, double error, int32_t limit);
        int simplePD1(double kp, double kd, int32_t error, int32_t limit);
        int simplePD2(double kp, double kd, int32_t error, int32_t limit);
        // printa as configurações
        void printConfig();
    private:
        // converte o PID para PWM - ainda precisamos pensar sobre
        int transformForPWM(int PID);
        uint32_t lastTime[4] = {0, 0, 0, 0};
        double kpt = 1, kdt = 1, kpr = 1, kdr = 1;
        double kple = 1, kile = 1, kpri = 1, kiri = 1;
        int32_t sumError[2] = {0, 0}, lastError[4] = {0, 0, 0, 0};
        bool verb = false;
};

typedef enum {
    ROTACIONAL,
    TRANSLACIONAL
} modos;

typedef enum {
    MOTOR_LEFT,
    MOTOR2_RIGHT
} motors;
