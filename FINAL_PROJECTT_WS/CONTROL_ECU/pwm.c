/*
 * pwm.c
 *
 *  Created on: Nov 4, 2024
 *      Author: hassan
 */
#include <avr/io.h>

void PWM_Timer0_Start(uint8_t duty_cycle)
{

	TCCR0|=(1<<WGM01)|(1<<WGM00)|(1<<COM01)|(1<<CS00)|(1<<CS02);
	DDRB|=(1<<PB3);
	OCR0=(duty_cycle*255)/100;
	TIMSK|=(1<<OCIE0);
}


