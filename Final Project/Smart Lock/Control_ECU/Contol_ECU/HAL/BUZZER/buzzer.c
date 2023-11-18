/******************************************************************************
 * Module: Buzzer
 * File Name: Buzzer.c
 * Description: source file for Buzzer driver
 * Author: Nouran Ahmed
 *******************************************************************************/
#include"buzzer.h"
#include "../../LIBRARY/common_macros.h" /* For GET_BIT Macro */
#include "../../MCAL/GPIO/gpio.h"

/*
 * Description :
 * Function responsible for initialize the Buzzer driver.
 */
void Buzzer_init() {
	/*
	 * Setup the direction for the buzzer pin as output pin through the GPIO driver.
	 * Turn off the buzzer through the GPIO.
	 */
	GPIO_setupPinDirection(BUZZER_PORT_ID, BUZZER_PIN_ID, PIN_OUTPUT);
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_LOW);
}

/*
 * Description :
 * Function to enable the Buzzer
 */
void Buzzer_on(void) {
	/*
	 * Turn on the buzzer through the GPIO.
	 */
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_HIGH);
}

/*
 * Description :
 * Function to disable the Buzzer
 */
void Buzzer_off(void) {
	/*
	 * Turn off the buzzer through the GPIO.
	 */
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_LOW);
}
