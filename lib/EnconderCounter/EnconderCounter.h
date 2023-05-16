 #include <Arduino.h>
  #include <Ticker.h>
  #include <driver/pcnt.h>
  #include "esp_task_wdt.h"
  #include "esp_task.h"
class EnconderCounter{
public:
EnconderCounter(int GPIO_PINO, pcnt_unit_t COUNTER_UNIT, unsigned long pulseForRevolution);
~EnconderCounter();
static void IRAM_ATTR pcnt_isr_handler(void *arg);

private:
//Variables
int PULSE_FOR_REVOLUTION;
unsigned long timeInterval; //ms
pcnt_unit_t COUNTER_UNIT;
int16_t PULSES;
public:
//Functions
unsigned long getRPM(unsigned long pastTime); // GET ROTATION PER MINUTE
double getRPS(unsigned long pastTime); //get ROTATION PER SECOND
int16_t getPulses();
};