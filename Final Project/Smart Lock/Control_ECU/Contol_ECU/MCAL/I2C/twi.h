/******************************************************************************
 * Module: I2C
 * File Name: i2c.h
 * Description: header file for the i2c driver
 * Author: Nouran Ahmed
 *******************************************************************************/
#ifndef MCAL_I2C_TWI_H_
#define MCAL_I2C_TWI_H_

#include "../../LIBRARY/std_types.h"
/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

/* I2C Status Bits in the TWSR Register */
#define TWI_START         0x08 /* start has been sent */
#define TWI_REP_START     0x10 /* repeated start */
#define TWI_MT_SLA_W_ACK  0x18 /* Master transmit ( slave address + Write request ) to slave + ACK received from slave. */
#define TWI_MT_SLA_R_ACK  0x40 /* Master transmit ( slave address + Read request ) to slave + ACK received from slave. */
#define TWI_MT_DATA_ACK   0x28 /* Master transmit data and ACK has been received from Slave. */
#define TWI_MR_DATA_ACK   0x50 /* Master received data and send ACK to slave. */
#define TWI_MR_DATA_NACK  0x58 /* Master received data but doesn't send ACK to slave. */

typedef enum {
	TWI_CLK_1, TWI_CLK_4, TWI_CLK_16, TWI_CLK_64
}TWI_Prescaler;

/*
 * Description:
 * The address of salves by your choice from bit 1 to bit 7 ,but not  bit zero
 *
 **/
typedef enum {
	SLAVE1 = 0b00000010,
	SLAVE2 = 0b00000100,
	SLAVE3 = 0b00001000,
	SLAVE4 = 0b00010000,
	SLAVE5 = 0b00100000,
	SLAVE6 = 0b01000000
} TWI_Address;

/*
 * Description:
 * SCL frequency = CPU Clock frequency "8MHZ" = 16 + 2(TWBR).4 ^ (TWPS)
 *
 **/
typedef enum {
	Normal_mode_100k = 0x20,            /*100000 = 8000000 = 16 + 2(TWBR).4 ^(0) = 32*/
	Fast_mode_400k = 0x02,              /*400000 = 8000000 = 16 + 2(TWBR).4 ^(0) = 2*/
	/*Fast_mode_plus_1M = 1000000,                 cannot be used in Frequency 8MH
	 High_speed_mode_3_4M = 3400000                 cannot be used in Frequency 8MH*/
} TWI_BaudRate;

typedef struct {
	TWI_Address address;
	TWI_BaudRate bit_rate;
	TWI_Prescaler prescaler;
} TWI_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void TWI_init(const TWI_ConfigType *Config_Ptr);
void TWI_start(void);
void TWI_stop(void);
void TWI_writeByte(uint8 data);
uint8 TWI_readByteWithACK(void);
uint8 TWI_readByteWithNACK(void);
uint8 TWI_getStatus(void);

#endif /* MCAL_I2C_TWI_H_ */
