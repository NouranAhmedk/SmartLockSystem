/******************************************************************************
 * Project: Smart Lock System
 * File Name: main.c
 * Description: Control ECU Application layer
 * Author: Nouran Ahmed
 *******************************************************************************/
#include"../HAL/BUZZER/buzzer.h"
#include"../HAL/DC_MOTOR/motor.h"
#include"../HAL/EEPROM/external_eeprom.h"
#include"../MCAL/I2C/twi.h"
#include"../MCAL/TIMER1/timer1.h"
#include"../LIBRARY/std_types.h"
#include"../LIBRARY/atmega_32.h"
#include <util/delay.h>
#include "../MCAL/UART/uart.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define PASSWORD_SIZE            5
#define KEY_PRESSED_DELAY       500
#define MEDIUM_DELAY            300
#define SMALL_DELAY             10
#define CLK_WISE_SECONDS        15
#define ANTI_CLK_WISE_SECONDS   15
#define IDLE_SECONDS            3
#define BUZZER_SECONDS          60
#define UART_START				0x01
#define INCORRECT_PASSWORD		0x02
#define CORRECT_PASSWORD		0x03
#define DOOR_STATE				0x04
#define UART_CHECKING_PASS      0x05
#define OPEN_DOOR_PASSWORD		0x06
#define CHOOSEN_OPTION          0x07
#define DOOR_PASSWORD_STATE     0x08
#define CHANGE_PASSWORD_STATE   0x09
/*******************************************************************************
 *                           Functions Prototypes                              *
 *******************************************************************************/
void ReceiveTwoPasswords(uint8 *password1, uint8 *password2);
uint8 ComparePasswords(uint8 *password1, uint8 *password2,uint8 *matchedPassword);
uint8 CompareSavedPassword(uint8 *password1, uint8 *matchedPassword);
void EEPROM_write_Password(uint8 *Str);
void EEPROM_read_Password(uint8 *Str);
void ReceiveDoorPassword(uint8 *password1);
void timer1_tick(void);
void timer1_delay(uint8 seconds);
void mainOpertaionsOptions(void);
void IncorrectOptions(void);
/*******************************************************************************
 *                                   Main                                      *
 *******************************************************************************/
volatile uint8 timer1_Ticks = 0;
volatile uint8 trial_flag = 3, exit_flag = 1;
int main(void) {
	SREG_REG.bits.I_BIT = 1; /*Enable I-bit by bit register field*/

	uint8 receivedPassword1[PASSWORD_SIZE];
	uint8 receivedPassword2[PASSWORD_SIZE];
	uint8 matchedPassword[PASSWORD_SIZE];
	uint8 comparePasswordsResult;
	uint8 choosenOption;
	uint8 doorPasswordsResultPlus , doorPasswordsResultMinus;


	/* UART description:
	 * The configuration of UART:
	 * 8-bit data
	 * parity check is disabled
	 * stop bit is one bit
	 * baud rate is 9600
	 */
	UART_ConfigType UART_config = { EIGHT_bit, Disabled, ONE_bit, BR3 };
	UART_init(&UART_config);

	/*
	 * TWI description:
	 * Bit Rate: 400.000 Kbps using F_CPU=8MHz
	 * Prescaler/1
	 * Interrupt OFF
	 * Acknowledge bit OFF
	 */
	TWI_ConfigType TWI_Config = { SLAVE1, Fast_mode_400k, TWI_CLK_1 };
	TWI_init(&TWI_Config);
	/*******************************************************************************
	 *                           System Modules Initializations                    *
	 *******************************************************************************/
	DcMotor_Init();
	Buzzer_init();
	while (1) {

		/*Step 1 part 1: Receive both passwords via UART*/
		ReceiveTwoPasswords(receivedPassword1, receivedPassword2);
		_delay_ms(MEDIUM_DELAY);

		/*Step 1 part 2: Compare the two passwords and take appropriate action*/
		comparePasswordsResult = ComparePasswords(receivedPassword1, receivedPassword2, matchedPassword);
		_delay_ms(KEY_PRESSED_DELAY);

		switch (comparePasswordsResult) {
		case CORRECT_PASSWORD:
			/*Step 1 part 3: In case the two Passwords are matched save password in EEPROM*/
			/*Send password1 via UART*/
			EEPROM_write_Password(receivedPassword1);

			/* Getting saved password from EEPROM*/
			EEPROM_read_Password(matchedPassword);
			_delay_ms(MEDIUM_DELAY);

			/* Send flag to M1 that is passwords are matched*/
			while (UART_recieveByte() != UART_CHECKING_PASS);
			UART_sendByte(CORRECT_PASSWORD);
			_delay_ms(KEY_PRESSED_DELAY);
			exit_flag = 1;

			/* Step 2 is ignored because its done in M1 [HMI]*/
			while( trial_flag > 0 && exit_flag) {

				/* Step 3 part 2: Receive door state ( + only or - only )*/
				UART_sendByte(CHOOSEN_OPTION);
				choosenOption = UART_recieveByte();
				_delay_ms(SMALL_DELAY);

				/* Step 3 part 3: Receive door password in case ( + or - )*/
				ReceiveDoorPassword(receivedPassword1);
				_delay_ms(KEY_PRESSED_DELAY);


				/* Step 3 part 4: Compare the passwords and saved password which is matched array*/
				doorPasswordsResultPlus = CompareSavedPassword(receivedPassword1 , matchedPassword);
				doorPasswordsResultMinus = CompareSavedPassword(receivedPassword1 , matchedPassword);

				if(choosenOption == DOOR_PASSWORD_STATE){
					switch (doorPasswordsResultPlus) {
					case CORRECT_PASSWORD:
						/* Send flag to M1 that is password and saved password are matched*/
						while (UART_recieveByte() != DOOR_STATE);
						UART_sendByte(CORRECT_PASSWORD);
						mainOpertaionsOptions();
						trial_flag = 3;
						exit_flag = 1;

						break;
					case INCORRECT_PASSWORD:
						/* Send flag to M1 that is password and saved password are matched*/
						while (UART_recieveByte() != DOOR_STATE);
						UART_sendByte(INCORRECT_PASSWORD);
						 /* Certain times of trials that has been applied form HCI to CONTROL_ECU in order to make both at same flow
						  * if the trails end [reach to zero] the buzzer will be on for 60 seconds*/
						trial_flag--;
						if(trial_flag == 0){
							IncorrectOptions();
							trial_flag = 3;
							exit_flag = 1;
						}
						break;
					}
				}else if (choosenOption == CHANGE_PASSWORD_STATE){
					switch (doorPasswordsResultMinus) {
					case CORRECT_PASSWORD:
						/* Send flag to M1 that is password and saved password are matched*/
						while (UART_recieveByte() != DOOR_STATE);
						UART_sendByte(CORRECT_PASSWORD);
						exit_flag = 0;

						break;
					case INCORRECT_PASSWORD:
						/* Send flag to M1 that is password and saved password are matched*/
						while (UART_recieveByte() != DOOR_STATE);
						UART_sendByte(INCORRECT_PASSWORD);
						 /* Certain times of trials that has been applied form HCI to CONTROL_ECU in order to make both at same flow
						  * if the trails end [reach to zero] the buzzer will be on for 60 seconds*/
						trial_flag--;
						if(trial_flag == 0){
							IncorrectOptions();
							trial_flag = 3;
							exit_flag = 1;
						}
						break;
					}
				}
			}
			break;
		 case INCORRECT_PASSWORD:
			/*Step 1 part 3: In case the two Passwords are not matched go to step 1 in HMI*/
			/* Send flag to M1 that is passwords are matched*/
			while (UART_recieveByte() != UART_CHECKING_PASS);
			UART_sendByte(INCORRECT_PASSWORD);
			break;
		}

	}

	return 0;
}

/*******************************************************************************
 *                    FUNCTION DEFINITION FOR TIMER                            *
 *******************************************************************************/
void timer1_tick(void) {
	timer1_Ticks++;
}

void timer1_delay(uint8 seconds) {
	timer1_Ticks = 0; // Reset the timer
	/*
	 * Timer1 Configuration
	 * TCNT1 = 0:		Initial value
	 * Prescaler: 		F_CPU/256
	 * Operation mode: 	Compare mode
	 * OCR1A = 31250	Compare value
	 * Generates an interrupt every 1 second
	 */

	Timer1_ConfigType TIMER1_Config = { 0, 31250, CLK_256, Compare_Mode };
	/* Reinitialize the timer */
	Timer1_init(&TIMER1_Config);
	Timer1_setCallBack(timer1_tick);
	while (timer1_Ticks != seconds);
	Timer1_deInit();
}

/*******************************************************************************
 *            FUNCTION DEFINITION FOR RECIEVING PASSWORDS FROM USER            *
 *******************************************************************************/

/*
 * ReceiveTwoPasswords function receives two passwords from M1 via UART.
 */
void ReceiveTwoPasswords(uint8 *password1, uint8 *password2) {
	uint8 i;

	/* Send MC2_READY byte to MC1 to ask it to send the string */
	UART_sendByte(UART_START);

	/* Receive password1 via UART */
	for (i = 0; i < PASSWORD_SIZE; i++) {
		password1[i] = UART_recieveByte();
	}

	/* Receive password2 via UART */
	for (i = 0; i < PASSWORD_SIZE; i++) {
		password2[i] = UART_recieveByte();
	}
	_delay_ms(KEY_PRESSED_DELAY ); /* Add a short delay after receiving passwords */
}

/*******************************************************************************
 *     FUNCTION DEFINITION FOR COMPARING PASSWORDS  [received 1 , received 2]  *
 *******************************************************************************/

/*
 * ComparePasswords function checks if two passwords match.
 * Returns 1 if they match, 0 if they don't match.
 */
uint8 ComparePasswords(uint8 *password1, uint8 *password2,
		uint8 *matchedPassword) {
	uint8 i;
	for (i = 0; i < PASSWORD_SIZE; i++) {
		if (password1[i] != password2[i]) {
			return INCORRECT_PASSWORD; /* Passwords do not match */
		}
		matchedPassword[i] = password1[i];
	}
	return CORRECT_PASSWORD; /* Passwords match */
}

/*******************************************************************************
 *       FUNCTION DEFINITION FOR SAVING IN EEPROM AND CHECKING IN EEPROM       *
 *******************************************************************************/
/*
 * EEPROM_write_Password function takes the user given password and writes it
 * in the EEPROM
 */
void EEPROM_write_Password(uint8 *password) {
	uint8 i;
	for (i = 0; i < PASSWORD_SIZE; i++) {
		EEPROM_writeByte(0x01 + i, password[i]);
		_delay_ms(MEDIUM_DELAY);
	}
}

/*
 * EEPROM_read_Password function reads the password saved in EEPROM
 * and returns it in a given array
 */
void EEPROM_read_Password(uint8 *password) {
	uint8 EEPROM_Read_Byte, i;
	for (i = 0; i < PASSWORD_SIZE; i++) {
		EEPROM_readByte(0x01 + i, &EEPROM_Read_Byte);
		password[i] = EEPROM_Read_Byte;
		_delay_ms(MEDIUM_DELAY);
	}
}

/*******************************************************************************
 *              FUNCTION DEFINITION FOR RECIEVED DOOR PASSWORD                 *
 *******************************************************************************/
/*
 * ReceiveDoorPassword function takes the user given password and compare it in EEPROM
 * to see if its matched or not
 */
void ReceiveDoorPassword(uint8 *password) {
	uint8 i;
	/* Send MC2_READY byte to MC1 to ask it to send the string */
	UART_sendByte(OPEN_DOOR_PASSWORD);
	// Receive password1 via UART
	for (i = 0; i < PASSWORD_SIZE; i++) {
		password[i] = UART_recieveByte();
	}

	_delay_ms(KEY_PRESSED_DELAY); /* Add a short delay after receiving passwords */
}

/*******************************************************************************
 *     FUNCTION DEFINITION FOR COMPARING PASSWORDS [ received , matched ]      *
 *******************************************************************************/
/*
 * CompareSavedPasswords function checks if two passwords match.
 * Returns 1 if they match, 0 if they don't match.
 */
uint8 CompareSavedPassword(uint8 *password1, uint8 *matchedPassword) {
	uint8 i;
	for (i = 0; i < PASSWORD_SIZE; i++) {
		if (password1[i] != matchedPassword[i]) {
			_delay_ms(KEY_PRESSED_DELAY);
			return INCORRECT_PASSWORD; /* Passwords do not match */
		}
	}
	_delay_ms(KEY_PRESSED_DELAY);
	return CORRECT_PASSWORD; /* Passwords match */
}

/*******************************************************************************
 *     FUNCTION DEFINITION FOR COMPARING PASSWORDS [ received , matched ]      *
 *******************************************************************************/
/*
 * Motor operation which clock wise 15 seconds and 3 seconds idle and 15 seconds anti clock wise
 */
void mainOpertaionsOptions(void) {
        /*CLk_WISE*/
		DcMotor_Rotate(CW, 100);
		timer1_delay(CLK_WISE_SECONDS);

		/*ANTI_CLk_WISE*/
		DcMotor_Rotate(STOP, 0);
		timer1_delay(IDLE_SECONDS);

		/*ANTI_CLk_WISE*/
		DcMotor_Rotate(A_CW,100);
		timer1_delay(ANTI_CLK_WISE_SECONDS);

		/*STOP*/
		DcMotor_Rotate(STOP, 0);
}

/*******************************************************************************
 *           FUNCTION DEFINITION FOR IMPELMENTING BUZZER OPTION                *
 *******************************************************************************/
/*
 * Turn on buzzer for 60 sec in case invalid operations of entering password for 3 times consecutive
 */
void IncorrectOptions(void) {
    	/*BUZZER_ON*/
		Buzzer_on();

		/*60 Secs*/
		timer1_delay(BUZZER_SECONDS);

		/*BUZZER_OFF*/
		Buzzer_off();
}

