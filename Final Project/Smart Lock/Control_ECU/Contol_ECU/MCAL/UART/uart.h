/******************************************************************************
 * Module: uart
 * File Name: uart.h
 * Description: header file for uart
 * Author: Nouran Ahmed
 *******************************************************************************/
#ifndef MCAL_UART_UART_H_
#define MCAL_UART_UART_H_

#include "../../LIBRARY/std_types.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef enum {
	FIVE_bit, SIX_bit, SEVEN_bit, EIGHT_bit              /*, NINE_bit = 7 not needed */
} UART_BitData;

typedef enum {
	Disabled, Reserved, Even_Parity, Odd_Parity
} UART_Parity;

typedef enum {
	ONE_bit, TWO_bit
} UART_StopBit;

typedef enum {
	BR1 = 2400,
	BR2 = 4800,
	BR3 = 9600,
	BR4 = 14400,
	BR5 = 19200,
	BR6 = 28800,
	BR7 = 38400,
	BR8 = 57600,
	BR9 = 76800,
	BR10 = 115200,
	BR11 = 230400,
	BR12 = 250000
} UART_BaudRate;

typedef enum
{
	UART_INT_OFF,UART_INT_ON
}UART_receive_Interrupt;

typedef struct {
	UART_BitData bit_data;
	UART_Parity parity;
	UART_StopBit stop_bit;
	UART_BaudRate baud_rate;
	/*UART_receive_Interrupt reciver_interrupt;*/
} UART_ConfigType;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType * Config_Ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* MCAL_UART2_UART_H_ */

