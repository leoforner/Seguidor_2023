// autor Alison
// email AlisonTristao@hotmail.com

//#pragma once // substitui o indef endif
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
        int32_t simplePID(double kp, double ki, double kd, double error, int32_t limit);
        int32_t calcPD(uint8_t mode, int32_t error, int32_t limit);
        int32_t calcPI(uint8_t mode, int32_t error, int32_t limit);
        // printa as configurações
        void printConfig();
    private:
        int32_t simplePD1(double kp, double kd, int32_t error, int32_t limit);
        int32_t simplePD2(double kp, double kd, int32_t error, int32_t limit);
        int32_t rightPI(double kp, double ki, double error, int32_t limit);
        int32_t leftPI(double kp, double ki, double error, int32_t limit);
        uint32_t lastTime[5] = {0, 0, 0, 0, 0};
        double kpt = 1, kdt = 1, kpr = 1, kdr = 1;
        double kple = 1, kile = 1, kpri = 1, kiri = 1;
        double lastError[5] = {0, 0, 0, 0, 0}, I_ant = 0;
        bool verb = false;
        int32_t pid, pi1 = 0, pi2 = 0, pd1 = 0, pd2 = 0;
};

typedef enum {
    ROTACIONAL,
    TRANSLACIONAL
} modos;

typedef enum {
    LEFT,
    RIGHT
} motors;
