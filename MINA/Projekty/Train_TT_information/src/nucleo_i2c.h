/*
 * nucleo_i2c.h
 *
 *  Created on: 11. 1. 2017
 *      Author: weiss_000
 */

#ifndef COMMON_NUCLEO_I2C_H_
#define COMMON_NUCLEO_I2C_H_

#include "nucleo_core.h"    // kvuli definici registru

bool InitI2C1(void);        //TODO konfigurovat ktere I2C a na kterzch pinech
bool InitI2C(I2C_TypeDef *portPtr);  //TODO konfigurovat kterych pinech

/*
 * device: address of chip, upper 7 bits valid !!
 * adr:    register address
 * value:  value to write to register
 */
void I2C_Write8(uint8_t device, uint8_t adr, uint8_t value);

/*
 * device: address of chip, upper 7 bits valid !!
 * adr:    register address
 * return: value from register
 */
uint8_t I2C_Read8(uint8_t device, uint8_t adr);

/*
 * device: address of chip, upper 7 bits valid !!
 * adr:    register address
 * return: value from register
 */
uint16_t I2C_Read16(uint8_t device, uint8_t adr);

/*
 * device: address of chip, upper 7 bits valid !!
 * adr:    register start address
 * bp:     pointer to data buffer
 * len:    requested count
 * return: count of readed bytes (eq. len)
 */
int I2C_ReadBlock(uint8_t device, uint8_t adr, uint8_t *bp, int len);

#endif /* COMMON_NUCLEO_I2C_H_ */
