/*
 * UART.c
 *
 * Created: 31/1/2026 22:25:46
 *  Author: yelen
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>

void INIT_UART(uint8_t BaudRate){
	DDRD |= (1<<DDD1);	//TX transmit -> salida
	DDRD &= ~(1<<DDD0);	//RX recive -> Entrada
	//configurar UCSR0A
	UCSR0A=0;
	
	
	//Configuración UCSR0B: Habililitndo la interrupción al escribir:
	//-Habilibitar recepción
	//-Habilitación de transmisión
	UCSR0B |= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	
	//Configurar UCSR0C
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);
	//Configurar UBRR0: UBRR0 = 103 -> 9600  @ 16MHz
	UBRR0 = BaudRate;

}

void WriteChar(char caracter){
	while ( (UCSR0A & (1<<UDRE0))==0){
		
	}

	UDR0=caracter;
	
}

void writeString(char* texto)
{
	for(uint8_t i = 0; *(texto+i) !='\0'; i++)
	{
		WriteChar(*(texto+i));
	}
	
}
