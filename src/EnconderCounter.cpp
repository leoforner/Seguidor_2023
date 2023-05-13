#include "PulseCounter.h"
//CONSTRUCTOR
PulseCounter::PulseCounter(){
      // Código da tarefa
      pcnt_config_t pcnt_config = {
      .pulse_gpio_num = 23,
      .lctrl_mode = PCNT_MODE_REVERSE,
      .hctrl_mode = PCNT_MODE_KEEP,
      .pos_mode = PCNT_COUNT_INC,
      .neg_mode = PCNT_COUNT_DIS,
      .counter_h_lim =  32767,
      .counter_l_lim = -32768,
      .unit = PCNT_UNIT_0,
  };
  pcnt_unit_config(&pcnt_config);

  // Add a watchpoint for the pulse counter
  //ESP_ERROR_CHECK(pcnt_set_event_value(PCNT_UNIT_0,PCNT_EVT_THRES_0, 5000));
  //pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_THRES_0);
  pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_H_LIM);
  pcnt_intr_enable(PCNT_UNIT_0);
  
  // Register PCNT ISR
  pcnt_isr_register(pcnt_isr_handler, NULL, 0, NULL);


  // Start PCNT unit
  pcnt_counter_pause(PCNT_UNIT_0);
  pcnt_counter_clear(PCNT_UNIT_0);
  pcnt_counter_resume(PCNT_UNIT_0);
}

 //void IRAM_ATTR PulseCounter::pcnt_isr_handler(void *arg){}