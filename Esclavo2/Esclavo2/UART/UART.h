/*
 * IncFile1.h
 *
 * Created: 2/02/2026 01:38:59
 *  Author: Lenovo
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdint.h>

// Inicializar UART
void UART_init(uint16_t ubrr);

// Enviar un carácter
void UART_writeChar(char c);

// Enviar una cadena
void UART_writeString(char *str);
#endif