/* UNIVERSIDAD DEL VALLE DE GUATEMALA
 * Descripción:
 * Created: 10/2/2026 17:36:36
 * Author : Daniela Moriera 
 */ 

#ifndef F_CPU
#define F_CPU 16000000
#endif
//Librerias
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include "Ultrasonico/Ultrasonico.h"
#include "UART/UART.h"  // Incluir librería UART
#include "PWM/PWM.h"
#include "ADC/ADC.h"
#include "I2C/I2C.h"


//Definir dirección
#define SlaveAddress 0x40

uint8_t buffer =0;
//uint8_t dato_enviado_SU=0; 

/************************Variables globales***************************/
Ultrasonico sensor1; //declarando el puntero de la librería

/************************Variables***************************/
uint16_t distancia_s1;
uint8_t distancia_map_s1;
uint8_t Lec_ADC=0;
char bufferUART[50];  // Buffer para imprimir por UART
/************************Prototipos de funciones ***************************/
void setup(); //configuración de los pines e interrupciones
void initTM1(); //configuración de l timer 1



int main(void)
{
	setup();
	UART_writeString("Sistema iniciado - Sensor Ultrasonico\r\n");
	_delay_ms(1000);
 
    while (1) 
    {
		if (buffer=='R'){	//Si se recibe R
			if (ultrasonico_lectura_disponible(&sensor1)) {
				// Procesar la lectura
				distancia_s1 = ultrasonico_get_distancia(&sensor1);
				distancia_map_s1 = ultrasonico_get_distancia_map(&sensor1);
				
				// IMPRIMIR POR UART
				sprintf(bufferUART, "Distancia: %u cm | Mapeado: %u\r\n", distancia_s1, distancia_map_s1);
				UART_writeString(bufferUART);
				
				// CONTROL DEL LED EN PD4
				if (distancia_s1 < 10) {
					UART_writeString("LED ENCENDIDO - Objeto cerca!\r\n");
					Servo_setPosition(1);
					} else {
					UART_writeString("LED APAGADO - Objeto lejos\r\n");
					Servo_setPosition(0);
				}
				
				// Reiniciar para siguiente medición
				ultrasonico_reiniciar(&sensor1);
				_delay_ms(500);  // Aumentado a 500ms para ver mejor por UART
			}
			
			ultrasonico_trigger(&sensor1);
			_delay_ms(1);
			if (Lec_ADC >= 100)
			{
				PORTB |= (1<<PB0);   // Motor ON
			}
			else
			{
				PORTB &= ~(1<<PB0);  // Motor OFF
			}
		}
			
			
			
			buffer=0;
			}	//limpiar buffer para que se haga una vez cuando se le mande la información
			
			
		_delay_ms(100);
    }



//SUBRUTINAS

void setup()
{
	cli();
	DDRB |= (1 << PB0);
	DDRB |= (1<<PB1);   // IN1 salida
	PORTB &= ~(1 << PB0);
	
	ServoTimer2_init();
	UART_init(103);
	// Configurar LED en PD4 como salida
	initTM1();
	ultrasonico_init(&sensor1);
	ADC_init(1, 2, 0, 1, 128);
	PCICR |= (1 << PCIE2);    // Habilitar interrupciones pinchange PD
	PCMSK2 |= (1 << PCINT18);  // Habilitar interrupción pinchange PD2
	//Configurar como esclavo
	I2C_init_Slave(SlaveAddress);
	
	sei();
}


void initTM1()
{
	TCCR1A = 0;
	// Configurar Timer1 para medición
	TCCR1B = (1 << CS11);    // Prescaler 8, duración de 0.5 µs
	
}

//VECTORES DE INTERRUPCIÓN
/**********INTERRUPT ISR*****************/
ISR(PCINT2_vect) {
	//si detecta que el PD3 está en alto echo_state es 1
	uint8_t echo_state;
	if (PIND & (1 << PD2)) {
		echo_state = 1;
	}
	else {
		echo_state = 0; //cuando pasa al flanco de bajada es cuando termina el pulso
	}
	ultrasonico_procesar_eco(&sensor1, echo_state);
}
ISR(ADC_vect)
{
	Lec_ADC=ADCH; //guardar el valor leido de voltaje en la variable
	ADCSRA |= (1<<ADEN)|(1<<ADSC);
}

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
			TWDR = distancia_s1;	//Datos a enviar, se enviará 2 para confirmar comunicación con el mastro. 
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
