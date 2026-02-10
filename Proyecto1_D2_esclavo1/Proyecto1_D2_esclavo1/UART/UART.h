/*
 * UART.h
 *
 * Created: 31/1/2026 22:25:58
 *  Author: yelen
 */ 


#ifndef UART_H_
#define UART_H_


void INIT_UART(uint8_t BaudRate);
void WriteChar(char caracter);
void writeString(char* texto);

#endif /* UART_H_ */