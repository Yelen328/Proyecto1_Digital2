/* UNIVERSIDAD DEL VALLE DE GUATEMALA
 * Proyecto1_Digital2.c
 * Descripción: 
 * Created: 10/2/2026 00:07:48
 * Author : yelena Cotzojay
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL  // 1. Definir la frecuencia (16MHz)
#include <util/delay.h>   // 3. Ahora sí, la librería de delays
#include <avr/interrupt.h>
#include <stdio.h>

#include "I2C/I2C.h"
#include "UART/UART.h"
//#include "sTemperatura/sTemperatura.h"

//Definición de direcciones 
#define slave1R (0x30 << 1)| 0x01		//para leer
#define slave1W (0x30 << 1)& 0b11111110	//Para escribir
#define slave2R (0x40 << 1)| 0x01		//para leer
#define slave2W (0x40 << 1)& 0b11111110	//Para escribir
#define slavesensortR (0x38 << 1)| 0x01		//para leer
#define slavesensortW (0x38 << 1)& 0b11111110	//Para escribir

//Para el sensor de temperatura:
#define initializacion_comand 0xE1 // Comando para iniciar la calibración.
#define TRIGGER 0xAC   // Comando para pedir una medición.
#define AHT10_SOFTRESET 0xBA // Comando para reiniciar el sensor por software.



uint8_t direccion;
uint8_t temp;
uint8_t bufferI2CS1=0;
uint8_t bufferI2CS2=0;
uint8_t buffersensor=0;
float humedad=0;
float temperatura=0;

void setup();
void refreshPORT(uint8_t VALOR);
void refreshPORT2(uint8_t VALOR);
void AHT10_begin(void);
void AHT10_Read(float *hum, float *temp);


int main(void)
{
   setup();
   char mensaje_serial[50];
   char buffer_sensor[50];
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
		
		sprintf(mensaje_serial, "Dato recibido del Esclavo 1: %d\r\n", bufferI2CS1);
		writeString(mensaje_serial);
		
		
		
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
		
		sprintf(mensaje_serial, "Dato recibido del Esclavo 2: %d\r\n", bufferI2CS2);
		writeString(mensaje_serial);
		
		
		
		
		//LEER ESCLAVO sensor de temperatura	
		
		writeString("Iniciando lectura...\r\n");
		AHT10_Read(&humedad, &temperatura);
		sprintf(buffer_sensor, "temperatura: %f\r\n", temperatura);
		writeString(buffer_sensor);
		_delay_ms(1000);
		
    }
}

void setup(){
	cli();	
	I2C_Master_Init(100000,1);
	INIT_UART(103);
	AHT10_begin();
	
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


void AHT10_begin(void){
	
	TWBR = 72;
	TWSR = 0x00;
	
	
	// 1. Reinicio por Software
	I2C_Start();
	I2C_Master_write(slavesensortW);
	I2C_Master_write(AHT10_SOFTRESET);
	I2C_Master_stop();
	
	_delay_ms(40); // El datasheet recomienda un poco más de tiempo tras el reset
	
	// 2. Calibración
	I2C_Start();
	I2C_Master_write(slavesensortW);
	I2C_Master_write(initializacion_comand);
	I2C_Master_write(0x08); // Parámetro de calibración
	I2C_Master_write(0x00);
	I2C_Master_stop();
	
	_delay_ms(20);
}


void AHT10_Read(float *hum, float *temp){
	uint8_t buffer[6];
	writeString("prepaso");
	// 1. Pedir medición
	I2C_Start();
	writeString("Paso 0");
	I2C_Master_write(slavesensortW);
	writeString("Paso 1");
	I2C_Master_write('R');	//Comando para prepararlo
	writeString("Paso 3");
	if (!I2C_repeatedStart())	//si está en el repeate start
	{
		I2C_Master_stop();
		return 0;
	}
	
	writeString("Paso 4");
	I2C_Master_write(slavesensortR);
	
	writeString("Paso 5");
	
	I2C_Master_write(TRIGGER);
	writeString("Paso trigger");
	
	I2C_Master_write(0x33);
	I2C_Master_write(0x00);
	I2C_Master_stop();
	
	writeString("Paso 6");
	
	_delay_ms(80); // Espera a que el sensor termine de medir
	
	// 2. Leer los 6 bytes usando tu función I2C_Master_read
	I2C_Start();
	I2C_Master_write(slavesensortR);
	writeString("Paso 7");
	
	for(int i=0; i < 6; i++) {
		if (i < 5) {
			// Tu función usa: I2C_Master_read(puntero_al_buffer, ack)
			// ack = 1 para que el sensor mande el siguiente byte
			I2C_Master_read(&buffer[i], 1);
			} else {
			// ack = 0 para el último byte (NACK)
			I2C_Master_read(&buffer[i], 0);
		}
	}
	I2C_Master_stop();
	// 3. Cálculos matemáticos
	uint32_t tdata = ((uint32_t)(buffer[3] & 0x0F) << 16) | ((uint32_t)buffer[4] << 8) | buffer[5];
	*temp = ((float)tdata * 200 / 1048576) - 50;
	
	uint32_t hdata = ((uint32_t)buffer[1] << 12) | ((uint32_t)buffer[2] << 4) | (buffer[3] >> 4);
	*hum = (float)hdata * 100 / 1048576;
}