  #include <Arduino.h>
  #include <Ticker.h>
  #include <driver/pcnt.h>
  #include "esp_task_wdt.h"
  #include "esp_task.h"
class EncoderCounter{
public:
EncoderCounter(int GPIO_PINO, pcnt_unit_t COUNTER_UNIT, unsigned long pulseForRevolution, uint16_t filterTime=36);
~EncoderCounter();
static void IRAM_ATTR pcnt_isr_handler(void *arg);

private:
//Variables
const double Pi = 3.14159265359; 
int PULSE_FOR_REVOLUTION;
//uint32_t timeInterval; //ms
pcnt_unit_t COUNTER_UNIT;
int16_t PULSES=0;
uint32_t filterTime;
//int16_t oldPulses=0;
uint32_t pastTime=0;
double currentVelocity = 0;
double h1 = 0.0, h2=0.0, r=0, a=0, b=0;

public:
//Functions
double getRPM(); // GET ROTATION PER MINUTE
double getRPS(); //get ROTATION PER SECOND
double getRadiansVelocity();
int16_t getPulses();
double filtro(double x);
void setFiltroCostant(double r);
void limpaCounter();
};