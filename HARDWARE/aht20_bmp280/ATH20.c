#include "ATH20.h"
#include "bsp_i2c.h"
#include "delay.h"
#include "cmsis_os.h"
#include "stdio.h"

uint8_t ATH20_Read_Status(void)//读取AHT10的状态寄存器
{
    uint8_t Byte_first;
    Sensors_I2C_ReadDirect(ATH20_SLAVE_ADDRESS, 1, &Byte_first);

	return Byte_first;
}

uint8_t ATH20_Read_Cal_Enable(void)
{
    uint8_t val = 0;//ret = 0,

    val = ATH20_Read_Status();
    if((val & 0x68) == 0x08)  //�ж�NORģʽ��У׼����Ƿ���Ч
        return 1;
    else
        return 0;
}

void ATH20_Read_CTdata(uint32_t *ct) //��ȡAHT10���¶Ⱥ�ʪ������
{
    uint32_t RetuData = 0;
	uint16_t cnt = 0;
    uint8_t Data[10];
    uint8_t tmp[10];

    tmp[0] = 0x33;
    tmp[1] = 0x00;
	Sensors_I2C_WriteRegister(ATH20_SLAVE_ADDRESS,StartTest, 2, tmp);  //P0 ���жϲ�ʹ��
	osDelay(75);//�ȴ�75ms

    cnt = 0;
	while(((ATH20_Read_Status()&0x80) == 0x80))//�ȴ�æ״̬����
	{
        osDelay(1);
        if(cnt++ >= 100)
        {
            break;
        }
	}

    Sensors_I2C_ReadDirect(ATH20_SLAVE_ADDRESS, 6, Data);

	RetuData = 0;
    RetuData = (RetuData|Data[1]) << 8;
	RetuData = (RetuData|Data[2]) << 8;
	RetuData = (RetuData|Data[3]);
	RetuData = RetuData >> 4;
	ct[0] = RetuData;

    RetuData = 0;
	RetuData = (RetuData|Data[3]) << 8;
	RetuData = (RetuData|Data[4]) << 8;
	RetuData = (RetuData|Data[5]);
	RetuData = RetuData&0xfffff;
	ct[1] = RetuData;
}

uint8_t count;
uint8_t ATH20_Init(void)
{
    uint8_t tmp[10];

    osDelay(40);

    tmp[0] = 0x08;
    tmp[1] = 0x00;
	Sensors_I2C_WriteRegister(ATH20_SLAVE_ADDRESS,INIT, 2, tmp);  //P0 ���жϲ�ʹ��

    osDelay(500);
    count = 0;

    while(ATH20_Read_Cal_Enable() == 0)//��Ҫ�ȴ�״̬��status��Bit[3]=1ʱ��ȥ�����ݡ����Bit[3]������1 ����������λ0xBA��AHT10�������³�ʼ��AHT10��ֱ��Bit[3]=1
    {
        Sensors_I2C_WriteRegister(ATH20_SLAVE_ADDRESS,SoftReset, 0, tmp);
        osDelay(200);

        Sensors_I2C_WriteRegister(ATH20_SLAVE_ADDRESS,INIT, 2, tmp);

        count++;
        if(count >= 10)
            return 0;
        osDelay(500);
    }
    return 1;
}