/******************************************************************************
 * Module: Timer 1
 * File Name: timer1.h
 * Description: header file for timer1
 * Author: Nouran Ahmed
 *******************************************************************************/
#ifndef MCAL_TIMER1_TIMER1_H_
#define MCAL_TIMER1_TIMER1_H_

#include "../../LIBRARY/std_types.h"
#include "../GPIO/gpio.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum {
	NO_CLK,
	CLK_1,
	CLK_8,
	CLK_64,
	CLK_256,
	CLK_1024,
	EXTERNAL_CLK_FALLING_EDGE,
	EXTERNAL_CLK_RISING_EDGE
} Timer1_Prescaler;

typedef enum {
	Normal_Mode, Compare_Mode
} Timer1_Mode;

typedef struct {
	uint16 initial_value;
	uint16 compare_value; /*It will be used in compare mode only.*/
	Timer1_Prescaler prescaler;
	Timer1_Mode mode;
} Timer1_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description:
 * Function to initialize the Timer driver
 * Setup the initial value of counter
 * Setup Normal mode or Compare mode
 * Setup Prescaler
 */
void Timer1_init(const Timer1_ConfigType *Config_Ptr);

/*
 * Description:
 * Functional to disable timer1
 */
void Timer1_deInit(void);

/*
 * Description: Function to set the Call Back function address.
 */
void Timer1_setCallBack(void (*a_ptr)(void));

#endif /* MCAL_TIMER1_TIMER1_H_ */
