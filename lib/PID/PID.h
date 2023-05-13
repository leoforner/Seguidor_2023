// autor Alison
// email AlisonTristao@hotmail.com

#pragma once // substitui o indef endif
#include <cstdint>
#include <Arduino.h>

class PID{
    public: 
        //construtor
        PID(double kpr, double kir, double kdr, int limit);
        PID(double kpr, double kir, double kdr,
                    double kpt, double kit, double kdt, int limit);
        ~PID();
        // inicia lib
        void begin();
        // atualiza as constantes rotacionais
        void setConst(double kpr, double kir, double kdr);
        // atualiza todas constantes
        void setConst(double kpr, double kir, double kdr,
                    double kpt, double kit, double kdt); 
        // atualiza o valor da velociade
        void setLimit(int limit);
        void setVerb(bool verb);
        // calcula o PID rotacional - default
        int32_t calcPID(int32_t error);
        /**
         * escolhemos qual modo do PID usar 
         * @param
         * Erro calculado com a biblioteca linesensor
         * @param
         * mode Rotacional ou Translacional
         */
        int32_t calcPID(int32_t error, uint8_t mode);
        int simplePI(double kp, double ki, int32_t error);
        int simplePD(double kp, double kd, int32_t error);
        // printa as configurações
        void printConfig();
    private:
        // calculo do PID geral
        int simplePID(double kp, double ki, double kd, int32_t error);
        // converte o PID para PWM - ainda precisamos pensar sobre
        int transformForPWM(int PID);
        uint32_t lastTime;
        double kpt, kit, kdt, kpr, kir, kdr;
        int32_t limit, sumError, lastError;
        bool verb;
};

typedef enum {
    ROTACIONAL,
    TRANSLACIONAL
} modos;
