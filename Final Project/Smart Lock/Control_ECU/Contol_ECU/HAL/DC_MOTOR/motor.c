/******************************************************************************
 * Module: DC Motor
 * File Name: motor.c
 * Description: source file for motor driver
 * Author: Nouran Ahmed
 *******************************************************************************/

#include"motor.h"
#include"../../MCAL/TIMER0_PWM/pwm.h"
#include "../../LIBRARY/common_macros.h" /* For GET_BIT Macro */
#include "../../MCAL/GPIO/gpio.h"

/*
 * Description :
 * Function responsible for initialize the DC_Motor driver.
 */
void DcMotor_Init(void) {
	GPIO_setupPinDirection(DC_MOTOR_FIRST_PORT_ID , DC_MOTOR_FIRST_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOTOR_SECOND_PORT_ID, DC_MOTOR_SECOND_PIN_ID, PIN_OUTPUT);
	GPIO_writePin(DC_MOTOR_FIRST_PORT_ID , DC_MOTOR_FIRST_PIN_ID, LOGIC_LOW);
	GPIO_writePin(DC_MOTOR_SECOND_PORT_ID, DC_MOTOR_SECOND_PIN_ID, LOGIC_LOW);
}

/*
 * Description :
 * Function responsible for initialize the DC_Motor_Rotate driver.
 */
void DcMotor_Rotate(DcMotor_State state, uint8 speed){
	switch(state){
	case STOP:
		GPIO_writePin(DC_MOTOR_FIRST_PORT_ID, DC_MOTOR_FIRST_PIN_ID, LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_SECOND_PORT_ID, DC_MOTOR_SECOND_PIN_ID, LOGIC_LOW);
		break;
	case CW:
		GPIO_writePin(DC_MOTOR_FIRST_PORT_ID , DC_MOTOR_FIRST_PIN_ID, LOGIC_HIGH);
		GPIO_writePin(DC_MOTOR_SECOND_PORT_ID, DC_MOTOR_SECOND_PIN_ID, LOGIC_LOW);
		break;
	case A_CW:
		GPIO_writePin(DC_MOTOR_FIRST_PORT_ID , DC_MOTOR_FIRST_PIN_ID, LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_SECOND_PORT_ID, DC_MOTOR_SECOND_PIN_ID, LOGIC_HIGH);
		break;
	}

	PWM_Timer0_Start(speed);

}
