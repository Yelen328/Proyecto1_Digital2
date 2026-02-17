/*
 * PWM.c
 *
 * Created: 17/2/2026 01:03:22
 *  Author: yelen
 */ 

#define F_CPU 16000000
#include "PWM.h"
#include <avr/interrupt.h>

#define SERVO_PIN PD4

volatile uint8_t pulse_width = 10;   // 10 x 100us = 1ms
volatile uint16_t counter = 0;

void ServoTimer2_init(void)
{
	// Configurar pin como salida
	DDRD |= (1 << SERVO_PIN);

	// Timer2 modo CTC
	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22);   // Prescaler 64

	// 16MHz / 64 = 250kHz
	// 1 tick = 4us
	// 25 ticks ? 100us
	OCR2A = 24;

	TIMSK2 = (1 << OCIE2A); // Habilitar interrupción

	//sei(); ya se usa en el  main
}

void Servo_setPosition(uint8_t position)
{
	if(position == 0)       // 0°
	pulse_width = 10;   // 1ms
	else if(position == 1)  // 90°
	pulse_width = 20;   // 2ms
}

ISR(TIMER2_COMPA_vect)
{
	counter++;

	if(counter == 1)
	PORTD |= (1 << SERVO_PIN);   // Pulso alto

	if(counter == pulse_width)
	PORTD &= ~(1 << SERVO_PIN);  // Pulso bajo

	if(counter >= 200)               // 20ms
	counter = 0;
}