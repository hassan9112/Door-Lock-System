/*
 * timer.c
 *
 *  Created on: Nov 7, 2024
 *      Author: Hassan
 */

#include "timer.h"
#include "common_macros.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
static volatile void (*g_callBackPtr_TIMER0)(void) = NULL_PTR;
static volatile void (*g_callBackPtr_TIMER1)(void) = NULL_PTR;
static volatile void (*g_callBackPtr_TIMER2)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER0_COMP_vect)
{
	if(g_callBackPtr_TIMER0 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr_TIMER0)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER0_OVF_vect)
{
	if(g_callBackPtr_TIMER0 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr_TIMER0)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr_TIMER1 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr_TIMER1)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr_TIMER1 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr_TIMER1)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER2_OVF_vect)
{
	if(g_callBackPtr_TIMER2 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr_TIMER2)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER2_COMP_vect)
{
	if(g_callBackPtr_TIMER2 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr_TIMER2)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void Timer_init(const Timer_ConfigType * Config_Ptr)
{
	if(Config_Ptr->timer_ID==TIMER0_ID)
	{
		TCNT0 = Config_Ptr->timer_InitialValue ; /* Set Timer Initial Value */
		OCR0  = Config_Ptr->timer_compare_MatchValue;  /* Set Compare value */
		/* Configure timer control register
		 * 1. Non_pwm mode FOC0=1
		 * 2. WGM00 = 0 , WGM01=1 for ctc and 0 for normal
		 * 3. OC0 DISABLED
		 * 4. clock = Configurable
		 */
		TCCR0 |= (1<<FOC0)|(Config_Ptr->timer_mode<<WGM01)|(Config_Ptr->timer_clock<<CS00);
		/*enable interrupt*/
		TIMSK |=(1<<(Config_Ptr->timer_mode));
	}
	else if(Config_Ptr->timer_ID==TIMER1_ID)
	{
		TCNT1 = Config_Ptr->timer_InitialValue ; /* Set Timer Initial Value */
		OCR1A  =Config_Ptr->timer_compare_MatchValue;  /* Set Compare value */
		/* Configure timer control register
		 * 1. Non_pwm mode FOC1B=1
		 * 2. WGM10,WGM11,WGM13=0 & WGM12=1 for ctc and 0 for normal
		 * 3. OC DISABLED
		 * 4. clock = Configurable
		 */
		TCCR1A |= (1<<FOC1A) ;
		TCCR1B |= (Config_Ptr->timer_mode<<WGM12)|(Config_Ptr->timer_clock<<CS10);
		/*enable interrupt*/
		if (Config_Ptr->timer_mode == NORMAL_MODE)
		{
			TIMSK |= (1 << TOIE1);
		}
		else if (Config_Ptr->timer_mode == CTC_MODE)
		{
			TIMSK |= (1 << OCIE1A);
		}

	}
	else if(Config_Ptr->timer_ID==TIMER2_ID)
	{
		TCNT2= Config_Ptr->timer_InitialValue ; /* Set Timer Initial Value */
		OCR2  =Config_Ptr->timer_compare_MatchValue;  /* Set Compare value */
		/* Configure timer control register
		 * 1. Non_pwm mode FOC2=1
		 * 2. WGM20 = 0 , WGM21=1 for ctc and 0 for normal
		 * 3. OC2 DISABLED
		 * 4. clock = Configurable
		 */
		TCCR2 |= (1<<FOC2)|(Config_Ptr->timer_mode<<WGM21)|(Config_Ptr->timer_clock<<CS20);
		/*enable interrupt*/
		TIMSK |=(1<<(TOIE2+(Config_Ptr->timer_mode)));
	}
}
void Timer_deInit(Timer_ID_Type timer_type)
{
	if(timer_type==TIMER0_ID)
	{
		// Stop the timer by setting the prescaler to 0
		TCCR0 = 0;
		// Disable Timer0 interrupts
		TIMSK &= ~(1 << TOIE0);
		TIMSK &= ~(1 << OCIE0);
		//clear the counter register
		TCNT0 = 0 ;
		OCR0  = 0 ;
	}
	else if(timer_type==TIMER1_ID)
	{
		// Stop the timer
		TCCR1A = 0;
		TCCR1B = 0;
		// Disable Timer1 interrupts
		TIMSK &= ~(1 << TOIE1);
		TIMSK &= ~(1 << OCIE1B);
		// Clear registers
		TCNT1 = 0;
		OCR1A = 0;
	}
	else if(timer_type==TIMER2_ID)
	{
		// Stop the timer
		TCCR2 = 0;
		// Disable Timer2 interrupts
		TIMSK &= ~(1 << TOIE2);
		TIMSK &= ~(1 << OCIE2);
		// Clear registers
		TCNT2 = 0;
		OCR2 = 0;
	}
}
void Timer_setCallBack(void(*a_ptr)(void), Timer_ID_Type a_timer_ID )
{
	if(a_timer_ID==TIMER0_ID)
	{
		g_callBackPtr_TIMER0 = a_ptr;
	}
	else if(a_timer_ID==TIMER1_ID)
	{
		g_callBackPtr_TIMER1 = a_ptr;
	}
	else if(a_timer_ID==TIMER2_ID)
	{
		g_callBackPtr_TIMER2 = a_ptr;
	}
}
