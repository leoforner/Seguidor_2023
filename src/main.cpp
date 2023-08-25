#include <Arduino.h>
#include <ArduinoJson.h>

#define nPoints 10

#define x 0
#define y 1

uint8_t cicliIndex = 0; 

// pontos passados
    float points[nPoints][2] = 
                     {{1, 1},
                      {2, 3},
                      {3, 6},
                      {4, 9},
                      {5, 18},
                      {6, 27},
                      {7, 40},
                      {8, 60},
                      {9, 90},
                      {10, 120}};

const int pinVM = 15;    // Pino para o potenciômetro 'vm'
const int pinOmega = 2;  // Pino para o potenciômetro 'omega'
const int pinNoise = 4;  // Pino para o potenciômetro 'noise'

// Função para mapear um valor de uma faixa para outra
float mapFloat(float value, float in_min, float in_max, float out_min, float out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// so confio q funciona
float generateGaussianNoise(float mean, float stddev) {
  static float spare;
  float u, v, s;

  do {
    u = random(0, 255) / 255.0 * 2.0 - 1.0;
    v = random(0, 255) / 255.0 * 2.0 - 1.0;
    s = u * u + v * v;
  } while (s >= 1.0 || s == 0.0);

  s = sqrt(-2.0 * log(s) / s);
  spare = v * s;
  return mean + stddev * u * s;
}

float* carDisplacement(float vm, float omega, float dT){
    // valores default para evitar divisao por zero
    float dx = vm * dT;
    float dy = 0;

    if(omega > 0.0){
        // integral (de 0 a T) vm * cos(omega * dT) dt
        dx = vm * sin(omega * dT)/omega;
        // integral (de 0 a T) vm * sin(omega * dT) dt
        dy = vm * (1 - cos(omega * dT))/omega;
    }

    //debug
    //printf("dx: %f\tdy: %f\traio: %f\n", dx, dy, vm/omega);

    // retorna o deslocamento em x e y
    float * displacement = new float[2];
    displacement[x] = dx;
    displacement[y] = dy;
    return displacement;
}

void addPoint(float points[][2], float displacement[2], float phi){
    // remove o ultimo elemento e adiciona 0 como o primeiro
    points[cicliIndex][x] = 0;
    points[cicliIndex][y] = 0;

    // calculo sen e cos uma vez para economizar clock
    float cosPhi = cos(phi);
    float sinPhi = sin(phi);

    // translada e rotaciona os pontos
    for(uint8_t i = 0; i < nPoints; i++){
        float tempDX = (points[i][x] - displacement[x]) * cosPhi - (points[i][y] - displacement[y]) * sinPhi;
        points[i][y] = (points[i][x] - displacement[x]) * sinPhi + (points[i][y] - displacement[y]) * cosPhi;
        points[i][x] = tempDX;
    }

    // muda o array
    cicliIndex = cicliIndex == nPoints-1 ? 0 : cicliIndex+1;
}

uint32_t start = 0;
void setup(){
    Serial.begin(115200);

    // Configuração dos pinos dos potenciômetros como entradas analógicas
    pinMode(pinVM, INPUT);
    pinMode(pinOmega, INPUT);
    pinMode(pinNoise, INPUT);

    // random seed
    randomSeed(analogRead(0));
}

void loop(){    
    // tempo a cada interação do carrinho
    uint32_t dTime = 3;

    // leitura dos valores dos potenciômetros
    uint16_t vmValue = analogRead(pinVM);
    uint16_t omegaValue = analogRead(pinOmega);
    uint16_t noiseValue = analogRead(pinNoise);

    // mapeamento dos valores lidos para a faixa de escolha
    float vm = mapFloat(vmValue, 0, 4095, 0, 10);
    float omega = mapFloat(omegaValue, 0, 4095, 0.0, -1.5708);
    float noise = mapFloat(noiseValue, 0, 4095, 0, 1);

    // calcula deslocamento do carrinho
    float * displacement = carDisplacement(vm, omega, dTime);

    // calcula um ruido gausiano
    float noiseX = generateGaussianNoise(0.0, noise);
    float noiseY = generateGaussianNoise(0.0, noise);

    // aplica o ruido
    displacement[x] += noiseX;
    displacement[y] += noiseY;

    // desloca os pontos
    addPoint(points, displacement, omega*dTime);

    // Criação do JSON
    StaticJsonDocument<2048> doc; // Tamanho do buffer para o JSON
    JsonArray x_array = doc.createNestedArray("x");
    JsonArray y_array = doc.createNestedArray("y");

    // passa o array para o json
    for(uint8_t i = 0; i < 10; i++){
        x_array.add(points[i][x]);
        y_array.add(points[i][y]);
    }

    // printa o json para o python ler
    serializeJson(doc, Serial);
    Serial.println();

    // deleta o ponteiro
    delete [] displacement;

    // nao recomendo baixar esse delay pois o python é lento replotando
    delay(150);
}