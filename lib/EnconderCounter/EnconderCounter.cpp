#include "EnconderCounter.h"
//CONSTRUCTOR
EnconderCounter::EnconderCounter(int GPIO_PINO, pcnt_unit_t COUNTER_UNIT, int pulseForRevolution, int timeInterval){
    this->PULSE_FOR_REVOLUTION = pulseForRevolution;
    this->COUNTER_UNIT = COUNTER_UNIT;
    this->timeInterval = timeInterval;
    this->currentTime = 0;
    this->pastTime = 0;
      // Código da tarefa
      pcnt_config_t pcnt_config = {
      .pulse_gpio_num = GPIO_PINO,
      .lctrl_mode = PCNT_MODE_REVERSE,
      .hctrl_mode = PCNT_MODE_KEEP,
      .pos_mode = PCNT_COUNT_INC,
      .neg_mode = PCNT_COUNT_DIS,
      .counter_h_lim =  32767,
      .counter_l_lim = -32768,
      .unit = COUNTER_UNIT,
  };
  pcnt_unit_config(&pcnt_config);
  // Add a watchpoint for the pulse counter
  //ESP_ERROR_CHECK(pcnt_set_event_value(PCNT_UNIT_0,PCNT_EVT_THRES_0, 5000));
  //pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_THRES_0);
  pcnt_event_enable(COUNTER_UNIT, PCNT_EVT_H_LIM);
  pcnt_intr_enable(COUNTER_UNIT);
  // Register PCNT ISR
  pcnt_isr_register(pcnt_isr_handler, NULL, 0, NULL);
  // Start PCNT unit
  pcnt_counter_pause(COUNTER_UNIT);
  pcnt_counter_clear(COUNTER_UNIT);
  pcnt_counter_resume(COUNTER_UNIT);
}
EnconderCounter::~EnconderCounter(){}
 void IRAM_ATTR EnconderCounter::pcnt_isr_handler(void *arg){}
 
  unsigned long EnconderCounter:: getRPM()
  {
    unsigned long rpm =  getRPS() * 60;
    return rpm;
  }
  unsigned long EnconderCounter:: getRPS()
  {
 
 pcnt_get_counter_value(PCNT_UNIT_0, &PULSES);
  Serial.print("counterValue: ");
  Serial.println(PULSES);
  currentTime = millis();
  if(currentTime - pastTime >= timeInterval)
  {
    pastTime = currentTime;
    return (PULSES/PULSE_FOR_REVOLUTION) / (timeInterval/1000);
  }
  
   
  }
