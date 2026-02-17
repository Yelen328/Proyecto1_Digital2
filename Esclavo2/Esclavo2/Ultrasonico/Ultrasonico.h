/*
 * Ultrasonico.h
 *
 * Created: 15/02/2026 01:05:27
 *  Author: Lenovo
 */ 


#ifndef ULTRASONICO_H_
#define ULTRASONICO_H_

#ifndef F_CPU
#define F_CPU 16000000
#endif
//Librerias
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>

// Configuración del sensor
#define TRIGGER_PIN   PD3   // Pin de trigger (PORTD3)
#define ECHO_PIN      PD2    // Pin de eco (PORTD2)
//#define TIMEOUT_TICKS 60000   // Timeout de seguridad

// Estados del sensor
typedef enum {
	SENSOR_LISTO = 0,        // Listo para trigger
	SENSOR_ESPERANDO_ECO = 1,     // Esperando flanco de subida
	SENSOR_MIDIENDO = 2,          // Esperando flanco de bajada
	SENSOR_LECTURA_COMPLETA = 3    // Lectura disponible
} EstadoSensor;

// Estructura principal del sensor
typedef struct {
	uint8_t trigger; //Activa la señal del trigger
	volatile uint16_t pulse_eco; //Guarda cuanto duró el pulso del eco
	volatile uint16_t inicio_eco; //Tiempo cuando el eco empieza a recibir
	volatile uint8_t estado_sensor; //guarda el estado del sensor
	uint16_t distancia; //Conversión del tiempo a distancia
	uint8_t distancia_master; //distancia convertida a valores de 0 a 255 para mandarla al maestro por I2C
} Ultrasonico;

// PROTOTIPOS DE FUNCIONES
void ultrasonico_init(Ultrasonico *sensor);
void ultrasonico_trigger(Ultrasonico *sensor);
uint8_t ultrasonico_lectura_disponible(Ultrasonico *sensor);
uint16_t ultrasonico_get_distancia(Ultrasonico *sensor);
uint8_t ultrasonico_get_distancia_map(Ultrasonico *sensor);
void ultrasonico_procesar_eco(Ultrasonico *sensor, uint8_t echo_state);
void ultrasonico_reiniciar(Ultrasonico *sensor); 


#endif /* ULTRASONICO_H_ */