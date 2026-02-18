/*
 * I2C.c
 *
 * Created: 10/2/2026 00:15:32
 *  Author: yelen
 */ 

#include "I2C.h"

void I2C_Master_Init(unsigned long SCL_Clock, uint8_t prescaler){
	DDRC &= ~((1<<DDC4)|(1<<DDC5));		//Pines i2c como entradas SDA Y SCL
	//Seleccionar el valor de los bits para el prescaler
	switch(prescaler){
		case 1:
		TWSR &= ~((1<<TWPS0)|(1<<TWPS1));
		break;
		case 4:
		TWSR &= ~((1<<TWPS1));
		TWSR |= (1<<TWPS0);
		break;
		case 16:
		TWSR &= ~((1<<TWPS0));
		TWSR |= (1<<TWPS1);
		break;
		case 64:
		TWSR |= (1<<TWPS0)|(1<<TWPS1);
		break;
		default:
		TWSR &= ~((1<<TWPS0)|(1<<TWPS1));
		break;
	}
	TWBR = ((F_CPU/SCL_Clock)-16)/(2*prescaler);	//Calcular la velocidad
	TWCR |= (1<<TWEN); //Activar comunicacion I2C
}

//Función de inicio de la comunicación I2C
uint8_t I2C_Start(void){
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);	//Trabaja en modo master, bandera de interrupcion y condicion de inicio.
	/*IMPORTANTE
		Lo normal con comunicacion I2C es que el maestro este polliando y al esclavo se utilice las interrupciones.
	*/
	while (!(TWCR&(1<<TWINT)));	//Esperamos que ocurra evento con la bandera de TWCR
	
	return ((TWSR & 0xF8)==0x08);	//Nos quedamos con los bits de estado
	/*Porque 0x08? Porque ese es el comando o el codigo de estado que indica que se envio al condicion de inicio.
	  Si se envio la condicion de inicio entonces esta funcion debe regresar un 1.
	*/
}

uint8_t I2C_repeatedStart(void){
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);	//Trabaja en modo master, bandera de interrupcion y condicion de inicio.
	
	while (!(TWCR&(1<<TWINT)));	//Esperamos a que se encienda la bandera de TWCR
	
	return ((TWSR & 0xF8)==0x10);	//Nos quedamos con los bits de estado
	/*Porque 0x10? Porque ese es el comando o el codigo de estado que indica que se envio al condicion de inicio.
	  Si se envio la condicion de inicio entonces esta funcion debe regresar un 1.
	*/
}

void I2C_Master_stop(void){
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);	//Inicia la secuencia de parada
	
	while (TWCR & (1<<TWSTO));	//Esperamos que el bit se limpie
}

uint8_t I2C_Master_write(uint8_t dato){
	uint8_t estado;
	
	TWDR=dato;		//Cargo el dato
	TWCR=(1<<TWEN)|(1<<TWINT);	//Inicia la secuencia de envio limpiando la bandera y habilitando la interface
	
	while(!(TWCR&(1<<TWINT)));	//Esperamos al flag TWINT
	estado = TWSR & 0xF8;		//Limpiamos para quedarnos unicamente con los bits de estados
	//Verificar si se le envio una SLA+W con ACK o un dato con ACK
	//Verificar comandos en datasheet o en la presentacion de I2C
	if (estado == 0x18 || estado == 0x28 || estado == 0x40 ){
		return 1;
		}else {
		return estado;
	}
}

uint8_t I2C_Master_read(uint8_t *buffer, uint8_t ack){
	uint8_t estado;

	if (ack){
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
		} else {
		TWCR = (1<<TWINT)|(1<<TWEN);
	}

	while (!(TWCR&(1<<TWINT)));

	estado = TWSR & 0xF8;

	if (ack) {
		if (estado != 0x50) return 0;
		} else {
		if (estado != 0x58) return 0;
	}

	*buffer = TWDR;
	return 1;
}

void I2C_init_Slave(uint8_t address){
	DDRC &= ~((1<<DDC4)|(1<<DDC5));	//Pines como entradas
	
	TWAR = address <<1;
	
	//Se habilita la interfaz, ACK automatico, se habilita la iSR
	
	TWCR = (1<<TWEA)|(1<<TWEN)|(1<<TWIE);
}