#ifndef ADC_h
#define ADC_h

float interpolacaoLinear(float analog, float vMin, float vMax, float adc_min, float adc_max) {
  return vMin + (analog - adc_min) * (vMax - vMin) / (adc_max - adc_min);
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

#endif