/******************************************************************************
 * Module: Buzzer
 * File Name: Buzzer.h
 * Description: header file for Buzzer driver
 * Author: Nouran Ahmed
 *******************************************************************************/
#ifndef HAL_BUZZER_BUZZER_H_
#define HAL_BUZZER_BUZZER_H_


/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
/* Buzzer HW Ports and Pins Ids */
#define BUZZER_PORT_ID         PORTB_ID
#define BUZZER_PIN_ID           PIN5_ID

/*******************************************************************************
 *                            Functions Prototypes                             *
 *******************************************************************************/

/*
 * Description :
 * Function responsible for initialize the Buzzer driver.
 */
void Buzzer_init();

/*
 * Description :
 * Function to enable the Buzzer
 */
void Buzzer_on(void);

/*
 * Description :
 * Function to disable the Buzzer
 */
void Buzzer_off(void);


#endif /* HAL_BUZZER_BUZZER_H_ */
