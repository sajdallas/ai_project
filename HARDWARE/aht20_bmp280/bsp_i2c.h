#ifndef __BSP_I2C_H
#define	__BSP_I2C_H

#include "sys.h"

/*********************软件IIC使用的宏****************************/
#define Soft_I2C_SDA 		GPIO_PIN_7
#define Soft_I2C_SCL 		GPIO_PIN_6
#define Soft_I2C_PORT       GPIOB
//
#define Soft_I2C_SCL_0 		HAL_GPIO_WritePin(Soft_I2C_PORT, Soft_I2C_SCL, GPIO_PIN_RESET)
#define Soft_I2C_SCL_1 		HAL_GPIO_WritePin(Soft_I2C_PORT, Soft_I2C_SCL, GPIO_PIN_SET)
#define Soft_I2C_SDA_0 		HAL_GPIO_WritePin(Soft_I2C_PORT, Soft_I2C_SDA, GPIO_PIN_RESET)
#define Soft_I2C_SDA_1   	HAL_GPIO_WritePin(Soft_I2C_PORT, Soft_I2C_SDA, GPIO_PIN_SET)

/*�ȴ���ʱʱ��*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))


void I2C_Bus_Init(void);

void Set_I2C_Retry(unsigned short ml_sec);
unsigned short Get_I2C_Retry(void);
int Sensors_I2C_ReadRegister(unsigned char Address, unsigned char RegisterAddr, 
                                          unsigned short RegisterLen, unsigned char *RegisterValue);
int Sensors_I2C_WriteRegister(unsigned char Address, unsigned char RegisterAddr, 
                                           unsigned short RegisterLen, const unsigned char *RegisterValue);

/* AHT20 专用：直接读取，不发送寄存器地址（AHT20 不是寄存器型设备） */
int Sensors_I2C_ReadDirect(unsigned char slave_addr, unsigned short len, unsigned char *data_ptr);

#endif