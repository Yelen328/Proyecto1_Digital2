/*
 * LDC.h
 *
 * Created: 27/1/2026 01:50:34
 *  Author: yelen
 */ 


#ifndef LDC_H_
#define LDC_H_



#include <avr/io.h>
#define F_CPU 16000000UL  // 1. Definir la frecuencia (16MHz)
#include <util/delay.h>   // 3. Ahora sí, la librería de delays
#include <avr/interrupt.h>
#include <stdio.h>
void enviar_dato(uint8_t data);
void LCD_CDM(char a);
void inicio8(void);
void LCD_Write_Char(char c);
void Mover_puntero(char a, char b);
void LCD_Write_string(char *a);
void Lcd_Shift_Right();
void Lcd_Shift_Left();
void Lcd_Clear();



#endif /* LDC_H_ */