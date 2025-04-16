/*
 * pwm.c
 *
 *  Created on: Nov 4, 2024
 *      Author: hassan
 */
#include <avr/io.h>

void PWM_Timer0_Start(uint8_t duty_cycle)
{
    // Set OC0 (PD6) as output pin for PWM signal
    DDRB |= (1 << PB3);  // PB3 is OC0 on some ATmega microcontrollers (e.g., ATmega16/32)

    // Set Fast PWM mode (WGM01 = 1, WGM00 = 1)
    TCCR0 |= (1 << WGM00) | (1 << WGM01);

    // Set Non-inverting mode (COM01 = 1, COM00 = 0)
    TCCR0 |= (1 << COM01);
    TCCR0 &= ~(1 << COM00);

    // Set the prescaler to F_CPU/64 (CS01 = 1, CS00 = 1, CS02 = 0)
    TCCR0 |= (1 << CS01) | (1 << CS00);
    TCCR0 &= ~(1 << CS02);

    // Set the duty cycle (OCR0 register)
    OCR0 = (duty_cycle * 255) / 100;
}


