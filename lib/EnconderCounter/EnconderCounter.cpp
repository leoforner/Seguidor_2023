#include "EnconderCounter.h"
#include "esp_err.h"
#include "esp_system.h"

//CONSTRUCTOR
EnconderCounter::EnconderCounter(int GPIO_PINO, pcnt_unit_t COUNTER_UNIT, unsigned long pulseForRevolution, uint16_t filterTime){
    this->PULSE_FOR_REVOLUTION = pulseForRevolution;
    this->COUNTER_UNIT = COUNTER_UNIT;
    this->timeInterval = timeInterval;
    this->filterTime = filterTime;
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
Serial.begin(115200);
  //Filter
  //uint16_t t = 1000;//convert_microsec_to_APB();
  //Serial.print("APB cycles: " );
  //Serial.println(t);
  pcnt_set_filter_value(COUNTER_UNIT, 1023); // o segundo argumento é a quantidade de pulsos do APB clock( frequencia do bus do esp), então o tempo é relativo a frequência do bus
  pcnt_filter_enable(COUNTER_UNIT); // The APB_CLK clock is running at 80 MHz

 
  

}
EnconderCounter::~EnconderCounter(){}
void IRAM_ATTR EnconderCounter::pcnt_isr_handler(void *arg){}

double EnconderCounter:: getRPM(unsigned long pastTime)
{
  double rpm =  getRPS(pastTime) * 60;
  return rpm;
}
uint16_t EnconderCounter:: convert_microsec_to_APB(uint16_t time)  
{
 //converte time para micro e depois multiplica por 80Mhz que é a frequencia do clock APB
 uint16_t v = (time/1000) * 80000000; 
 if(v>1023) v = 1023; //o valor máximo v é 1023 pois filter_val é um valor de 10-bit (1.27ms)
  return v;
}  
double EnconderCounter:: getRPS(unsigned long pastTime)
{

pcnt_get_counter_value(COUNTER_UNIT, &PULSES);
//Serial.print("counterValue: ");
//Serial.println(PULSES);
unsigned long timeInterval = micros() - pastTime;
if(timeInterval <= 0)
{
  Serial.println("Time interval is <= 0");
  return 0;
}
else{
  //Serial.printf("timeI Interval %d \n", timeInterval);
  //Serial.printf("pulses %d \n", PULSES);
  //Serial.print("RPS: ");
  pcnt_counter_pause(COUNTER_UNIT);
  pcnt_counter_clear(COUNTER_UNIT);
  pcnt_counter_resume(COUNTER_UNIT);
  double t = ((PULSES*1.0) /(PULSE_FOR_REVOLUTION*1.0)) / (timeInterval/1e6);
   //Serial.printf("RPS %f \n", t);
  return t;
}
}
int16_t EnconderCounter::getPulses(){
  pcnt_get_counter_value(COUNTER_UNIT, &PULSES);
  return PULSES;
}
double EnconderCounter:: getRadiansVelocity(unsigned long pastTime) 
{
  return getRPS(pastTime) * 2 * PI;
}