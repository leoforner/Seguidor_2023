#include "EnconderCounter.h"
EnconderCounter encoder_esquerdo(36, PCNT_UNIT_0, 140, 1000);
EnconderCounter encoder_direito(13, PCNT_UNIT_1, 140, 1000);
void setup() {
    Serial.begin(115200);

}

unsigned long pastTime = 0;
bool rot = false;
void loop() {
  double speed_esquerdo = encoder_esquerdo.getRPS();
  double speed_direito = encoder_direito.getRPS();
  Serial.printf("Velocidade left: %.2f", speed_esquerdo);
  Serial.printf("Velocidade right: %.2f", speed_direito);

}