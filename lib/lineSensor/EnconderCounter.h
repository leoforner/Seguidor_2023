 #include <Arduino.h>
  #include <Ticker.h>
  #include <driver/pcnt.h>
  #include "esp_task_wdt.h"
  #include "esp_task.h"
class PulseCounter{
public:
PulseCounter(int GPIO_PINO, pcnt_unit_t COUNTER_UNIT, int pulseForRevolution, int timeInterval);
~PulseCounter();
static void IRAM_ATTR pcnt_isr_handler(void *arg){}

private:
//Variables
int PULSE_FOR_REVOLUTION;
unsigned long currentTime;
unsigned long pastTime;
unsigned long timeInterval; //ms
pcnt_unit_t COUNTER_UNIT;
int16_t PULSES;
//Functions
unsigned long getRPM();
unsigned long getRPS();
};