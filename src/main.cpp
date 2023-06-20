#include "EnconderCounter.h"
#include <BluetoothSerial.h>
#include <PID.h>

BluetoothSerial SerialBT;

PID pid1;
PID pid2;

#define divTensao 13

#define PWMB 17
#define esq1 2
#define esq2 0
#define dir1 4
#define dir2 16

#define left 39
#define right 36

double kp = 0, ki = 0;

EnconderCounter * encoder1;

float interpolacaoLinear(float analog, float vMin, float vMax,
                float adc_min, float adc_max) {
  return vMin +
         (analog - adc_min) * (vMax - vMin) / (adc_max - adc_min);
}

float analogicoParaTensao(float analog) {
  if (analog < 22)
    return interpolacaoLinear(analog, 0.0, 0.17, 0, 22);
  else if (analog >= 22 && analog < 1832)
    return interpolacaoLinear(analog, 0.17, 1.66, 22, 1832);
  else if (analog >= 1832 && analog < 3138)
    return interpolacaoLinear(analog, 1.66, 2.69, 1832, 3138);
  else if (analog >= 3138 && analog < 4095)
    return interpolacaoLinear(analog, 2.69, 3.12, 3138, 4095);
  else
    return 3.2;
}

String texto = "";
void recebeDados(){
  // recebe as chars e soma em um texto
  char a = SerialBT.read();
  texto += a;

  // separa as constantes quando recebe o texto todo
  if(a == '}'){
    // muda as constantes 
    kp = (texto.substring(1, texto.indexOf('/'))).toFloat();
    ki = (texto.substring(texto.indexOf('/')+1, texto.indexOf('}'))).toFloat();

    // printa os valores
    Serial.print(kp);     Serial.print("\t");
    //Serial.print(kp1);     Serial.print("\t"); 
    Serial.print(ki);     Serial.print("\n");

    // limpa a variavel para o proximo loop
    //Serial.println(texto);
    texto = "";
  }
}


void beginEnc(void * xTaskParameters){
    encoder1 = new EnconderCounter(23, PCNT_UNIT_0, 140, 1000);  

    for(;;){ // loop perpétuo
    // caso ele receba algum dado ele altera as constantes 
    if(SerialBT.available()){
      recebeDados();
    }
  } 

    vTaskDelete(NULL);
}

void setup() {
    Serial.begin(115200);
    SerialBT.begin("teste_lol");

    pinMode(esq1, OUTPUT);
    pinMode(esq2, OUTPUT);
    pinMode(dir1, OUTPUT);
    pinMode(dir2, OUTPUT);
    pinMode(left, INPUT);
    pinMode(right, INPUT);

    pinMode(divTensao, INPUT);

    pinMode(PWMB, OUTPUT);
    ledcSetup(0, 5000, 12); // canal para esquerdo
    ledcAttachPin(PWMB, 0);

    xTaskCreate(beginEnc, "qualquercoisaai", 10000, NULL, 1, NULL);
    delay(1000);
}

void loop() {
    float tensaoBat = (analogicoParaTensao(analogRead(divTensao)))*7.08/1.78;
    int vMax_6v = (6.0*4095)/tensaoBat;
    if(vMax_6v > 4095) vMax_6v = 4095;

    float setPoint = analogRead(left)*20.0/4095.0;

    double vel1 = (encoder1->getRPS())/3;
    double erro1 = setPoint - vel1;
    uint32_t pwmsaida1 = pid1.simplePI(kp, ki, erro1, 4095);

    digitalWrite(dir1, HIGH);
    digitalWrite(dir2, LOW);
    digitalWrite(esq1, HIGH);
    digitalWrite(esq2, LOW);

    pwmsaida1 = map(pwmsaida1, 0, 4096, 0, vMax_6v);

    ledcWrite(0, pwmsaida1);

    Serial.printf("%.2f,%.3f\n", setPoint, vel1);
    //Serial.printf("bateria: %.2f\n", tensaoBat);    
    delay(100);
}