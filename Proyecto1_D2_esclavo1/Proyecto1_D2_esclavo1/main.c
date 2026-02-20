/* UNIVERSIDAD DEL VALLE DE GUATEMALA
 * Proyecto1_D2_esclavo1.c
 * Descripción:
 * Created: 10/2/2026 09:36:36
 * Author : yelena Cotzojay
 */ 

#include <avr/io.h>
#define  F_CPU	16000000
#include "util/delay.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "I2C/I2C.h"
#include "ADC/ADC.h"
#include "UART/UART.h"

#define S0 2		// S0 a pin 2
#define S1 3		// S1 a pin 3
#define S2 6		// S2 a pin 6
#define S3 7		// S3 a pin 7
#define salidaTCS 5	// salidaTCS a pin 5


//Definir dirección
#define SlaveAddress 0x30

uint8_t bufferi2c =0;
char buffer[32];
int rojo=0;
volatile uint8_t color_detectado = 1; // 1 para ROJO, 0 para DENEGADO
//volatile uint8_t ValorADC=0;
uint8_t pasos[] = {
	(1<<PB0) | (1<<PB1),  // Paso 1: PB0, PB1
	(1<<PB1) | (1<<PB2),  // Paso 2: PB1, PB2
	(1<<PB2) | (1<<PB3),  // Paso 3: PB2, PB3
	(1<<PB3) | (1<<PB0)   // Paso 4: PB3, PB1
};

//PROTOTIPOS DE FUNCIÓN
void setup();
void INIT_SCOLOR();
uint16_t Leer_frecuencia_reloj();
void mover_motor(float n_vueltas, uint8_t sentido);



int main(void)
{
	setup();
    /* Replace with your application code */
    while (1) 
    {
		
		//Si se recibe R
		
		if (bufferi2c=='R'){
		
		//Establecer fotodiodos con filtro rojo
		PORTD &= ~((1<<S2)|(1<<S3));
		rojo= Leer_frecuencia_reloj();
		
		
		_delay_ms(200);
		
		//Establecer fotodiodos con filtro verde
		PORTD |= ((1<<S2)|(1<<S3));
		int verde= Leer_frecuencia_reloj();
		_delay_ms(200);
		
		//Establecer fotodiodos con filtro azul
		PORTD |= (1<<S3);
		PORTD &= ~(1<<S2);
		int azul= Leer_frecuencia_reloj();
		_delay_ms(200);
		
		sprintf(buffer, "rojo: %d  Verde: %d	azul: %d\r\n",
		rojo, verde, azul);
		writeString(buffer);	
			
		if ( rojo >1500 ){		// si valores dentro del rango
			//writeString("ROJO \r\n");				// muestra texto
			mover_motor(3,1);
			color_detectado=1;
		}
		else {	// si valores dentro del rango
			//writeString("DENEGADO \r\n");				// muestra texto
			color_detectado =0;
		}
		}
    }
}


//SUBRUTINAS
void setup(){
	cli();
	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3);
	//inicialmente apagado
	PORTB = 0x00;	//Estado inicial apagado
	
	//Inicializar ADC
	//Alineación a la izquierda
	//Vref=AVCC= 5V
	ADC_init(1, 2, 1, 128);
	//configurar canal 6 ADC
	ADC_CANAL(6);
	
	//Configurar como esclavo
	I2C_init_Slave(SlaveAddress);
	INIT_UART(103);
	INIT_SCOLOR();
	sei();

}

void INIT_SCOLOR(){
	//Bit 6 y 7 del puerto d como salidas
	DDRD|= (1<<DDD2)|(1<<DDD3)|(1<<DDD6)|(1<<DDD7);
	//Inicialmente apagado
	PORTD &= ~((1 << PORTD2) | (1 << PORTD3)| (1 << PORTD6)| (1 << PORTD7));

	//ENTRADAS
	DDRD &= ~(1<<DDD5);
	
	// establece frecuencia de salida
	//del modulo al 20 por ciento
	PORTD|=(1<<S0);
	PORTD &=~(1<<S1);
	INIT_UART(103);
	
}

uint16_t Leer_frecuencia_reloj(){
	TCNT1=0; //resetear contador
	//External Clock sauce T1. pin. Clock on rising edge
	TCCR1B= (1 << CS12) | (1 << CS11) | (1 << CS10);
	
	// 3. Esperar un tiempo fijo (Ventana de muestreo)
	_delay_ms(100);

	// 4. Detener el contador
	TCCR1B = 0;

	return TCNT1; // Devuelve cuántos pulsos ocurrieron en 100ms
	
}

void mover_motor(float n_vueltas, uint8_t sentido){
	//n_vueltas determina la cantidad de vueltas que se desea que de el motor
	//sentido: determina si gira hacia la izquierda o derecha
	//Velocidad: determina la velocidad de motor NOTA: no colocar tiempos menores a 2ms
	
	//Núermo de vueltas máximas que se pueden definir son 15 vueltas
	uint16_t total_pasos=(uint16_t)(n_vueltas*2048);
	
	uint16_t contador=0;
	uint8_t paso_actual=0;
	
	while (contador< total_pasos){
		// PASO 1: APAGAR los 4 pines totalmente antes de hacer nada
		PORTB &= ~((1<<PORTB0) | (1<<PORTB1) | (1<<PORTB2) | (1<<PORTB3));
		// PASO 2: ENCENDER solo los que corresponden al paso actual
		PORTB |= pasos[paso_actual];
		
		
		// Delay
		
		_delay_ms(3);
		
		
		if (sentido==1)//Sentido horario
		{
			paso_actual++;
			if (paso_actual>3)
			{
				paso_actual=0;
			}
		}
		
		else{//Sentido antihorario
			paso_actual--;
			if (paso_actual<0)
			{
				paso_actual=3;
			}
		}
		
		contador++;
	}
	// Apagar motor al finalizar para no calentar bobinas
	PORTB &= ~((1<<PORTB0) | (1<<PORTB1) | (1<<PORTB2) | (1<<PORTB3));

}


/*VECTORES DE INTERRUPCIÓN
ISR(ADC_vect){
	ValorADC = ADCH;	//Guardar el valor de adc
	ADCSRA |= (1<<ADIF);
}*/


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
			bufferi2c=TWDR;
			TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
			break;
			
		//Si Slave transmitir datos
		
		case 0xA8:	//Dirección recibida para ser leído
		case 0xB8: //Dato transmitido, ACK recibido
			TWDR = color_detectado;	//Datos a enviar
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