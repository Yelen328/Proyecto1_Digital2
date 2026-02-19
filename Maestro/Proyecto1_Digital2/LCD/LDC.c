/*
 * LCD8.c
 *
 * Created: 27/01/2026 15:43:53
 *  Author: Lenovo
 */ 

#include "LDC.h"
//#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
void enviar_dato(uint8_t data)
{
	// Limpiar primero los bits usados
	PORTD &= ~((1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7));
	PORTB &= ~((1<<PB0)|(1<<PB1));

	//Linpiar los primeros dos bits y desplazarlos para solo mostrar del D2 al D7
	PORTD |= ((data & 0x3F) << 2);

	//Los dos bits más significativos se guardan y se muestran en el puerto B
	PORTB |= (data >> 6) & 0x03;
}
//funcion para enviar un comando
void LCD_CDM(char a)
{
	PORTC &= ~(1<<PORTC0);  //RS = 0, se le indica que es modo comando
	enviar_dato(a);
	PORTC |= (1<<PORTC1);    // E = 1, se le indica que envie los datos
	_delay_ms(2);
	PORTC &= ~(1<<PORTC1);   // E = 0,  se le indica que se cierra el envio de datos
	_delay_ms(2);
}
void inicio8(void)
{
	// Limpiar primero los bits usados
	PORTD &= ~((1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7));
	PORTB &= ~((1<<PB0)|(1<<PB1));
	_delay_ms(20); 
	LCD_CDM(0x30);
	_delay_ms(5);
	LCD_CDM(0x30);
	_delay_ms(5);
	LCD_CDM(0x30);
	_delay_ms(10);

	
	//function Set 
	LCD_CDM(0x38);
	//Display ON/OF
	LCD_CDM(0X0C);
	//entry mode 
	LCD_CDM(0x01);
	LCD_CDM(0x06);
	
	
}
void LCD_Write_Char(char c)
{
	PORTC |= (1<<PORTC0);  //RS = 1, Para mandar el caracter
	enviar_dato(c);
	PORTC |= (1<<PORTC1);    // E = 1, se le indica que envie los datos
	_delay_ms(2);
	PORTC &= ~(1<<PORTC1);   // E = 0,  se le indica que se cierra el envio de datos
	_delay_ms(2);
}
void Mover_puntero(char a, char b)
{
	char temp; 
	if(a == 0){
	temp = 0x80 + b-1;  //Posicionarse en la linea 1 y se suma la columna
	LCD_CDM(temp);
	}
	else if(a == 1){
	temp=0xC0 + b-1;  //Posicionarse en la linea 2  y se suma la columna
	LCD_CDM(temp);
	}
}
void LCD_Write_string(char *a)
{
	int i; 
	for(i=0; a[i] != '\0'; i++)
	{ //recorrer el puntero caracter por caracter hasta que este vacío
	LCD_Write_Char(a[i]); 
	}
}
void Lcd_Shift_Right() //Activa el corrimiento hacia la derecha
{
	LCD_CDM(0x1C);    
}

void Lcd_Shift_Left()    //Activa el corrimiento hacia la izquierda
{
	LCD_CDM(0x18); 
}
//limpiar la LCD
void Lcd_Clear()    
{
	LCD_CDM(1);  
}