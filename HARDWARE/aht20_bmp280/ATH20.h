#ifndef __ATH20_H
#define __ATH20_H

#include "sys.h"
#include "bsp_i2c.h"

#define ATH20_SLAVE_ADDRESS    0x38

//****************************************
// AHT20
//****************************************
#define	INIT		    0xBE
#define	SoftReset		0xBA
#define	StartTest		0xAC

uint8_t ATH20_Init(void);
uint8_t ATH20_Read_Cal_Enable(void);
void ATH20_Read_CTdata(uint32_t *ct);

#endif