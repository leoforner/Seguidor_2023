#include <Arduino.h>

#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

// multtasks
TaskHandle_t observer;

// constantes do PID
float kp = 100, ki = 0.065, kd = 470;
//float kp1 = 270, ki1 = 0.6, kd1 = 430;


// velocidade do carrinho

String texto = "";
void recebeDados(){
  // recebe as chars e soma em um texto
  char a = SerialBT.read();
  texto += a;

  // separa as constantes quando recebe o texto todo
  if(a == '}'){
    // muda as constantes 
    kp = (texto.substring(1, texto.indexOf('/'))).toFloat();
    ki = (texto.substring(texto.indexOf('/')+1, texto.indexOf('%'))).toFloat();
    kd = (texto.substring(texto.indexOf('%')+1, texto.indexOf('&'))).toFloat();

    // printa os valores
    Serial.print(kp);     Serial.print("\t");
    Serial.print(ki);     Serial.print("\t");
    Serial.print(kd);     Serial.print("\t"); 
    texto = "";
  }
}

void enviaDados(void * pvParameters){
  for(;;){ // loop perpétuo
    // caso ele receba algum dado ele altera as constantes 
    if(SerialBT.available()){
      recebeDados();
    }else{
      // senao ele envia os dados para montar o grafico
      delay(150);
      // caso queirm mudar oq ele envia mude aqui
      SerialBT.printf("{%d}", erro);
    }
  } 
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("Esp32 seguidor v1");
  // inicia a task bluetooth
  xTaskCreatePinnedToCore(
    enviaDados,   //  Função da tarefa 
    "taskBT",     //  nome da tarefa 
    10000,        //  Tamanho (bytes) 
    NULL,         //  parâmetro da tarefa 
    2,            //  prioridade da tarefa (tem q ser 2 pra n dar conflito) 
    &observer,    //  observa a tarefa criada
    0);           //  nucleo
}


void loop() {
Serial.print("This loop runs on id:");
Serial.println(xPortGetCoreID());
}