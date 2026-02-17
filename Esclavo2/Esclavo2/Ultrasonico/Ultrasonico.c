/*
 * Ultrasonico.c
 *
 * Created: 15/02/2026 01:05:04
 *  Author: Daniela Moreira 
 */ 

#include "Ultrasonico.h"
#ifndef F_CPU
#define F_CPU 16000000
#endif
//Librerias
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>

// Inicialización del sensor
void ultrasonico_init(Ultrasonico *sensor) {
	// Configurar pines
	DDRD |= (1 << TRIGGER_PIN);      // Trigger como salida
	DDRD &= ~(1 << ECHO_PIN);         // Echo como entrada
	
	PORTD &= ~(1 << TRIGGER_PIN);     // Trigger en LOW inicialmente
	
	// Inicializar variables
	sensor->trigger = 1;
	sensor->pulse_eco = 0;
	sensor->inicio_eco = 0;
	sensor->estado_sensor = SENSOR_LISTO;
	sensor->distancia = 0;
	sensor->distancia_master = 0;
}

// Disparar el sensor ultrasónico
void ultrasonico_trigger(Ultrasonico *sensor) {
	if (sensor->estado_sensor == SENSOR_LISTO && sensor->trigger) {
		// Resetear contador del timer
		TCNT1 = 0;
		
		// Generar pulso de trigger
		_delay_us(2);  // Pequeña pausa para estabilidad
		PORTD |= (1 << TRIGGER_PIN);
		_delay_us(10); //generar el pulso de 10 us
		PORTD &= ~(1 << TRIGGER_PIN);
		
		sensor->estado_sensor = SENSOR_ESPERANDO_ECO;
		sensor->trigger = 0; //Se apaga el pulso luego de hacer el pulso
	}
}

// Procesar el eco (debe llamarse desde la ISR)
void ultrasonico_procesar_eco(Ultrasonico *sensor, uint8_t echo_state) {
	if (echo_state && sensor->estado_sensor == SENSOR_ESPERANDO_ECO) {
		// Flanco de subida - comenzó el eco
		sensor->inicio_eco = TCNT1; //guarda el valore del tiempo en el que se empezó a recibir el eco
		sensor->estado_sensor = SENSOR_MIDIENDO;
	}
	else if (!echo_state && sensor->estado_sensor == SENSOR_MIDIENDO) {
		// Flanco de bajada - terminó el eco
		sensor->pulse_eco = TCNT1 - sensor->inicio_eco; //hace una resta entre el inicio del eco y el final del eco para determinar el tiempo de duración del pulso
		sensor->estado_sensor = SENSOR_LECTURA_COMPLETA;
		
		// Calcular distancia con el tiempo del eco
		uint16_t tiempo_us = sensor->pulse_eco * 0.5;  // 0.5 µs por tick de la configuración del TM1
		sensor->distancia = tiempo_us / 58;  // Fórmula estándar: de us a cm  us/58 = cm
		
		// Limitar a máximo 50cm
		if (sensor->distancia> 400) {
			sensor->distancia = 400;
		}
		
		// Mapear a 0-255
		sensor->distancia_master= ((sensor->distancia * 255UL) / 400UL);
	}
}
//Funciones para devolver valores 
// Verificar si hay una nueva lectura disponible
uint8_t ultrasonico_lectura_disponible(Ultrasonico *sensor) { //si hay una medición lista devuelve 1, si no la hay devuelve 0. Funciona como una bandera. 
	return (sensor->estado_sensor == SENSOR_LECTURA_COMPLETA);
}

// Obtener última distancia en cm
uint16_t ultrasonico_get_distancia(Ultrasonico *sensor) {
	return sensor->distancia;
}

// Obtener última distancia mapeada (0-255)
uint8_t ultrasonico_get_distancia_map(Ultrasonico *sensor) {
	return sensor->distancia_master;
}

// Reiniciar sensor para nueva medición (llamar después de procesar)
void ultrasonico_reiniciar(Ultrasonico *sensor) {
	sensor->estado_sensor = SENSOR_LISTO;
	sensor->trigger = 1;
}