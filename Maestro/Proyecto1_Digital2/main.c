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
#include "LCD/LDC.h"
//#include "sTemperatura/sTemperatura.h"

//Definición de direcciones 
#define slave1R (0x30 << 1)| 0x01		//para leer
#define slave1W (0x30 << 1)& 0b11111110	//Para escribir
#define slave2R (0x40 << 1)| 0x01		//para leer
#define slave2W (0x40 << 1)& 0b11111110	//Para escribir
#define ST_I2CR	(0x48<<1)|0x01
#define ST_I2cW  (0x48<<1) & 0b11111110

//Para el sensor de temperatura:
#define initializacion_comand 0xE1 // Comando para iniciar la calibración.
#define TRIGGER 0xAC   // Comando para pedir una medición.
#define AHT10_SOFTRESET 0xBA // Comando para reiniciar el sensor por software.



uint8_t direccion;
uint8_t temp;
uint8_t bufferI2CS1=0;
uint8_t bufferI2CS2=0;
uint8_t buffersensor=0;
uint8_t estado_motor_DC=0;
float temperatura=0;

void setup();
void refreshPORT(uint8_t VALOR);
void refreshPORT2(uint8_t VALOR);

//void actualizarS3(char *lista, float temp);

//Funciones para sensor I2C
uint8_t leer_LM75(void);
float obtenerTemperatura();

char buffer_sensor[50];
uint16_t lectura_LM75 = 0;

int main(void)
{
   setup();
   Lcd_Clear();  // Limpiar pantalla
   //establecer el encabezado de las etiquetas
   /*
   Mover_puntero(0, 1);
   LCD_Write_string("Temp");
   Mover_puntero(8, 1);
   LCD_Write_string("AC");
   Mover_puntero(12, 1);
   LCD_Write_string("Door");*/
   char mensaje_serial[50];
   char buffer_sensor[50];
   WriteChar('a');
    while (1) 
    {
		  //Mover_puntero(0, 1);
		 // LCD_Write_string("S1");
		//Preparar el slave 1
		 Mover_puntero(0, 1);
		 LCD_Write_string("Temp");
		 Mover_puntero(0, 8);
		 LCD_Write_string("AC");
		 Mover_puntero(0, 12);
		 LCD_Write_string("Door");
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
	
		if (bufferI2CS1==1)
		{
			Mover_puntero(1,8);
			 LCD_Write_string("OK_");
		}
		else
		{
			Mover_puntero(1,8);
			 LCD_Write_string("DEN");
		}
		
		
		
		
		//LEER ESCLAVO 2
		if (!I2C_Start()) return 0;	// Iniciar el start, si no hay ningun problema, continua
		
		if (!I2C_Master_write(slave2W))	//si no devuelve 1 para la comunicación
		{
			I2C_Master_stop();
			return 0;
		}
		
		if (!I2C_Master_write(estado_motor_DC)){
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

		//Imprime el estado de la puerta
		if (bufferI2CS2==1)
		{	
			Mover_puntero(1,14);
			LCD_Write_string("A");
		}
		
		else{
			Mover_puntero(1,14);
			LCD_Write_string("C");
		}
		
		sprintf(mensaje_serial, "Dato recibido del Esclavo 2: %d\r\n", bufferI2CS2);
		writeString(mensaje_serial);
		
		
		//LEER ESCLAVO sensor de temperatura	
		//Lectura de S3 - Sensor de temperatura
		
		//LDC_CURSOR(2,1);	//Setear el cursor para la primera columna segunda fila
		//LDC_write_string("TEMP:");
		
		if(leer_LM75()){
			PINC |= (1<<PINC3);
			lectura_LM75 = obtenerTemperatura();
			Mover_puntero(1,1);
			sprintf(buffer_sensor, " %.d C", lectura_LM75);
			LCD_Write_string(buffer_sensor);
			writeString(buffer_sensor);
			
		}
		else {
			writeString("Lectura de sensor I2C fallo\n");
		}
		
		
		_delay_ms(1);
		
		
		//DATOS A ENVIAR AL ESCLAVO 2
		if (lectura_LM75 >= 25)
		{
			estado_motor_DC=1;
			//PORTB |= (1<<PB0);   // Motor ON
		}
		else
		{
			estado_motor_DC=0;
			//PORTB &= ~(1<<PB0);  // Motor OFF
		}
		
		_delay_ms(500);
		
    }
}

void setup(){
	cli();	
	I2C_Master_Init(100000,1);
	INIT_UART(103);
	//configurar puerto D y B
		//configurar puerto D y B
		DDRD |= (1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7);
		DDRB |= (1<<PB0)|(1<<PB1);
		PORTD &= ~((1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7));
		PORTB &= ~((1<<PB0)|(1<<PB1));
		//configuración del puerto C
		DDRC |= (1<<PC0)|(1<<PC1);	//Salidas y el resto como entradas
		PORTC &= ~((1<<PC0)|(1<<PC1));
	inicio8(); 
	Lcd_Clear();
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


/*Funcion para comunicarse con el sensor I2C de temperatura LM75*/
uint8_t leer_LM75(void)
{
	uint8_t MSB, LSB;

	if(!I2C_Start()) return 0;

	if(!I2C_Master_write(ST_I2cW)){
		I2C_Master_stop();
		return 0;
	}

	// Pointer TEMP = 0x00
	if(!I2C_Master_write(0x00)){
		I2C_Master_stop();
		return 0;
	}

	if(!I2C_repeatedStart()){
		I2C_Master_stop();
		return 0;
	}

	if(!I2C_Master_write(ST_I2CR)){
		I2C_Master_stop();
		return 0;
	}

	if(!I2C_Master_read(&MSB, 1)){
		I2C_Master_stop();
		return 0;
	}

	if(!I2C_Master_read(&LSB, 0)){
		I2C_Master_stop();
		return 0;
	}

	I2C_Master_stop();

	lectura_LM75 = (MSB << 8) | LSB;

	return 1;
}

/*Funcion para convertir la lectura del sensor a °C*/
float obtenerTemperatura(void)
{
	int16_t temp = lectura_LM75 >> 7;
	return temp * 0.5;
}


