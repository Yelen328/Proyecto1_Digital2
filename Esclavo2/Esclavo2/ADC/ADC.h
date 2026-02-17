/*
 * ADC.h
 *
 * Created: 11/04/2025 12:25:05
 *  Author: Daniela Moreira 
 *Libreria para inicializar y configurar el ADC
 */ 


#ifndef ADC_H_
#define ADC_H_

void ADC_init(uint8_t Alin, uint8_t Ref, uint8_t Pin, uint8_t Act_interrup, uint8_t Prescaler);

#endif /* ADC_H_ */