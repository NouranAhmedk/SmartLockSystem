/******************************************************************************
 * Module: DC Motor
 * File Name: motor.h
 * Description: header file for motor driver
 * Author: Nouran Ahmed
 *******************************************************************************/

#ifndef HAL_DC_MOTOR_MOTOR_H_
#define HAL_DC_MOTOR_MOTOR_H_

#include "../../LIBRARY/std_types.h"


/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
/* DC_Motor HW Ports and Pins Ids */
#define DC_MOTOR_FIRST_PORT_ID          PORTB_ID
#define DC_MOTOR_SECOND_PORT_ID         PORTB_ID
#define DC_MOTOR_ENABLE_PORT_ID         PORTB_ID
#define DC_MOTOR_FIRST_PIN_ID           PIN0_ID
#define DC_MOTOR_SECOND_PIN_ID          PIN1_ID
#define DC_MOTOR_ENABLE_PIN_ID          PIN3_ID


/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum {
	STOP, CW, A_CW    /* STOP : 0 0 -> 0 , ClockWise : 0 1 -> 1 , Anti Clockwise : 1  0 -> 2 */
} DcMotor_State;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Function responsible for initialize the DC_Motor driver.
 */
void DcMotor_Init(void);

/*
 * Description :
 * Function responsible for initialize the DC_Motor_Rotate driver.
 */
void DcMotor_Rotate(DcMotor_State state, uint8 speed);

#endif /* HAL_DC_MOTOR_MOTOR_H_ */
