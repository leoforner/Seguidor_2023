#include <Arduino.h>

#include <BluetoothSerial.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
BluetoothSerial SerialBT;

// multtasks
TaskHandle_t observer;

void recebeDados()
{

}

// the loop2 function also runs forver but as a parallel task
void loopBluetooth (void* pvParameters) {
  while (1) {
     Serial.print("This loop runs on id:");
    Serial.println(xPortGetCoreID());
    delay (2000);
    if(SerialBT.available()){
      recebeDados();
    }
}
}
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode (LED_BUILTIN, OUTPUT);

  Serial.begin (115200);
  SerialBT.begin("Esp32 seguidor v1");
  xTaskCreatePinnedToCore (
    loopBluetooth,     // Function to implement the task
    "loopBluetooth",   // Name of the task
    1000,      // Stack size in bytes
    NULL,      // Task input parameter
    0,         // Priority of the task
    NULL,      // Task handle.
    0          // Core where the task should run
  );
}

// the loop function runs over and over again forever
void loop() {
  Serial.print("This loop runs on id:");
  Serial.println(xPortGetCoreID());
  digitalWrite (LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay (1000);  // wait for a second
  digitalWrite (LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
  delay (1000);  // wait for a second
}

