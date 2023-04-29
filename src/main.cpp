#include <Arduino.h>
#define in 12
#define out 27

double leituraAnalogica;
int sinalAnalogico, degraus = 16;

void setup() {
  Serial.begin(115200);
  pinMode(in, INPUT);
  pinMode(out, OUTPUT);
      
  ledcSetup(0, 1.0e5, 12); 
  ledcAttachPin(out, 0);
}

void loop() {
  // descarrega o capacitor
  ledcWrite(0, 0);
  delay(500);

  // aplica degruas na porta analogica
  for(float tensaoAplicada = 0.0; tensaoAplicada < 3.27; tensaoAplicada += (3.27/degraus)){
    // aumenta de a tensao
    sinalAnalogico = (tensaoAplicada*4095)/3.27;
    ledcWrite(0, sinalAnalogico);

      // calcula a media de 100 mmedidas
    for(int i = 0; i < 100; i++){
        leituraAnalogica += analogRead(in);
    } leituraAnalogica /= 100.0;

    Serial.printf("%.2f;%.0f\n", tensaoAplicada, leituraAnalogica);

    // zera a variavel para o proximo loop
    leituraAnalogica = 0; 
  }
}