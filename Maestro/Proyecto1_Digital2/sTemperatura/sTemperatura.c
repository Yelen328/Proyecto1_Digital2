/*
 * sTemperatura.c
 *
 * Created: 16/2/2026 00:49:18
 *  Author: yelen
 */ 

#include "sTemperatura.h"

#include <avr/io.h>
#define  F_CPU	16000000
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>   // librería de delays

// La dirección I2C del sensor (su "nombre" en el bus).
#define slavesensortR (0x38 << 1)| 0x01		//para leer
#define slavesensortW (0x38 << 1)& 0b11111110	//Para escribir 
  
#define initializacion_comand 0xE1 // Comando para iniciar la calibración.
#define TRIGGER 0xAC   // Comando para pedir una medición.
#define AHT10_SOFTRESET 0xBA // Comando para reiniciar el sensor por software.

void i2c_init(void) {
	// Configura el Bit Rate Register (TWBR): Selecciona el factor de división para el generador de velocidad de bits
	// Fórmula: SCL_freq = CPU_freq / (16 + 2(TWBR) * Prescaler)
	// Para 16MHz y 100kHz de I2C:
	TWBR = 72;
	TWSR = 0x00; // Prescaler = 1
	
}

// --- i2c_start ---
void i2c_start(void) {
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); // Enviar condición de START
	//TWINT: interrupt flag: TWI was finished its current job and expects applicarion software responde
	//TWSTA: START CONDITION BIT->checks if the bus is avaliable, and generates a start condition on the bus if it is free.
	//TWEN: TWI ENABLE BIT / enables and antivate TWI-> 1: takes control over the I/O ins connected to the SCL and SDA
	//									0: Desactiva TWI y todas las transmisiones se terminan, independientemente de la operación
	while (!(TWCR & (1<<TWINT)));               // Esperar a que termine el hardware
}

// --- i2c_stop ---
void i2c_stop(void) {
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN); // Enviar condición de STOP
	//TWSTO: STOP condiition bit
	while(TWCR & (1<<TWSTO));                   // Esperar a que se ejecute el STOP
}

// --- i2c_write ---
void i2c_write(uint8_t data) {
	//TWDR: transmite mode-> contains the next byte to be trasnsmited
	TWDR = data;                                // Cargar el dato al registro de datos
	TWCR = (1<<TWINT) | (1<<TWEN);              // Iniciar la transmisión
	while (!(TWCR & (1<<TWINT)));               // Esperar a que termine de enviarse
}

// --- i2c_read (con ACK o NACK) ---
// El AHT10 necesita que se comfime (ACK) cada byte excepto el último
uint8_t i2c_read_ack(void) {
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);  // Leer y enviar ACK
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}

uint8_t i2c_read_nack(void) {
	TWCR = (1<<TWINT) | (1<<TWEN);              // Leer sin enviar ACK (Fin de lectura)
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}

void AHT10_begin(void){
	//Inicializar el hardware I2C del ATmega (Frecuencia, etc.)
	i2c_init();
	
	i2c_start();	//Inicia la comunicación en el bus I2C
	i2c_write(slavesensortW);
	i2c_write(AHT10_SOFTRESET); //Comando 0xBA
	i2c_stop();
	_delay_ms(20);	//Esperar a que el sensor reinicie
	
	i2c_start();
	i2c_write(slavesensortW);		// Dirección en modo escritura
	i2c_write(initializacion_comand);	//Comando 0xE1
	i2c_write(0x08);				// Bits de control según datasheet
	i2c_write(0x00);				// Bits de control según datasheet
	i2c_stop();
	
	_delay_ms(20); // Espera a que termine de calibrar
}

void AHT10_Read(float *hum, float *temp){
	uint8_t buffer[6];
	
	//Pedir datos:
	i2c_start();
	i2c_write(slavesensortW);
	i2c_write(TRIGGER);
	i2c_write(0x33);
	i2c_write(0x00);
	i2c_stop();
	
	_delay_ms(80);	// Tiempo para que el sensor procese
	
	//Leer datos 
	i2c_start();
	i2c_write(slavesensortR);
	for(int i=0; i<=5; ++i) {
		if (i < 5)
			buffer[i] = i2c_read_ack();  // Pide el siguiente byte
		else
			buffer[i] = i2c_read_nack(); // Indica que es el último byte
	}
	i2c_stop();
	
	//Cálculos
	//para tempeeratura
	uint32_t tdata = ((uint32_t)(buffer[3] & 0x0F) << 16) | ((uint32_t)buffer[4] << 8) | buffer[5];
	*temp = ((float)tdata * 200 / 1048576) - 50;
	
	uint32_t hdata = ((uint32_t)buffer[1] << 12) | ((uint32_t)buffer[2] << 4) | (buffer[3] >> 4);
	*hum = (float)hdata * 100 / 1048576;
}
	

