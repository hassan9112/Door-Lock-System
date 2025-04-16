 /******************************************************************************
 *
 * Module: DCmotor
 *
 * File Name: dcmotor.c
 *
 * Description: Source file for the ATmega16 DCmotor driver
 *
 * Author: Medhat Adel Tawfik
 *
 *******************************************************************************/

#include "DCMotor.h"
#include "avr/io.h"
#include "gpio.h"
#include"pwm.h"

void DcMotor_Init(void)
{
    GPIO_setupPinDirection(MOTOR_PORT, MOTOR_PIN1, PIN_OUTPUT);
    GPIO_setupPinDirection(MOTOR_PORT, MOTOR_PIN2, PIN_OUTPUT);
    GPIO_writePin(MOTOR_PORT, MOTOR_PIN1, LOGIC_LOW);
    GPIO_writePin(MOTOR_PORT, MOTOR_PIN2, LOGIC_LOW);
}

void DcMotor_Rotate(DcMotor_State state, uint8 speed)
{
    if(state == CW) {
        GPIO_writePin(MOTOR_PORT, MOTOR_PIN1, LOGIC_HIGH);
        GPIO_writePin(MOTOR_PORT, MOTOR_PIN2, LOGIC_LOW);
    } else if(state == A_CW) {
        GPIO_writePin(MOTOR_PORT, MOTOR_PIN1, LOGIC_LOW);
        GPIO_writePin(MOTOR_PORT, MOTOR_PIN2, LOGIC_HIGH);
    } else {
        GPIO_writePin(MOTOR_PORT, MOTOR_PIN1, LOGIC_LOW);
        GPIO_writePin(MOTOR_PORT, MOTOR_PIN2, LOGIC_LOW);
    }
    PWM_Timer0_Start(speed);
}
