#include "EnconderCounter.h"
#include "esp_err.h"
#include "esp_system.h"

//CONSTRUCTOR
EnconderCounter::EnconderCounter(int GPIO_PINO, pcnt_unit_t COUNTER_UNIT, unsigned long pulseForRevolution){
    this->PULSE_FOR_REVOLUTION = pulseForRevolution;
    this->COUNTER_UNIT = COUNTER_UNIT;
    this->timeInterval = timeInterval;

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

  //Filter
  //pcnt_set_filter_value(COUNTER_UNIT, 100); // o segundo argumento é a quantidade de pulsos do APB clock( frequencia do bus do esp), então o tempo é relativo a frequência do bus
  //pcnt_filter_enable(COUNTER_UNIT);
  //int apb_freq = rtc_clk_apb_freq_get();
    //printf("APB clock frequency: %d Hz\n", apb_freq)

}
  
  EnconderCounter::~EnconderCounter(){}
 void IRAM_ATTR EnconderCounter::pcnt_isr_handler(void *arg){}
 
  unsigned long EnconderCounter:: getRPM(unsigned long pastTime)
  {
    unsigned long rpm =  getRPS(pastTime) * 60;
    return rpm;
  }
  double EnconderCounter:: getRPS(unsigned long pastTime)
  {
 
  pcnt_get_counter_value(PCNT_UNIT_0, &PULSES);
  //Serial.print("counterValue: ");
  Serial.println(PULSES);
  unsigned long timeInterval = micros() - pastTime;
  if(timeInterval <= 0)
  {
    Serial.println("Time interval is <= 0");
    return 0;
  }
  else{
    //Serial.printf("timeI Interval %d \n", timeInterval);
    //Serial.printf("pulse cast %d \n", PULSES);
    //Serial.print("RPS: ");
    double t = ((PULSES*1.0) /(PULSE_FOR_REVOLUTION*1.0)) / (timeInterval/1e6);
   // Serial.printf("RPS %f \n", t);
    return t;
  }
  }
