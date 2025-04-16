 /******************************************************************************
 *
 * Module: Buzzer
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the ATmega16 Buzzer driver
 *
 * Author: Medhat Adel Tawfik
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"

// Function to initialize the buzzer
void Buzzer_init(void);

// Function to turn the buzzer on
void Buzzer_on(void);

// Function to turn the buzzer off
void Buzzer_off(void);

#endif /* BUZZER_H_ */
