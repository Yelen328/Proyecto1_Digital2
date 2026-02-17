/*
 * sTemperatura.h
 *
 * Created: 16/2/2026 00:49:28
 *  Author: yelen
 */ 


#ifndef STEMPERATURA_H_
#define STEMPERATURA_H_	16000000

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>   // librería de delays

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(uint8_t data);
uint8_t i2c_read_ack(void);
uint8_t i2c_read_nack(void);
void AHT10_begin(void);
void AHT10_Read(float *hum, float *temp);



#endif /* STEMPERATURA_H_ */