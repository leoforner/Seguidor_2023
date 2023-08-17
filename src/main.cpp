// bibliotecas
#include <Arduino.h>
#include <EnconderCounter.h>

// includes
#include "adc.h"
#include "pins.h"
#include "wheels.h"
#include <esp_now.h>
#include <WiFi.h>

EncoderCounter encoderLeft(enc3, PCNT_UNIT_0, 140, 1000);
EncoderCounter encoderRight(enc1, PCNT_UNIT_1, 140, 1000);

         
// rodas
wheels wheelLeft, wheelRight;

// canais pwm (precisa definir aqui para definir em pins.h)
uint8_t channelLeft = 0, channelRight = 1;

//declaracao de variaveis que serao enviadas para comunicação
long int timestart = 0;
long int DataTime[300];
double DataVelesq[300];
double DataVeldir[300];

// MAC Address do ESP Receptor
uint8_t broadcastAddress[] = {0x84, 0xCC, 0xA8, 0x54, 0x53, 0x30};

// Definição da estrutura
typedef struct struct_message {
  long int time;
  double velesq;
  double veldir;
  } struct_message;

// Instancia da estrutura
struct_message myData;
esp_now_peer_info_t peerInfo;

// confirmação de envio dos dados
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

int contador = 0; //iterador do loop de envio de dados





void setup(){
   
    definePins();
     // Init Serial Monitor
    Serial.begin(115200);
    
    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(OnDataSent);
    
    // Register peer
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    
  // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
  }
    timestart = millis();

    // define as propriedades das rodas
    wheelLeft.mov = STOPPED;    // inicia como parada
    wheelLeft.l1 = bin1;
    wheelLeft.l2 = bin2;
    wheelLeft.channelPWM = channelLeft;

    wheelRight.mov = STOPPED;   // inicia como parada
    wheelRight.l1 = ain1;
    wheelRight.l2 = ain2;
    wheelRight.channelPWM = channelRight;

    // manda pwm zero nos motores (quando a esp reseta estava salvando o ultimo pwm aplicado)
    applyPWM(&wheelLeft, 0);
    applyPWM(&wheelRight, 0);

    delay(3000);
    // sinalização piscando led
    for(uint8_t i = 0; i < 4; i++){
        digitalWrite(led, HIGH);
        delay(100);
        digitalWrite(led, LOW);
        delay(500);
    }
}

void loop(){
    float tensaoBateria = (analogicoParaTensao(analogRead(divTensao)))* 3.96; // 7.6/1.92; //7.6v viram 1.92v (divisor de tensão)
    if(tensaoBateria < 7.7) state == OFF; // desliga
    int pwm_saida = (8.0*4095)/tensaoBateria;

    

    // aplica o pwm
    applyPWM(&wheelRight, pwm_saida);
    applyPWM(&wheelLeft, pwm_saida);  
    // Set values to send
  if (contador<300){
    DataTime[contador] = millis() - timestart;
    DataVelesq[contador] = random(-20, 20);
    DataVeldir[contador] = random(-20, 20);
  }else{
    for (int i =0; i<300; i++){
      myData.time = DataTime[i];
      myData.velesq = DataVelesq[i];
      myData.veldir = DataVeldir[i];
      // Send message via ESP-NOW
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
     if (result == ESP_OK) {
       Serial.println("Sent with success");
     }
     else {
        Serial.println("Error sending the data");
     }
     delay(1000);
    }

  }
  delay(14);
  contador++;

    delay(2);
}