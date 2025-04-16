/*
 * timer.h
 *
 *  Created on: Nov 7, 2024
 *      Author: hassan
 */

#ifndef TIMER_H_
#define TIMER_H_


#include "std_types.h"

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
typedef enum
{
	TIMER0_ID , TIMER1_ID , TIMER2_ID
}Timer_ID_Type;

typedef enum
{
	TIMER0_1_PRESCALER_NONE =	0,
	TIMER0_1_PRESCALER_1 = 		1,
	TIMER0_1_PRESCALER_8  = 	2,
	TIMER0_1_PRESCALER_64  = 	3,
	TIMER0_1_PRESCALER_256  =   4,
	TIMER0_1_PRESCALER_1024 = 	5,
	TIMER0_1_EXTERNAL_FALLING = 6,
	TIMER0_1_EXTERNAL_RAISING = 7,

	TIMER2_PRESCALER_NONE = 	0,
	TIMER2_PRESCALER_1 = 		1,
	TIMER2_PRESCALER_8 = 		2,
	TIMER2_PRESCALER_32 = 		3,
	TIMER2_PRESCALER_64 = 		4,
	TIMER2_PRESCALER_128 = 		5,
	TIMER2_PRESCALER_256 = 		6,
	TIMER2_PRESCALER_1024 = 	7
}Timer_ClockType;

typedef enum
{
	NORMAL_MODE = 0 , CTC_MODE = 1
}Timer_ModeType;

typedef struct
{
	uint16 timer_InitialValue;
	uint16 timer_compare_MatchValue;     /*it will be used in compare mode only*/
	Timer_ID_Type  timer_ID;
	Timer_ClockType timer_clock;
	Timer_ModeType  timer_mode;
}Timer_ConfigType;

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/
void Timer_init(const Timer_ConfigType * Config_Ptr);
void Timer_deInit(Timer_ID_Type timer_type);
void Timer_setCallBack(void(*a_ptr)(void), Timer_ID_Type a_timer_ID );

#endif /* TIMER_H_ */
