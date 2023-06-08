#include <Arduino.h>
#include <lineSensor.h>

// analogicos
#define left 39
#define right 36
#define divTensao 13

// botoes
#define enc1 23
#define enc2 22
#define enc3 5
#define enc4 18
#define start 19

// motor 1
#define pwma 15
#define ain2 2
#define ain1 0

// motor 2
#define pwmb 17
#define bin1 4
#define bin2 16 

// sensores frontais
#define IR 34
uint8_t pinos[] = {35, 32, 33, 25, 26, 27, 14, 12},
        pinCount = 8;

lineSensor ls(pinCount, pinos, true);

void setup() {
  Serial.begin(115200);

  pinMode(left, INPUT);
  pinMode(right, INPUT);
  pinMode(enc1, INPUT_PULLDOWN);
  pinMode(enc2, INPUT_PULLDOWN);
  pinMode(enc3, INPUT_PULLDOWN);
  pinMode(enc4, INPUT_PULLDOWN);
  pinMode(start, INPUT_PULLDOWN);
  pinMode(divTensao, INPUT);
  pinMode(ain1, OUTPUT);
  pinMode(ain2, OUTPUT);
  pinMode(bin1, OUTPUT);
  pinMode(bin2, OUTPUT);
  pinMode(pwma, OUTPUT);
  pinMode(pwmb, OUTPUT);

  // biblioteca dos sensores
  ls.begin();
  //ls.setVerb(true);
  ls.calibration(DYNAMIC);
  ls.printConfig();

  // ponte h config 
  pinMode(pwma, OUTPUT);
  pinMode(pwmb, OUTPUT);
  ledcSetup(0, 5000, 12); // canal para esquerdo
  ledcSetup(1, 5000, 12); // canal para o direito
  ledcAttachPin(pwma, 1);
  ledcAttachPin(pwmb, 0);

  delay(1000);
}

uint16_t contador = 0;
bool vira = true;
void loop() {
  // calcula onde esta a linha
  double position = ls.searchLine() - 3500.0;
  
  //sensores
  Serial.printf("ls: %.2f\tleft: %d\tright: %d\tdivTen: %d\t", position, analogRead(left), analogRead(right), analogRead(divTensao));
  
  //encoders
  Serial.printf("enc1: %d\tenc2: %d\tenc3: %d\tenc4: %d\tstart: %d\t", digitalRead(enc1), digitalRead(enc2), digitalRead(enc3), digitalRead(enc4), digitalRead(start));
  
  // inverte a rotacao
  if(!contador){
    vira = !vira;
    digitalWrite(ain1, !vira);
    digitalWrite(ain2, vira);
    digitalWrite(bin1, !vira);
    digitalWrite(bin2, vira);
  }

  // aumenta o pwm
  contador = (contador >= 4095) ? 0 : contador + 100; 
  ledcWrite(0, contador);
  ledcWrite(1, contador);

  Serial.printf("lado: %s\tpwm: %d\n", vira ? "frente" : "contrario", contador);
  delay(100);
}