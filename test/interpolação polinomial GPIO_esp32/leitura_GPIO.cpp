#include <Arduino.h>
#define in 33
#define out 32

double leituraAnalogica;
int sinalAnalogico, degraus = 16;

void setup() {
  Serial.begin(115200);
  pinMode(in, INPUT);
  pinMode(out, OUTPUT);
      
  ledcSetup(0, 5000, 12); 
  ledcAttachPin(out, 0);
}

void loop() {
  for(double tensaoAplicada = 0.0; tensaoAplicada <= 3.27; tensaoAplicada += (3.27/32)){
    // converte a tensão para um sinal analogico
    sinalAnalogico = (tensaoAplicada*4095)/3.27;
    ledcWrite(0, sinalAnalogico);
    delay(500);

    // calcula a media do sinal recebido
    for(int i = 0; i < 100; i++){
      leituraAnalogica += analogRead(in);
    }leituraAnalogica /= 100;

    Serial.printf("%f;%f\n", tensaoAplicada, leituraAnalogica);

    leituraAnalogica = 0;
  }
}