#include <Arduino.h>
#include <lineSensor.h>

uint32_t timeFilter = 0; // filtro para o botao
uint8_t state = 0;

// analogicos
#define left 34
#define right 36
#define divTensao 39

// botoes
#define enc1 18
#define enc2 19
#define enc3 21
#define enc4 22
#define start 23

// motor 1
#define pwma 5
#define ain2 2
#define ain1 15

// motor 2
#define pwmb 17
#define bin1 4
#define bin2 16 

// sensores frontais
#define IR 34

uint8_t pinos[] = {32, 33, 25, 26, 27, 14, 12, 13},
        pinCount = 8;

lineSensor ls(pinCount, pinos, true);

void IRAM_ATTR change_state(){
  if((millis() - timeFilter) > 1000){
    if(state < 4) { // avança o estado
      state++;  
    }else{         // reseta a esp
      Serial.println("Esp reset - state 3");
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
    delay(10);
  }
  //vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);

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

  Serial.println("Calibrando...");
  // biblioteca dos sensores
  ls.begin();
  //ls.setVerb(true);
  ls.calibration(STATIC);
  ls.printConfig();

  // ponte h config 
  pinMode(pwma, OUTPUT);
  pinMode(pwmb, OUTPUT);
  ledcSetup(0, 5000, 12); // canal para esquerdo
  ledcSetup(1, 5000, 12); // canal para o direito
  ledcAttachPin(pwma, 1);
  ledcAttachPin(pwmb, 0);

  Serial.println("Sensor calibrado, pressione o botao para iniciar trajeto");
  while(state < 2) delay(10);
  delay(3000);
}

void loop() {
  // calcula onde esta a linha
  double position = ls.searchLine(&state) - 3500.0;
  Serial.printf("%.2f - %d - %d\n", position, state, analogRead(right));

  // controle entra aqui
  delay(1);
}