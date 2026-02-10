/*UNIVERSIDAD DEL VALLE DE GUATEMALA
 * I2C.h
 *Esta documento tiene el objetivo de establecer los prototipso de funión para 
 implementar la librería de I2C
 * Created: 10/2/2026 00:16:06
 *  Author: yelena Cotzojay
 */ 


#ifndef I2C_H_
#define I2C_H_

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <stdint.h>

//Función para inicializar I2C Maestro
void I2C_Master_Init(unsigned long SCL_Clock, uint8_t prescaler);

uint8_t I2C_Start(void);									//Funcion para iniciar conversacion (el Master Habla)
uint8_t I2C_repeatedStart(void);							//Funcion para seguir conversacion (el master escucha)
void I2C_Master_stop(void);										//Funcion para parar comunicacion

//Función de transmisión de datos del maestro al esclavo
//esta función devolverá un 0 si el esclavo ah recibido 
//el dato
uint8_t I2C_Master_write(uint8_t dato);	

//Función de recepción de datos enviados por el esclavo al maestro 
//esta función es para leer los datos que estáne en el esclavo					
uint8_t I2C_Mater_read(uint8_t buffer, uint8_t ack);

//función para inicializar I2C esclavo
void I2C_init_Slave(uint8_t address);

#endif /* I2C_H_ */