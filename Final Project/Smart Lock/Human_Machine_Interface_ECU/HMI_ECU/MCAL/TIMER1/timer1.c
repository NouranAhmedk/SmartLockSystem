/******************************************************************************
 * Module: Timer 1
 * File Name: timer1.c
 * Description: source file for timer1
 * Author: Nouran Ahmed
 *******************************************************************************/
#include "timer1.h"
#include <avr/interrupt.h>
#include<avr/io.h>/* To use the Timer1 Registers */

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
ISR(TIMER1_OVF_vect) {
	if (g_callBackPtr != NULL_PTR) {
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER1_COMPA_vect) {
	if (g_callBackPtr != NULL_PTR) {
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

/*
 * Description:
 * Function to initialize the Timer driver
 * Setup the initial value of counter
 * Setup Normal mode or Compare mode
 * Setup Prescaler
 */
void Timer1_init(const Timer1_ConfigType *Config_Ptr) {

	/*
	 * the FOC1A/FOC1B bits are only functional in non-PWM modes.
	 */
	TCCR1A = (1 << FOC1A) | (1 << FOC1B);

	/*
	 * insert the required clock value in the first three bits (CS10, CS11 and CS12)
	 * of TCCR1B Register
	 */

	TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr->prescaler);
	/*
	 * start count from initial value
	 */
	TCNT1 = Config_Ptr->initial_value;

	/*Compare Mode*/
	if ((Config_Ptr->mode) == Compare_Mode) {

		OCR1A = Config_Ptr->compare_value; /* Set Compare Value */
		TIMSK = (1 << OCIE1A); /* Enable Timer1 Compare Interrupt */
		TCCR1B |= (1 << WGM12); /* Select WGM12 = 1 and WGM11 = 0 and WGM10 = 0  (Mode Number 4) */

	}
	/*Normal Mode*/
	else if ((Config_Ptr->mode) == Normal_Mode) {

		TIMSK = (1 << TOIE1); /* Enable Timer1 Overflow Interrupt */
	}
}

/*
 * Description:
 * Functional to disable timer1
 */
void Timer1_deInit(void) {
	/*Clear the time registers*/
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	OCR1A = 0;

	/* Disable the  Overflow interrupt and Compare interrupt at OCIE1A*/
	TIMSK &= 0xEB ; /*(0b11101011)*/

	/* Reset the global pointer value */
	g_callBackPtr = NULL_PTR;
}

/*
 * Description: Function to set the Call Back function address.
 */
void Timer1_setCallBack(void (*a_ptr)(void)) {
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}
