/*
 * ADC.c
 *
 * Created: 27/01/2026 12:24:49
 *  Author: Lenovo
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "ADC.h"

void ADC_init(uint8_t Alin, uint8_t Ref, uint8_t Pin, uint8_t Act_interrup, uint8_t Prescaler){
	ADMUX = 0; //limpiar el registro del ADMUX
	
	//especificar la justificación utilizada
	if (!Alin){
		ADMUX &= ~(1<<ADLAR); //juantificación a la derecha
	}
	else {
		ADMUX |= (1<<ADLAR); //juatificación a la izquierda
	}
	
	switch (Ref) //definir el voltaje de referencia con el que se comparará
	{
		case 1:
		ADMUX |= (1<<REFS0)|(1<<REFS1); //voltaje de referencia interna 1.1
		break;
		case 2:
		ADMUX |= (1<<REFS0); //voltaje Vcc(5V)
		default:
		break;
	}
	 //Seleccionar el pin del puerto C que se quiere utilizar.
	switch(Pin){
		case 0:
		//se apagaron los registros en el inicio del código
		break;
		case 1:
		ADMUX |= (1<<MUX0);
		break;
		case 2:
		ADMUX |= (1<<MUX1);
		break;
		case 3:
		ADMUX |= (1<<MUX0)|(1<<MUX1);
		break;
		case 4:
		ADMUX |= (1<<MUX2);
		break;
		case 5:
		ADMUX |= (1<<MUX0)|(1<<MUX2);
		break;
		case 6:
		ADMUX |= (1<<MUX2)|(1<<MUX1);
		break;
		case 7:
		ADMUX |= (1<<MUX0)|(1<<MUX1)|(1<<MUX2);
		break;
		default:
		ADMUX |= (1<<MUX2)|(1<<MUX1);
		break;
	}
	//habilitar o no la ruitna de interrupción del ADC
	ADCSRA = 0;
	if (!Act_interrup){
		ADCSRA &= ~(1<<ADIE); //interrupción deshabilitada 
	}
	else {
		ADCSRA |= (1<<ADIE);	//	Habilitar interrupciones
	}
	
	//configuración del prescaler del ADC
	switch (Prescaler){
		case 2:
		break;
		case 4:
		ADCSRA |= (1<<ADPS1);
		break;
		case 8:
		ADCSRA |= (1<<ADPS1)|(1<<ADPS0);
		break;
		case 16:
		ADCSRA |= (1<<ADPS2);
		break;
		case 32:
		ADCSRA |= (1<<ADPS2)| (1<<ADPS0);
		break;
		case 64:
		ADCSRA |= (1<<ADPS1) |(1<<ADPS2);
		break;
		case 128:
		ADCSRA |= (1<<ADPS1)|(1<<ADPS0)|(1<<ADPS2);
		break;
		default:
		ADCSRA |= (1<<ADPS1)|(1<<ADPS0)|(1<<ADPS2);
		break;
	}
	
	ADCSRA |= (1<<ADEN)|(1<<ADSC); //Habilitar ADC e iniciar conversión
}