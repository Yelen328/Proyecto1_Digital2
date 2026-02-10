/*
 * ADC.h
 *
 * Created: 27/1/2026 17:19:17
 *  Author: yelena Cotzojay
 */ 


#ifndef ADC_H_
#define ADC_H_

void ADC_init(uint8_t Alineacion, uint8_t VRef, uint8_t interrupcion, uint8_t Prescaler);
void ADC_CANAL(uint8_t Canal);
#endif /* ADC_H_ */