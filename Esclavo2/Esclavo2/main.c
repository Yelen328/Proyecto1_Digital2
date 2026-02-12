/* UNIVERSIDAD DEL VALLE DE GUATEMALA
 * Descripción:
 * Created: 10/2/2026 17:36:36
 * Author : Daniela Moriera 
 */ 

#include <avr/io.h>
#define  F_CPU	16000000
#include "util/delay.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "I2C/I2C.h"


//Definir dirección
#define SlaveAddress 0x40

uint8_t buffer =0;
uint8_t dato_enviado=2; 

//PROTOTIPOS DE FUNCIÓN
void setup();


int main(void)
{
	setup();
    /* Replace with your application code */
    while (1) 
    {
		if (buffer=='R'){	//Si se recibe R
			PINB |= (1<<PINB5);	//Hacer un toggle
			buffer=0;
			}	//limpiar buffer para que se haga una vez cuando se le mande la información
			
		
		
		_delay_ms(100);
		
		
		
    }
}


//SUBRUTINAS
void setup(){
	cli();
	DDRB |= (1<<DDB5);
	PORTB &= ~(1<<PORTB5);
	//Configurar como esclavo
	I2C_init_Slave(SlaveAddress);
	sei();

}


//VECTORES DE INTERRUPCIÓN


//Vector de interrupción de I2C
ISR(TWI_vect){
	uint8_t estado = TWSR & 0xFC; //Revisar los 5 bits más significativos del registro de estado
	switch(estado){
		//Slave debe recibir el dato
		
		case 0x60:	//se recibido la dirección a la que se quiere escribir
		case 0x70: //General Call (se quiere escribir en el esclavo)
			TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			//limpiar interrupción
			//Activar la interfase
			//Volver a activar la interrupción
			//Activar ACK
			break;
			
		case 0x80:	//Datos recibido, ACK enviado
		case 0x90:	//Dato recibido General Call, ACK enviado
			buffer=TWDR; //si se recibe un 'R' en eel buffer hacer un toggle en el PB5
			TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			break;
			
		//Si Slave transmitir datos
		
		case 0xA8:	//Dirección recibida para ser leído
		case 0xB8: //Dato transmitido, ACK recibido
			TWDR = dato_enviado;	//Datos a enviar, se enviará 2 para confirmar comunicación con el mastro. 
			TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			break;
			
		case 0xC0: //Dato transmitido, Nack recibido
		case 0xC8: //Ultimo dato transmitido
			TWCR=0;
			TWCR=(1<<TWEN)|(1<<TWEA)|(1<<TWIE);
			break;
			
		case 0xA0: //Stop o repeated star recibido como slave
			TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			break;
			
		//cualquier error
		//Se reinicia la interfaz
		default:
			TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			break;
		
	}
}
