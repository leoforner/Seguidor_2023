 #include <Arduino.h>
  #include <Ticker.h>
  #include <driver/pcnt.h>
  #include "esp_task_wdt.h"
  #include "esp_task.h"
class EnconderCounter{
public:
EnconderCounter(int GPIO_PINO, pcnt_unit_t COUNTER_UNIT, unsigned long pulseForRevolution, uint16_t filterTime=0);
~EnconderCounter();
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
public:
//Functions
double getRPM(); // GET ROTATION PER MINUTE
double getRPS(); //get ROTATION PER SECOND
double getRadiansVelocity();
int16_t getPulses();
uint16_t convert_microsec_to_APB(uint16_t time); //pega a velocidade radial por segundos
};