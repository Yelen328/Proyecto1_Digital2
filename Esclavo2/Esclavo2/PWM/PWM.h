/*
 * PWM.h
 *
 * Created: 16/02/2026 06:47:32
 *  Author: Lenovo
 */ 


#ifndef PWM_H_
#define PWM_H_
#include <avr/io.h>

#include <avr/io.h>

void ServoTimer2_init(void);
void Servo_setPosition(uint8_t position); // 0 = 0°, 1 = 90°




#endif /* PWM_H_ */