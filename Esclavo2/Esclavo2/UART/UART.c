/*
 * UART.c
 *
 * Created: 2/02/2026 01:38:42
 *  Author: Lenovo
 */ 
#include "uart.h"

void UART_init(uint16_t ubrr)
{
	// Configurar pines
	DDRD |= (1<<DDD1);   // TX como salida
	DDRD &= ~(1<<DDD0);  // RX como entrada

	// Baud rate
	UBRR0 = ubrr;

	// Habilitar RX, TX e interrupción RX
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);

	// 8 bits, sin paridad, 1 stop bit
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
}

void UART_writeChar(char c)
{
	while (!(UCSR0A & (1<<UDRE0))); // Esperar buffer libre
	UDR0 = c;
}

void UART_writeString(char *str)
{
	while (*str)
	{
		UART_writeChar(*str++);
	}
}
