/*
 * pir.h
 *
 *  Created on: Nov 4, 2024
 *      Author: hassa
 */

#ifndef PIR_H_
#define PIR_H_
#include <avr/io.h>
#include <util/delay.h>

// Define the pin connected to the PIR sensor (e.g., PB0)
#define PIR_PIN PC2

// Define the PORT and DDR for the PIR sensor
#define PIR_PORT PORTC
#define PIR_DDR DDRC
#define PIR_INPUT PINC

void PIR_init(void);

uint8_t PIR_getState(void);


#endif /* PIR_H_ */
