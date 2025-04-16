/*
 * pir.c
 *
 *  Created on: Nov 4, 2024
 *      Author: hassa
 */
#include"pir.h"
void PIR_init(void) {
    // Set the PIR_PIN as input
    PIR_DDR &= ~(1 << PIR_PIN);  // Clear the corresponding bit to configure as input
    PIR_PORT |= (1 << PIR_PIN);  // Enable pull-up resistor (optional depending on sensor type)
}

// Function to return PIR State
uint8_t PIR_getState(void)
{
    // Read the state of the PIR_PIN
    return (PIR_INPUT & (1 << PIR_PIN)) ? 1 : 0; // Returns 1 if HIGH (motion detected), 0 if LOW (no motion)
}

