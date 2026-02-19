/*
 * LDC.c
 *Descripción: Esta librería tiene el fin de facilitar la 
 representación de datos en una pantalla LDC, para la realización
 de la misma se tomó como base el ejemplo visto en clase.
 * Created: 27/1/2026 01:50:49
 *  Author: yelen
 */ 

#include "LDC.h"

//Función para inicializar LDC 
void initLDC8(void){
	//SALIDAS
	DDRD |= (1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7);	//declarar el puerto B como salidas
	PORTD = 0x00;	//Estado inicial apagado
	
	DDRB |= (1<<DDB3)|(1<<DDB2)|(1<<DDB4)|(1<<DDB5);	//Declarar los bits 2 y 3 del puerto B como salida
	PORTB = 0x00;	//Inicialmente apagado
	
	LDC_port(0x00);	//puerto inicialmente en 0
	_delay_ms(20);	//esperar 20ms
	/*LDC_CMD(0x30);
	_delay_ms(5);

	LDC_CMD(0x30);
	_delay_ms(5);

	LDC_CMD(0x30);
	_delay_ms(10);	//configurarlo en 8 bit*/
	
	//FUNCTION SET
	LDC_CMD(0x38);	// -> configurado en 2 lineas y de 5x8 pixeles
	
	//DISPLAY ON//OF
	LDC_CMD(0x0C);	//Dispay ENCIENDE
	
	//CLEAR DISPLAY
	LDC_CMD(0x01);
	
	//Entry mode
	LDC_CMD(0x06);
} 


//Función para enviar un comando
void LDC_CMD(char a){
	//RS=0; 
	PORTB &= ~(1<<PORTB3);
	LDC_port(a);
	
	//EN=1
	PORTB |= (1<<PORTB2);
	_delay_ms(1);
	
	//EN=0
	PORTB &= ~(1<<PORTB2);
}


//Función para colocar en el puerto un valor
void LDC_port(uint8_t valor){
	if (valor & 1)
	//B5 ==1;	bit0
	PORTB |= (1<<PORTB5);
	else 
	PORTB &= ~(1<<PORTB5); 
	
	if (valor & 2)
	//B4 ==1;	bit1
	PORTB |= (1<<PORTB4);
	else
	PORTB &= ~(1<<PORTB4);
	
	if (valor & 4)
	//D2 ==1;	bit2
	PORTD |= (1<<PORTD2);
	else
	PORTD &= ~(1<<PORTD2);
	
	if (valor & 8)
	//D3 ==1;	bit3
	PORTD |= (1<<PORTD3);
	else
	PORTD &= ~(1<<PORTD3);
	
	if (valor & 16)
	//D4 ==1;	bit4
	PORTD |= (1<<PORTD4);
	else
	PORTD &= ~(1<<PORTD4);
	
	if (valor & 32)
	//D5 ==1;	bit5
	PORTD |= (1<<PORTD5);
	else
	PORTD &= ~(1<<PORTD5);
	
	if (valor & 64)
	//D6 ==1;	bit6
	PORTD |= (1<<PORTD6);
	else
	PORTD &= ~(1<<PORTD6);
	
	if (valor & 128)
	//D7 ==1;	bit7
	PORTD |= (1<<PORTD7);
	else
	PORTD &= ~(1<<PORTD7);
}

 //Función para enviar un carácter
 void LDC_write_char(char caracter){
	 //RS=1;
	 PORTB |= (1<<PORTB3);
	 LDC_port(caracter);
	 //transición de 1 a 0 en enable para lograr capturar el dato 
	 PORTB |= (1<< PORTB2);
	 _delay_ms(4);
	 PORTB &= ~(1<<PORTB2);
	 _delay_us(100);
 }
 
 //Fución para enviar una cadena
 void LDC_write_string(char *cadena){
	 int i;
	 for (i=0; cadena[i] != '\0'; i++)
	 LDC_write_char(cadena[i]);
 }
 
 // Desplazamiento a la derecha
 void LDC_Desplazamiento_derecha(void){
	 LDC_CMD(0x1C);
 }
 
 //Desplazamiento a la izquierda
 void LDC_Desplazamiento_izquierda(void){
	 LDC_CMD(0x18);
 }
 
 //Establecer cursor
 void LDC_CURSOR(char c, char f){
	 char temp;
	 if (f==1){
		 temp=0x80+c-1;
		 LDC_CMD(temp);
 }
	else if (f==2)
	{
		temp=0xC0+c-1;
		LDC_CMD(temp);
	}
 }




