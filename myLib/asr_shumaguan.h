#ifndef __SHUMAGUAN_H
#define __SHUMAGUAN_H

#include "asr.h"


#ifdef smg_yang//段码
                             //0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   A,   B,   C,   D,   E,   F,   -,  NONE, H,   L,   J,   b,   c,   d,   n,   h,   一,  二,  三,
const uint8_t  _smg_seg1[29]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E,0xBF,0xFF,0x89,0xC7,0xE1,0x83,0xA7,0xA1,0xAB,0x8B,0xF7,0xB7,0xB6};
//共阳显示字库
#endif
#ifdef smg_ying             //0,   1,   2,   3,   4,   5,   6,   7,  8,   9,   A,   B,   C,   D,   E,   F,   -,  NONE, H,   L,   J,   b,   c,   d,   n,   h,   一,  二,  三,
const uint8_t _smg_seg1[29]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x7,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x40,0x00,0x76,0x38,0x1E,0x7C,0x58,0x5E,0x54,0x74,0x08,0x48,0x49};
//共阴显示字库
#endif

volatile uint8_t _smg_display_buff[8]={17,17,17,17,17,17,17,17};

uint8_t xianshi_dian=0; //独立小数点控制标志
uint8_t _smg_point_bit=0; //小数点位置
volatile uint8_t index_m = 0;



#if (SMG_PORT == HAL_PA_BASE)
#define SMG_PORT_MODE {scu_set_device_gate(SMG_PORT,ENABLE);set_pin_to_gpio_mode(PA0);set_pin_to_gpio_mode(PA1);gpio_set_output_mode(SMG_PORT,0xff);}
#else
#define SMG_PORT_MODE {scu_set_device_gate(SMG_PORT,ENABLE);gpio_set_output_mode(SMG_PORT,0xff);setPinFun(13,FIRST_FUNCTION);setPinFun(14,FIRST_FUNCTION);}
#endif

#ifdef SMG_WEI1
#define SMG_WEI1_OUT {pinMode(SMG_WEI1,output);set_pin_to_gpio_mode(SMG_WEI1);}
#define SMG_WEI1_HL(X)   digitalWrite(SMG_WEI1,(X))
#endif

#ifdef SMG_WEI2
#define SMG_WEI2_OUT {pinMode(SMG_WEI2,output);set_pin_to_gpio_mode(SMG_WEI2);} 
#define SMG_WEI2_HL(X)   digitalWrite(SMG_WEI2,(X))
#endif

#ifdef SMG_WEI3
#define SMG_WEI3_OUT {pinMode(SMG_WEI3,output);set_pin_to_gpio_mode(SMG_WEI3);}
#define SMG_WEI3_HL(X)   digitalWrite(SMG_WEI3,(X))
#endif

#ifdef SMG_WEI4
#define SMG_WEI4_OUT {pinMode(SMG_WEI4,output);set_pin_to_gpio_mode(SMG_WEI4);} 
#define SMG_WEI4_HL(X)   digitalWrite(SMG_WEI4,(X))
#endif

#ifdef SMG_WEI5
#define SMG_WEI5_OUT {pinMode(SMG_WEI5,output);set_pin_to_gpio_mode(SMG_WEI5);}
#define SMG_WEI5_HL(X)   digitalWrite(SMG_WEI5,(X))
#endif

#ifdef SMG_WEI6
#define SMG_WEI6_OUT {pinMode(SMG_WEI6,output);set_pin_to_gpio_mode(SMG_WEI6);}
#define SMG_WEI6_HL(X)   digitalWrite(SMG_WEI6,(X))
#endif

#ifdef SMG_WEI7
#define SMG_WEI7_OUT {pinMode(SMG_WEI7,output);set_pin_to_gpio_mode(SMG_WEI7);}
#define SMG_WEI7_HL(X)   digitalWrite(SMG_WEI7,(X))
#endif

#ifdef SMG_WEI8
#define SMG_WEI8_OUT {pinMode(SMG_WEI8,output);set_pin_to_gpio_mode(SMG_WEI8);}
#define SMG_WEI8_HL(X)   digitalWrite(SMG_WEI8,(X))
#endif


void smg_init(void)
{
    uint8_t j1;
	#ifdef SMG_WEI1
	SMG_WEI1_OUT//位设置为推挽输出
	#endif
	#ifdef SMG_WEI2
	SMG_WEI2_OUT//位设置为推挽输出
	#endif
	#ifdef SMG_WEI3
	SMG_WEI3_OUT//位设置为推挽输出
	#endif
	#ifdef SMG_WEI4
	SMG_WEI4_OUT//位设置为推挽输出	
	#endif
	#ifdef SMG_WEI5
	SMG_WEI5_OUT//位设置为推挽输出
	#endif
	#ifdef SMG_WEI6  
	SMG_WEI6_OUT//位设置为推挽输出
	#endif
	#ifdef SMG_WEI7
	SMG_WEI7_OUT//位设置为推挽输出
	#endif
	#ifdef SMG_WEI8
	SMG_WEI8_OUT//位设置为推挽输出	
	#endif 
    SMG_PORT_MODE//数码管段推挽输出  


#ifdef smg_yang
		j1=0;
#endif
#ifdef smg_ying
		j1=1;
#endif

    #ifdef SMG_WEI1
        SMG_WEI1_HL(j1);
    #endif
    #ifdef SMG_WEI2
        SMG_WEI2_HL(j1);
    #endif
    #ifdef SMG_WEI3
        SMG_WEI3_HL(j1);
    #endif
    #ifdef SMG_WEI4
        SMG_WEI4_HL(j1);
    #endif
    #ifdef SMG_WEI5
        SMG_WEI5_HL(j1);
    #endif
    #ifdef SMG_WEI6
        SMG_WEI6_HL(j1);
    #endif
    #ifdef SMG_WEI7
        SMG_WEI7_HL(j1);
    #endif
    #ifdef SMG_WEI8
        SMG_WEI8_HL(j1);
    #endif

#ifdef smg_yang
    gpio_set_output_high_level(SMG_PORT,pin_all);
#endif
#ifdef smg_ying
	gpio_set_output_low_level(SMG_PORT,pin_all);
#endif

}



void smg_display(uint8_t data,uint8_t point)
{
    gpio_register_t* gpio = (gpio_register_t*)SMG_PORT;
#ifdef smg_ying

		if(point)
		{
            gpio->gpio_data[0xff] = (uint8_t)(_smg_seg1[data]| 0x80) ;
		}
		else
		{
             gpio->gpio_data[0xff] = (uint8_t)(_smg_seg1[data]) ;
		}  
#endif
#ifdef smg_yang
		if(point)
		{
            gpio->gpio_data[0xff] = (uint16_t)(_smg_seg1[data]&0x7f) ;
		}
		else
		{
            gpio->gpio_data[0xff] = (uint16_t)(_smg_seg1[data]) ;
		} 
#endif
}

void smg_display_update_buf(uint8_t nbit,uint8_t data)
{
    _smg_display_buff[nbit] = data;
}




void smg_scan_callback(void)
{
    uint8_t i1;
    uint8_t j1;

#ifdef smg_yang
		i1=1;
		j1=0;
#endif
#ifdef smg_ying
		i1=0;
		j1=1;
#endif
#ifdef smg_yang
    gpio_set_output_high_level(SMG_PORT,pin_all);
#endif
#ifdef smg_ying
	gpio_set_output_low_level(SMG_PORT,pin_all);
#endif


#if (SAOMIAO_WEISHU == 0)
    switch (index_m) 
    {
        case 0:SMG_WEI1_HL(i1);SMG_WEI4_HL(j1);smg_display(_smg_display_buff[index_m],0x01&((_smg_point_bit | xianshi_dian)>>index_m));index_m=1;
        break;
        case 1:SMG_WEI1_HL(j1);SMG_WEI2_HL(i1);smg_display(_smg_display_buff[index_m],0x01&((_smg_point_bit | xianshi_dian)>>index_m));index_m=2;
        break;
        case 2:SMG_WEI2_HL(j1);SMG_WEI3_HL(i1);smg_display(_smg_display_buff[index_m],0x01&((_smg_point_bit | xianshi_dian)>>index_m));index_m=3;
        break;
        case 3:SMG_WEI3_HL(j1);SMG_WEI4_HL(i1);smg_display(_smg_display_buff[index_m],0x01&((_smg_point_bit | xianshi_dian)>>index_m));index_m=0;
        break;
        default:
        break;
    }

#elif (SAOMIAO_WEISHU == 1)
    switch (index_m) 
    {
        case 4:SMG_WEI5_HL(i1);SMG_WEI8_HL(j1);smg_display(_smg_display_buff[index_m],0x01&((_smg_point_bit | xianshi_dian)>>index_m));index_m=5;
        break;
        case 5:SMG_WEI5_HL(j1);SMG_WEI6_HL(i1);smg_display(_smg_display_buff[index_m],0x01&((_smg_point_bit | xianshi_dian)>>index_m));index_m=6;
        break;
        case 6:SMG_WEI6_HL(j1);SMG_WEI7_HL(i1);smg_display(_smg_display_buff[index_m],0x01&((_smg_point_bit | xianshi_dian)>>index_m));index_m=7;
        break;
        case 7:SMG_WEI7_HL(j1);SMG_WEI8_HL(i1);smg_display(_smg_display_buff[index_m],0x01&((_smg_point_bit | xianshi_dian)>>index_m));index_m=4;
        break;
        default:index_m = 4;
        break;
    }
#else
    switch (index_m) 
    {
        case 0:SMG_WEI1_HL(i1);SMG_WEI8_HL(j1);smg_display(_smg_display_buff[index_m],0x01&((_smg_point_bit | xianshi_dian)>>index_m));index_m=1;
        break;
        case 1:SMG_WEI1_HL(j1);SMG_WEI2_HL(i1);smg_display(_smg_display_buff[index_m],0x01&((_smg_point_bit | xianshi_dian)>>index_m));index_m=2;
        break;
        case 2:SMG_WEI2_HL(j1);SMG_WEI3_HL(i1);smg_display(_smg_display_buff[index_m],0x01&((_smg_point_bit | xianshi_dian)>>index_m));index_m=3;
        break;
        case 3:SMG_WEI3_HL(j1);SMG_WEI4_HL(i1);smg_display(_smg_display_buff[index_m],0x01&((_smg_point_bit | xianshi_dian)>>index_m));index_m=4;
        break;
        case 4:SMG_WEI4_HL(j1);SMG_WEI5_HL(i1);smg_display(_smg_display_buff[index_m],0x01&((_smg_point_bit | xianshi_dian)>>index_m));index_m=5;
        break;
        case 5:SMG_WEI5_HL(j1);SMG_WEI6_HL(i1);smg_display(_smg_display_buff[index_m],0x01&((_smg_point_bit | xianshi_dian)>>index_m));index_m=6;
        break;
        case 6:SMG_WEI6_HL(j1);SMG_WEI7_HL(i1);smg_display(_smg_display_buff[index_m],0x01&((_smg_point_bit | xianshi_dian)>>index_m));index_m=7;
        break;
        case 7:SMG_WEI7_HL(j1);SMG_WEI8_HL(i1);smg_display(_smg_display_buff[index_m],0x01&((_smg_point_bit | xianshi_dian)>>index_m));index_m=0;
        break;
        default:
        break;
    }
#endif
}



void smg_display_num(uint8_t nbit,int32_t num)
{
    int32_t temp;
    uint8_t i;
    if(num>=0)
    {
        for(i=nbit;i<8;i++)
        {
            temp=num%10;
            num=num/10;
            _smg_display_buff[i]= (uint8_t)temp;
            if(num == 0)
            {
                break;
            }
        }
    }
    else//负数
    {
        num = -num;
        for(i=nbit;i<8;i++)
        {
            temp=num%10;
            num=num/10;
            _smg_display_buff[i] = (uint8_t)temp;
            if(num == 0)
            {
				if(i<8)
				{
					_smg_display_buff[i+1]=16;//添加负号
				}
                break;
            }
        }
    }
}

void smg_display_float(uint8_t nbit,float num)
{
    uint8_t i ;
    signed long temp;
    signed long int_part,float_part;
    int_part = (signed long)num/1;

    float_part = (signed long)(num*10)%10;//break;//保留1位精度

    if(float_part<0)
    {
        float_part = -float_part;
    }
    for(i=nbit;i<8;i++)
    {
        temp=float_part%10;
        float_part=float_part/10;
        _smg_display_buff[i]=temp;
        if(float_part == 0)
        {
            break;
        }
    }
    _smg_point_bit = (0x01 <<(i+1));//显示小数点


    if(int_part>=0)
    {
        for(i=i+1;i<8;i++)
        {
            temp=int_part%10;
            int_part=int_part/10;
            _smg_display_buff[i]=temp;
            if(int_part == 0)
            {
                break;
            }
        }
    }
    else
    {
        int_part = -int_part;
        for(i=i+1;i<8;i++)
        {
            temp=int_part%10;
            int_part=int_part/10;
            _smg_display_buff[i]=temp;
            if(int_part == 0)
            {
				if(i<8)
				{
                    _smg_display_buff[i+1]=16;//添加负号
				}
                break;
            }
        }
    } 
}

void smg_display_clear_bit(uint8_t nbit)
{
	uint8_t i;

    if(nbit == 0xff)
    {
        for(i=0;i<8;i++)
        {
            _smg_display_buff[i] = 17;
        }
        xianshi_dian = 0;
        _smg_point_bit=0;			
    }
    else 
    {
        _smg_display_buff[nbit]=17;
        _smg_point_bit &= ~(0x01<<nbit);
    }
}   

void smg_display_dian(unsigned char nbit,unsigned char point)
{
	if(point)
	{
		xianshi_dian |= (0x01<<nbit);
	}
	else
	{
		xianshi_dian &= ~(0x01<<nbit);
	}
}


#endif
