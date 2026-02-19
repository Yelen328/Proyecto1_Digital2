/*
 * LDC.h
 *
 * Created: 27/1/2026 01:50:34
 *  Author: yelen
 */ 


#ifndef LDC_H_
#define LDC_H_

#define  F_CPU	16000000
#include "util/delay.h"
#include <avr/io.h>

//inicializa LDC
void initLDC8(void);

//Coloca un dato en el puerto
void LDC_port (uint8_t valor);

//Envia un comando
void LDC_CMD(char a);

//Función para enviar un caracter
void LDC_write_char(char caracter);

//Función para enviar una cadena
void LDC_write_string(char *cadena);

//Desplazamiento a la derecha
void LDC_Desplazamiento_derecha(void);

//Desplazamiento a la izquierda
void LDC_Desplazamiento_izquierda(void);

//Establecer Cursor 
void LDC_CURSOR(char c, char f);
#endif /* LDC_H_ */