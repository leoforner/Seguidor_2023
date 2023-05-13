#include <Arduino.h>
#include <Ticker.h>
#include <driver/pcnt.h>
#include "esp_task_wdt.h"
#include "esp_task.h"
// Define PCNT ISR
bool flag = false;
void IRAM_ATTR pcnt_isr_handler(void *arg)
{
  //flag = !flag;
  //Serial.print("OI");
 // if(!flag){flag= true;
//pcnt_counter_pause(PCNT_UNIT_0);
//pcnt_counter_clear(PCNT_UNIT_0);
//pcnt_counter_resume(PCNT_UNIT_0);
  //}
 //  pcnt_counter_resume(PCNT_UNIT_0);
   //vTaskDelete(NULL);
}

void setup()
{
  // Código da tarefa
    pcnt_config_t pcnt_config = {
    .pulse_gpio_num = 23,
    .lctrl_mode = PCNT_MODE_REVERSE,
    .hctrl_mode = PCNT_MODE_KEEP,
    .pos_mode = PCNT_COUNT_INC,
    .neg_mode = PCNT_COUNT_DIS,
    .counter_h_lim = 5000,
    .counter_l_lim = -5000,
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

  Serial.begin(115200);

}
void loop()
{
int16_t counter_value;
pcnt_get_counter_value(PCNT_UNIT_0, &counter_value);
Serial.print(counter_value);
Serial.println();
if(counter_value >= 10)
{
  pcnt_counter_clear(PCNT_UNIT_0);
}
delay(5);
}