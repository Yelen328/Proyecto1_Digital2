/*
 * ADC.c
 *
 * Created: 27/1/2026 17:22:30
 *  Author: yelena cotzojay
 */ 


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>

void ADC_init(uint8_t Alineacion, uint8_t VRef, uint8_t interrupcion, uint8_t Prescaler){
	ADMUX = 0; //Apagar todo
	
	//Justificación utilizada
	if (!Alineacion){
		ADMUX &= ~(1<<ADLAR); //juantificación a la derecha
	}
	else {
		ADMUX |= (1<<ADLAR); //juatificación a la izquierda
	}
	
	switch (VRef) //definir el voltaje de referencia
	{
		case 1:
		ADMUX |= (1<<REFS0)|(1<<REFS1); //voltaje de referencia interna 1.1
		break;
		
		case 2:
		ADMUX |= (1<<REFS0); //voltaje Vcc(5V)
		break;
		
		default:
		break;
	}
	
	//Seleccionar si se activará o no la ruitna de interrupción del ADC
	ADCSRA = 0;
	if (!interrupcion){
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
	
	ADCSRA |= (1 << ADSC); // Iniciar primera conversión
	ADCSRA |= (1 << ADEN);	//Habilitar ADS
}

void ADC_CANAL(uint8_t Canal){
	//Seleccion de canal
	ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2)|(1<<MUX3));	//limpiar todos los bits
	switch(Canal){
		//ADC0
		case 0:
		break;
		
		//ADC1
		case 1:
		ADMUX |= (1<<MUX0);
		break;
		
		//ADC2
		case 2:
		ADMUX |= (1<<MUX1);
		break;
		
		//ADC3
		case 3:
		ADMUX |= (1<<MUX0)|(1<<MUX1);
		break;
		
		//ADC4
		case 4:
		ADMUX |= (1<<MUX2);
		break;
		
		//ADC5
		case 5:
		ADMUX |= (1<<MUX0)|(1<<MUX2);
		break;
		
		//ADC6
		case 6:
		ADMUX |= (1<<MUX2)|(1<<MUX1);
		break;
		
		//ADC7
		case 7:
		ADMUX |= (1<<MUX0)|(1<<MUX1)|(1<<MUX2);
		break;
		
		default:
		//como predeterminado el canal 0
		break;
	}
}