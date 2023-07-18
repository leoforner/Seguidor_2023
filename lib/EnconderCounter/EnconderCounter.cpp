#include "EnconderCounter.h"
#include "esp_err.h"
#include "esp_system.h"

//CONSTRUCTOR
EnconderCounter::EnconderCounter(int GPIO_PINO, pcnt_unit_t COUNTER_UNIT, unsigned long pulseForRevolution, uint16_t filterTime){
    this->PULSE_FOR_REVOLUTION = pulseForRevolution;
    this->COUNTER_UNIT = COUNTER_UNIT;
    this->filterTime = filterTime;
      // Código da tarefa
      pcnt_config_t pcnt_config = {
      .pulse_gpio_num = GPIO_PINO,
      .lctrl_mode = PCNT_MODE_REVERSE,
      .hctrl_mode = PCNT_MODE_KEEP,
      .pos_mode = PCNT_COUNT_INC,
      .neg_mode = PCNT_COUNT_INC,
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
  pcnt_set_filter_value(COUNTER_UNIT, 36); // o segundo argumento é a quantidade de pulsos do APB clock( frequencia do bus do esp), então o tempo é relativo a frequência do bus
  pcnt_filter_enable(COUNTER_UNIT); // The APB_CLK clock is running at 80 MHz

this->setFiltroCostant(0.4); //constant padrão caso nenhuma seja definida 
}
EnconderCounter::~EnconderCounter(){}
void IRAM_ATTR EnconderCounter::pcnt_isr_handler(void *arg){}

double EnconderCounter:: getRPM()
{
  double rpm =  getRPS() * 60;
  return rpm;
}
uint16_t EnconderCounter:: convert_microsec_to_APB(uint16_t time)  
{
 //converte time para micro e depois multiplica por 80Mhz que é a frequencia do clock APB
 uint16_t v = (time/1000) * 80000000; 
 if(v>1023) v = 1023; //o valor máximo v é 1023 pois filter_val é um valor de 10-bit (1.27ms)
  return v;
}  
double EnconderCounter:: getRPS()
{
uint32_t now = micros();
uint32_t timeInterval = now - pastTime;
pcnt_get_counter_value(COUNTER_UNIT, &PULSES);
  if(PULSES <=0)
  {
    if(now - pastTime >= 30)
    {
      currentVelocity = 0;  //se já faz mais de X tempo desde a ultima vez que eu medi a velocidade e ainda tá dando 0 pulso, então provavelmente o carrinho tá parado
    }
     return currentVelocity;
  }
  else if (timeInterval <= 0){
    return currentVelocity;
    }
  else{
    currentVelocity = ((PULSES*1.0) /(PULSE_FOR_REVOLUTION*1.0)) / (timeInterval/1e6);
    pastTime = micros();
    pcnt_counter_pause(COUNTER_UNIT);
    pcnt_counter_clear(COUNTER_UNIT);
    pcnt_counter_resume(COUNTER_UNIT);
    //Serial.printf("RPS %f \n", t);
    return filtro(currentVelocity);
    }
} 



int16_t EnconderCounter::getPulses(){
  pcnt_get_counter_value(COUNTER_UNIT, &PULSES);
  return PULSES;
}
double EnconderCounter:: getRadiansVelocity() 
{
  return getRPS() * 2 * PI;
}
double EnconderCounter:: filtro(double x){
  double yn = x - (a * h1) - (b * h2);

  h1 = yn;

  h2 = h1;

  yn /= 1/(1 + a + b);

  return yn;
}

void EnconderCounter:: setFiltroCostant(double r){
 this->r = r;
 double a = -2.0*r;
 double b = r*r; 
}