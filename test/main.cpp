#include <Arduino.h>
#include <lineSensor.h>
#include <EnconderCounter.h>
#include <mathModel.h>
#include "pins.h"

uint32_t timeFilter = 0;  // filtro para o botao
uint8_t state = 0;        // estados do carrinho 
// 0 desligado
// 1 calibrando
// 2 e 3 pista
// 4 final da pista

uint8_t pinos[] = {32, 33, 25, 26, 27, 14, 12, 13}, pinCount = 8;
lineSensor sensorFrontal(pinCount, pinos, true);

mathModel modeloMatematico;                  // X    Y
double carVector[3][2] =   {{+7.5, -2.0},    // roda direita
                            {-7.5, -2.0},    // roda esquerda
                            {+0.0, 11.0}};   // linha de sensores
double wheelsRadius = 1.5, actingTime = 1;

EnconderCounter encoderLeft(enc3, PCNT_UNIT_0, 140, 1000);
EnconderCounter encoderRight(enc1, PCNT_UNIT_1, 140, 1000);

void IRAM_ATTR change_state(){
  if((millis() - timeFilter) > 200){
    if(state < 4) { // avança o estado
      state++;  
    }else{         // reseta a esp
      Serial.println("Esp reset - state 4");
      ESP.restart();    
    }
    timeFilter = millis();
  }
}

void IRAM_ATTR interrupt(void * param){
  attachInterrupt(digitalPinToInterrupt(start), change_state, HIGH);
  //attachInterrupt(digitalPinToInterrupt(right), change_state, LOW);
  //attachInterrupt(digitalPinToInterrupt(left), change_state, LOW);
  
  // pino 36 e 39 nao suportam interrupt
  // ficamos verificando os senores laterais sempre
  while(1){
    if(!analogRead(right) && state > 1){
      // delay de 100 millis para esperar o loop principal subtrair 1,
      // senao somamos antes do loop considerar intersec.
      // podemos mudar esses 100 millis para menos, porem depende do tempo total de cada loop
      while(!analogRead(right)) delay(100);
      change_state();
    }
    delay(5);
  }
  //vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);

  pinMode(led, OUTPUT);
  digitalWrite(led, LOW); // desliga o led
  pinMode(left, INPUT);
  pinMode(right, INPUT);
  pinMode(enc1, INPUT);
  pinMode(enc2, INPUT);
  pinMode(enc3, INPUT);
  pinMode(enc4, INPUT);
  pinMode(start, INPUT);
  pinMode(divTensao, INPUT);
  pinMode(ain1, OUTPUT);
  pinMode(ain2, OUTPUT);
  pinMode(bin1, OUTPUT);
  pinMode(bin2, OUTPUT);
  pinMode(pwma, OUTPUT);
  pinMode(pwmb, OUTPUT);

  // anexa as interrupcoes ao segundo nucleo
  xTaskCreatePinnedToCore(
    interrupt,
    "interrupt",
    2048,
    NULL,
    2,
    NULL,
    0
  );

  Serial.println("Carrinho ligado, pressione o botao para iniciar calibração");
  while(state < 1) delay(10);

  modeloMatematico.begin(carVector, wheelsRadius, actingTime);

  Serial.println("Calibrando...");
  sensorFrontal.begin();
  sensorFrontal.setLed(led);
  // a linha tem 5.7 centimetros com 8 sensores
  float pesos[8];
  for(int i = 0; i < 8; i++) pesos[i] = i * 5.7/7; 
  // altera o peso padrao para calcular a distancia da linha
  sensorFrontal.setweights(pesos);
  // dessa forma a saida esta em nanometro
  // divindo por 100 a medida passa para centimetros
  sensorFrontal.setTrackCharacteristics(100, 0, 30);
  //sensorFrontal.setVerb(true);
  sensorFrontal.calibration(STATIC);
  sensorFrontal.printConfig();

  // ponte h config 
  pinMode(pwma, OUTPUT);
  pinMode(pwmb, OUTPUT);
  ledcSetup(0, 5000, 12); // canal para esquerdo
  ledcSetup(1, 5000, 12); // canal para o direito
  ledcAttachPin(pwma, 1);
  ledcAttachPin(pwmb, 0);

  Serial.println("Sensor calibrado, pressione o botao para iniciar trajeto");
  while(state < 2) delay(10);
  
  // sinalização piscando led
  delay(100);
  for(uint8_t i = 0; i < 4; i++){
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(800);
  }
}

void loop() {
  // distancia entre o sensor e a linha em cm
  double distanciaLinha = (sensorFrontal.searchLine(&state)/100.0) - 5.70/2.0;
  
  // calcula o setPoint de cada roda em cm/s
  double* wheelsSetPoint = modeloMatematico.calculateSetPoints(distanciaLinha);

  // resultados
  Serial.printf("erro: %.2f\tr1: %.3frps\tr2: %.3frps\t", distanciaLinha, wheelsSetPoint[0], wheelsSetPoint[1]);

  // velocidade dos motores
  double velRight = encoderRight.getRPS();
  double velLeft = encoderLeft.getRPS();
  Serial.printf("Right: %.2frps\tLeft: %.2frps\n", velRight, velLeft);

  delete wheelsSetPoint;

  // controle entra aqui
  delay(1);
}