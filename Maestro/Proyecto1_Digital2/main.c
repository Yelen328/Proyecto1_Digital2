/* UNIVERSIDAD DEL VALLE DE GUATEMALA
 * Proyecto1_Digital2.c
 * Descripción: 
 * Created: 10/2/2026 00:07:48
 * Author : yelena Cotzojay
 */ 

#include <avr/io.h>
#define  F_CPU	16000000
#include "util/delay.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "I2C/I2C.h"

//Definición de direcciones 
#define slave1R (0x30 << 1)| 0x01		//para leer
#define slave1W (0x30 << 1)& 0b11111110	//Para escribir
#define slave2R (0x40 << 1)| 0x01		//para leer
#define slave2W (0x40 << 1)& 0b11111110	//Para escribir


uint8_t direccion;
uint8_t temp;
uint8_t bufferI2CS1=0;
uint8_t bufferI2CS2=0;

void setup();
void refreshPORT(uint8_t VALOR);
void refreshPORT2(uint8_t VALOR);


int main(void)
{
   setup();
    while (1) 
    {
		//Preparar el slave 1
		if (!I2C_Start()) return 0;	// Iniciar el start, si no hay ningun problema, continua
		
		if (!I2C_Master_write(slave1W))	//si no devuelve 1 para la comunicación
		{
			I2C_Master_stop();
			return 0;
		}
		
		
		I2C_Master_write('R');	//Comando para prepararlo
		
		if (!I2C_repeatedStart())	//si está en el repeate start 
		{	
			I2C_Master_stop();
			return 0;
		}
			
		if (!I2C_Master_write(slave1R)){	//SI todo corre bien le escribo al esclavo 
			I2C_Master_stop();				// si no se da, paro la comunición
			return 0;
		}
		
		I2C_Master_read(&bufferI2CS1,0);	//NACK
		I2C_Master_stop();
		
		
		//LEER ESCLAVO 2
		if (!I2C_Start()) return 0;	// Iniciar el start, si no hay ningun problema, continua
		
		if (!I2C_Master_write(slave2W))	//si no devuelve 1 para la comunicación
		{
			I2C_Master_stop();
			return 0;
		}
		
		
		I2C_Master_write('R');	//Comando para prepararlo
		
		if (!I2C_repeatedStart())	//si está en el repeate start
		{
			I2C_Master_stop();
			return 0;
		}
		
		if (!I2C_Master_write(slave2R)){	//SI todo corre bien le escribo al esclavo
			I2C_Master_stop();				// si no se da, paro la comunición
			return 0;
		}
		
		I2C_Master_read(&bufferI2CS2,0);	//NACK
		I2C_Master_stop();
		
		
		refreshPORT(bufferI2CS1);
		refreshPORT2(bufferI2CS2);
		_delay_ms(1000);
    }
}

void setup(){
	cli();
	//SALIDAS
	DDRD |= (1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7);	//declarar el puerto D como salidas
	PORTD = 0x00;	//Estado inicial apagado
	
	
	//Declarar los bits 0 y 1 del puerto B como salida
	DDRB |= (1<<DDB0)|(1<<DDB1);
	PORTB = 0x00;	//Inicialmente apagado
	
	I2C_Master_Init(100000,1);
	sei();
}

void refreshPORT(uint8_t VALOR){
	// ---- PORTD bits PD2–PD7 ----
	PORTD &= ~((1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7)); // limpiar

	if(VALOR & (1<<0)) PORTD |= (1<<PD2);
	if(VALOR & (1<<1)) PORTD |= (1<<PD3);
	if(VALOR & (1<<2)) PORTD |= (1<<PD4);
	if(VALOR & (1<<3)) PORTD |= (1<<PD5);
	if(VALOR & (1<<4)) PORTD |= (1<<PD6);
	if(VALOR & (1<<5)) PORTD |= (1<<PD7);

	// ---- PORTB bits PB0–PB1 ----
	PORTB &= ~((1<<PB0)|(1<<PB1)); // limpiar

	if(VALOR & (1<<6)) PORTB |= (1<<PB0);
	if(VALOR & (1<<7)) PORTB |= (1<<PB1);
}

void refreshPORT2(uint8_t VALOR){
	// ---- PORTB bits PB2–PB3 ----
	PORTB &= ~((1<<PB2)|(1<<PB3)); // limpiar

	if(VALOR & (1<<0)) PORTB |= (1<<PB2);
	if(VALOR & (1<<1)) PORTB |= (1<<PB3);
}


