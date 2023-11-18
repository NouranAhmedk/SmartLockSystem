/******************************************************************************
 * Project: Smart Lock System
 * File Name: main.c
 * Description: Human Machine Interface Ecu Application layer
 * Author: Nouran Ahmed
 *******************************************************************************/
#include"../HAL/LCD/lcd.h"
#include"../HAL/KEYPAD/keypad.h"
#include"../MCAL/TIMER1/timer1.h"
#include"../LIBRARY/std_types.h"
#include"../LIBRARY/atmega_32.h"
#include <util/delay.h> /* For the delay functions */
#include "../MCAL/UART/uart.h"

/*******************************************************************************
 *                                DEFINITIONS                                  *
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
 *                          FUNCTIONS PROTOTYPES                               *
 *******************************************************************************/
void EnterPassword(uint8 *password);
void ReEnterSamePassword(uint8 *password);
void SendPassword(uint8 *password1);
void SendTwoPasswords(uint8 *password1, uint8 *password2);
void MainSystemOptions(uint8 *password);
void CorrectReceivedState(void);
void timer1_tick(void);
void timer1_delay(uint8 seconds);
/*******************************************************************************
 *                                MAIN                                         *
 *******************************************************************************/
volatile uint8 timer1_Ticks = 0;
volatile uint8 trial_flag = 3;
int main(void) {
	SREG_REG.bits.I_BIT = 1;/*Enable I-bit by bit register field*/

	uint8 password1[PASSWORD_SIZE];
	uint8 password2[PASSWORD_SIZE];
	uint8 matchedFlag;

	/* UART description:
	 * The configuration of UART:
	 * 8-bit data
	 * parity check is disabled
	 * stop bit is one bit
	 * baud rate is 9600
	 */
	UART_ConfigType UART_config = { EIGHT_bit, Disabled, ONE_bit, BR3 };
	UART_init(&UART_config);
	LCD_init();

	while (1) {
		/*Step 1 part 1: Take password and confirmed password from user*/
		EnterPassword(password1);
		_delay_ms(MEDIUM_DELAY);
		ReEnterSamePassword(password2);
		_delay_ms(MEDIUM_DELAY);

		/*Step 1 part 2: Send password and confirmed password to M2 via UART*/
		SendTwoPasswords(password1, password2);
		_delay_ms(MEDIUM_DELAY);

		/*Step 1 part 3: Receive a signal from M2 to indicate the state of the two entered passwords*/
		UART_sendByte(UART_CHECKING_PASS);
		matchedFlag = UART_recieveByte();

		/*Step 1 part 4: In case received signal is correct will go to step 2 if not go to step 1 */
		switch (matchedFlag) {
		case CORRECT_PASSWORD:
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 0, "Pass Matched!");
			_delay_ms(MEDIUM_DELAY);

			/*Step 2 part 1: go to main options of the system + open door , - change password*/
			MainSystemOptions(password1);

			break;
		case INCORRECT_PASSWORD:
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 0, "Pass Not Matched!");
			_delay_ms(MEDIUM_DELAY);

			/*Step 1 part 5: go to the start of the system*/
			break;
		}

	}
	return 0;
}

/*******************************************************************************
 *                    FUNCTION DEFINITION FOR TIMER                            *
 *******************************************************************************/
/*
 * Timer 1 handling the delay , in order to avoid any delay that might happened due to context switch
 */
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
 *            FUNCTION DEFINITION FOR GETTING PASSWORD FROM USER               *
 *******************************************************************************/

/*
 * EnterPassword function takes a password of 5 digits from the user and
 * saves it in an array.
 */
void EnterPassword(uint8 *password) {
	uint8 i, key;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Plz enter pass:");
	LCD_moveCursor(1, 0); /* Move the cursor to the second row */
	i = 0; /* Reset 'i' when starting a new password entry */

	while (i < PASSWORD_SIZE) {
		key = KEYPAD_getPressedKey();
		if ((key >= 0) && (key <= 9)) {
			password[i] = key;
			LCD_displayCharacter('*'); /* Display an asterisk for each character entered */
			i++; /* Only increment when a valid key is pressed*/
		}
		_delay_ms(KEY_PRESSED_DELAY); /* Delay between key presses*/
	}
	while (KEYPAD_getPressedKey() != '=');
	_delay_ms(KEY_PRESSED_DELAY);
}

/*
 * ReEnterSamePassword function re-enters the same password to confirm it.
 * It checks if the re-entered password matches the first password.
 */
void ReEnterSamePassword(uint8 *password) {
	uint8 i, key;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Plz re-enter the");
	LCD_moveCursor(1, 0); /* Move the cursor to the second row */
	LCD_displayStringRowColumn(1, 0, "same pass:");
	LCD_moveCursor(1, 10);
	i = 0; /* Reset 'i' when starting a new password entry */

	while (i < PASSWORD_SIZE) {
		key = KEYPAD_getPressedKey();
		if ((key >= 0) && (key <= 9)) {
			password[i] = key;
			LCD_displayCharacter('*'); /* Display an asterisk for each character entered */
			i++; /* Only increment when a valid key is pressed*/
		}
		_delay_ms(KEY_PRESSED_DELAY); /* Delay between key presses*/
	}
	while (KEYPAD_getPressedKey() != '=');
	_delay_ms(KEY_PRESSED_DELAY);
}

/*******************************************************************************
 *       FUNCTION DEFINITION FOR SENDING PASSWORDS FROM USER VIA UART          *
 *******************************************************************************/

/*
 * SendTwoPasswords function sends two passwords via UART.
 */
void SendTwoPasswords(uint8 *password1, uint8 *password2) {
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Send...");
	LCD_moveCursor(1, 0); /* Move the cursor to the second row */

	/* Wait until MC2 is ready to receive the string */
	while (UART_recieveByte() != UART_START) {
	}

	/* Send password1 via UART */
	for (uint8 i = 0; i < PASSWORD_SIZE; i++) {
		UART_sendByte(password1[i]);
		_delay_ms(SMALL_DELAY);
	}
	_delay_ms(KEY_PRESSED_DELAY); /* Add a short delay between passwords */

	/* Send password2 via UART */
	for (uint8 i = 0; i < PASSWORD_SIZE; i++) {
		UART_sendByte(password2[i]);
		_delay_ms(SMALL_DELAY);
	}

}

/*
 * Send saved passwords function sends two passwords via UART to check.
 */
void SendPassword(uint8 *password1) {
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Send...");
	LCD_moveCursor(1, 0); /* Move the cursor to the second row */
	/* Wait until MC2 is ready to receive the string */
	while (UART_recieveByte() != OPEN_DOOR_PASSWORD) {}

	/* Send password1 via UART */
	for (uint8 i = 0; i < PASSWORD_SIZE; i++) {
		UART_sendByte(password1[i]);
		_delay_ms(SMALL_DELAY);
	}
}
/*******************************************************************************
 *                FUNCTION DEFINITION FOR SHOW RECEIVED STATE                  *
 *******************************************************************************/

/*
 * The sending message on screen for updating state of motor
 */
void CorrectReceivedState(void) {

	/* The door is unlocking*/
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 4, "Door is");
	LCD_displayStringRowColumn(1, 2, "Unlocking...");
	timer1_delay(CLK_WISE_SECONDS);

	/* idle state*/
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 3, "WAITING...");
	timer1_delay(IDLE_SECONDS);

	/* The door is locking*/
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 4, "Door is");
	LCD_displayStringRowColumn(1, 3, "Locking...");
	timer1_delay(ANTI_CLK_WISE_SECONDS);

}

/*******************************************************************************
 *           FUNCTION DEFINITION FOR MAIN OPERTAION OF THE SYSTEM              *
 *******************************************************************************/

/*
 * MainSystemOptions for handling system options
 */
void MainSystemOptions(uint8 *password) {
	uint8 key, stateReceivedBytePlus,stateReceivedByteMinus ;
	while (1) {
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 1, "+ : Open Door");
		LCD_displayStringRowColumn(1, 1, "- : Change pass");

		/* Step 3 part 1: + or - for main system options*/
		key = KEYPAD_getPressedKey();

		/* Wait until MC2 is ready to receive the string */
		while (UART_recieveByte() != CHOOSEN_OPTION ) {}
		while( trial_flag > 0 ) { /* This while loop will insure the system will leave its state in case the user entered wrong password it
		                             will ask the user to enter password for certain of trails which is THREE TIMES as trail_flag is defined
		                             global at the beginning of the file */
			switch (key) {
			/* Step 3 part 2 : Receive state of the password in case ( + ) */
			case '+':
				/* Receive state of the password */
				UART_sendByte(DOOR_PASSWORD_STATE);

				/* Step 3 part 3: Enter the saved password you have entered in the first step and send via UART*/
				EnterPassword(password);
				SendPassword(password);

				/* Receive state of the password */
				UART_sendByte(DOOR_STATE);
				stateReceivedBytePlus = UART_recieveByte();

				/* According to received state what will be shown */
				switch (stateReceivedBytePlus) {
				/* Step 3 part 4: In case correct user The door system will be performed */
				case CORRECT_PASSWORD:
					/* Step 3 part 5: This function will appear Unlocking, waiting, Locking */
					CorrectReceivedState();
					/* This will insure the trail_flag retain the default trials of the system */
					trial_flag = 3;
					break;
				/* Step 5 part 1: This case will handle incorrect input from the user as
				 * First trial ( First entering wrong password in the system ) the flag will be decrement by -1 as the default value
				 * of the flag is 3 therefore the remaining trails is 2 and so on ...
				 *  */
				case INCORRECT_PASSWORD:
					LCD_clearScreen();
					LCD_displayStringRowColumn(0, 1, "Wrong Password");
					_delay_ms(KEY_PRESSED_DELAY);
					trial_flag--;
					/* After three trials failed*/
					if(trial_flag == 0){
						LCD_clearScreen();
						/* An error msg will be displayed at the same time the buzzer will be on for 60 seconds*/
						LCD_displayStringRowColumn(0, 5, "ERROR");
						timer1_delay(BUZZER_SECONDS);
					}
					break;
				}
				break;
				/* Step 4 part 1 : Receive state of the password in case ( - ) */
			case '-':
				/* Receive state of the password */
				UART_sendByte(CHANGE_PASSWORD_STATE);

				/* Step 4 part 2: Enter the saved password you have entered in the first step and send via UART*/
				EnterPassword(password);
				SendPassword(password);

				/* Receive state of the password */
				UART_sendByte(DOOR_STATE);
				stateReceivedByteMinus= UART_recieveByte();

				/* According to received state what will be shown */
				switch (stateReceivedByteMinus) {
				/* Step 4 part 3: In case user enter the right password the user will be directed to first step by breaking the two while loops */
				case CORRECT_PASSWORD:
					/* This will insure the trail_flag retain the default trials of the system */
					trial_flag = 3;
					break;
					/* Step 5 part 1: This case will handle incorrect input from the user as
				     * First trial ( First entering wrong password in the system ) the flag will be decrement by -1 as the default value
					 * of the flag is 3 therefore the remaining trails is 2 and so on ...
					 **/
				case INCORRECT_PASSWORD:
					LCD_clearScreen();
					LCD_displayStringRowColumn(0, 1, "Wrong Password");
					_delay_ms(KEY_PRESSED_DELAY);
					trial_flag--;
					if(trial_flag == 0){
						/* After three trials failed*/
						LCD_clearScreen();
						/* An error msg will be displayed at the same time the buzzer will be on for 60 seconds*/
						LCD_displayStringRowColumn(0, 5, "ERROR");
						timer1_delay(BUZZER_SECONDS);
					}
					break;
				}
				break;
			}
			if (stateReceivedBytePlus == CORRECT_PASSWORD || stateReceivedByteMinus == CORRECT_PASSWORD || trial_flag == 0 ) {
				trial_flag = 3;
				break; /* Break out of the while (1) loop this will be directed to step 3 which is + : open door and - : change pass */
			}
		}
		if (stateReceivedByteMinus == CORRECT_PASSWORD) {
			break;  /* Break out of the while (1) loop this will be to step 1 which is enter password: and re-enter password */
		}
		_delay_ms(MEDIUM_DELAY);
	}
}
