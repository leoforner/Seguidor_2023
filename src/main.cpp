#include <Arduino.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBt;

float kp1, ki1, kd1, kp2, ki2, kd2, kp3, ki3, kd3, speedR, speedL;

void setup() {
    Serial.begin(115200);
    SerialBt.begin("esp32");
}

void loop() {
    String texto;
    char a;

    //recebe o sinal bluetooth e transforma em string
    while(SerialBt.available()){
      a = SerialBt.read();
      texto += a;    
    }            
    
    //transforma nas variáveis corretas dependendo da chave recebida
      if(texto[0] == '{' && texto[(texto.length()-1)] == '}'){
        sscanf(texto.c_str(), "{%f/%f/%f/%f/%f/%f/%f/%f/%f/}", &kp1, &ki1, &kd1, &kp2, &ki2, &kd2, &kp3, &ki3, &kd3);
      }
      else if(texto[0] == '[' && texto[(texto.length()-1)] == ']'){
        sscanf(texto.c_str(), "[%f/%f]", &speedR, &speedL);
      }
      //reconhece "quebras" no sinal bluetooth
      else if(!texto.isEmpty()){
        Serial.println("ERRO");
      }
}
