/******************************************************************************
 * Module: Pulse width modulation
 * File Name: pwm.h
 * Description: header file for PWM
 * Author: Nouran Ahmed
 *******************************************************************************/

#ifndef MCAL_TIMER0_PWM_PWM_H_
#define MCAL_TIMER0_PWM_PWM_H_

#include "../../LIBRARY/std_types.h"
#include "../GPIO/gpio.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
/* PWM Ports and Pins Ids */
#define PWM_TIMER0_PORT_ID  PORTB_ID /*Configure PB3/OC0 as output pin --> pin where the PWM signal is generated from MC*/
#define PWM_TIMER0_PIN_ID   PIN3_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description:
 * Starts the PWM (Pulse Width Modulation) output using Timer0 with the specified duty cycle.
 * The function sets up the necessary configurations and starts generating PWM signals.
 * Parameters:
 * duty_cycle: The duty cycle value for the PWM output. Should be between 0 and 255.
 */
void PWM_Timer0_Start(uint8 duty_cycle);

#endif /* MCAL_TIMER0_PWM_PWM_H_ */
