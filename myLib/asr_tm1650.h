/********************************************************************************
* @File name: asr_tm1650.h
* @Author: hhdd
* @Version: 1.1
* @Date: 2022-3-21
* @Description: TM1650驱动的4位数码管库。
********************************************************************************/
#ifndef __TM1650_H_
#define __TM1650_H_

#include "asr_softiic.h"

//                           0    1     2   3    4    5     6    7    8    9    A    B    C    D    E    F    -
const uint8_t tm1650_tab[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x40};
//const uint8_t tm1650_tab[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E,0xBF,0xFF};

class TM1650{
public:
	TM1650();
	TM1650(uint8_t sda,uint8_t scl);
	void begin();
	void clear();	//清除
	void display_num(int16_t num);	//显示数字
	void display_float(float f); //显示小数
	void display_time(uint8_t hour, uint8_t min, uint8_t point); //显示时间
	void display_bit(uint8_t num, uint8_t _bit,uint8_t point); //在bit位显示数num;point为1，小数点亮,否则为灭
	void clear_bit(uint8_t _bit);
private:
	uint8_t _sda;
	uint8_t _scl;
	uint8_t _connected;
	void write_data(uint8_t addr, uint8_t dat);
};

TM1650::TM1650()
{
	_connected = 1;
}

TM1650::TM1650(uint8_t sda,uint8_t scl)
{
    _sda = sda;
	_scl = scl;
	_connected = 0;
}

/************************************************************************
  * @brief  初始化.
  * @param  none.
  * @return none
  **********************************************************************/
void TM1650::begin()
{
	if(_connected == 0){
		softiic.begin(_sda,_scl);
	}else{
	#if defined(TW_ASR_PRO)
        dpmu_set_io_reuse(PA2,THIRD_FUNCTION);
        dpmu_set_io_reuse(PA3,THIRD_FUNCTION);//配置IIC引脚
        iic_polling_init(IIC0,100,0,LONG_TIME_OUT);
	#else
		i2c_io_init();
        i2c_master_init(IIC0,100,0X5FFFFF);
	#endif
	}
    
    write_data(0x48,0x31);     // 开显示、8段显示方式、3级亮度
}

/************************************************************************
  * @brief  TM1650清除.
  * @param  none.
  * @return none
  **********************************************************************/
void TM1650::clear()
{
    uint8_t i;
	for(i=0; i<4;i++)
	{
		write_data(0x68+i*2,0x00);  
	}
}

/************************************************************************
  * @brief  TM1650显示整数.
  * @param  num  要显示的数字(-999~9999).
  * @return none
  **********************************************************************/
void TM1650::display_num(int16_t num)
{
	int16_t temp=0;
	uint8_t i=0;
	if(num >= 10000){clear();return;}
	if(num <= -1000){clear();return;}

	if(num >= 0){
		for(i=4;i>=1;i--){
			temp = num % 10;
			num = num / 10;
			display_bit(temp,4-i,0);
			if(num == 0)
			{
				break;
			}
		}
	}else{
		num = -num;
		for(i=4;i>=1;i--){
			temp = num % 10;
			num = num / 10;
			display_bit(temp,4-i,0);
			if(num == 0){
				i--;
				display_bit(16,4-i,0);	//添加负号
				break;
			}
		}
	}
}

/************************************************************************
  * @brief  TM1650显示小数.
  * @param  f  要显示的数字(-999~9999).
  * @return none
  **********************************************************************/
void TM1650::display_float(float f)
{
	uint8_t a[4];
	uint8_t b[4];
	uint8_t int_bit=0,float_bit=0;
	uint16_t int_part=0,float_part=0;
	uint8_t i=0;

	if(f >= 10000){clear();return;}
	if(f <= -1000){clear();return;}

	clear();
	if(f == 0)
	{
		display_num(0);
	}
	else if(f > 0)
	{
		int_part = (int16_t)f/1;
		//获取整数位位数
		a[0] = int_part%10;if(a[0] != 0)int_bit = 1;//最低位
		a[1] = int_part%100/10;if(a[1] != 0)int_bit = 2;
		a[2] = int_part%1000/100;if(a[2] != 0)int_bit = 3;
		a[3] = int_part/1000;if(a[3] != 0)int_bit = 4;

		//获取小数位位数
		float_part = (uint32_t)(f*10000)%10000;
		b[0] = float_part/1000;if(b[0] != 0)float_bit = 1;
		b[1] = float_part%1000/100;if(b[1] != 0)float_bit = 2;
		b[2] = float_part%100/10;if(b[2] != 0)float_bit = 3;
		b[3] = float_part%10;if(b[3] != 0)float_bit = 4;
		
		if(int_bit == 4)	//4位全为整数位
		{
			display_num(int_part);
		}else if(int_bit == 3)	//整数位为3位
		{
			if(b[0] == 0)	//整数有3位,小数0位有效
			{
				display_num(int_part);
			}else
			{
				display_bit(a[2],3,0);
				display_bit(a[1],2,0);
				display_bit(a[0],1,1);
				display_bit(b[0],0,0);
			}
		}else if(int_bit == 2)//整数有2位
		{
			if(b[1] == 0)
			{
				if(b[0] == 0)	//整数有2位,小数为0
				{
					display_num(int_part);
				}else			//整数有2位,小数后1位有效
				{
					display_bit(a[1],2,0);
					display_bit(a[0],1,1);
					display_bit(b[0],0,0);
				}
			}else	//小数点后二位不为零
			{
				display_bit(a[1],3,0);
				display_bit(a[0],2,1);
				display_bit(b[0],1,0);
				display_bit(b[1],0,0);
			}
		}else if(int_bit == 1)//整数只有1位
		{
			if(b[2] == 0)
			{
				if(b[1] == 0)
				{
					if(b[0] == 0)	//小数部分全为0
					{
						display_num(int_part);
					}else          //小数后一位不为0
					{
						display_bit(a[0],1,0);
						display_bit(b[0],0,0);
					}
				}else
				{
					display_bit(a[0],2,1);
					display_bit(b[0],1,0);
					display_bit(b[1],0,0);
				}
			}else		//小数部分全不为0
			{
				display_bit(a[0],3,1);
				display_bit(b[0],2,0);
				display_bit(b[1],1,0);
				display_bit(b[2],0,0);
			}
		}else if(int_bit == 0)
		{
			if(float_bit > 3)float_bit=3;
			for(i=0; i<float_bit; i++){
				display_bit(b[(float_bit-1)-i],i,0);
			}
			display_bit(0,float_bit,1);
		}
	}else{
		f = -f;
		int_part = (int16_t)f/1;
		//获取整数位位数
		a[0] = int_part%10;if(a[0] != 0)int_bit = 1;//最低位
		a[1] = int_part%100/10;if(a[1] != 0)int_bit = 2;
		a[2] = int_part%1000/100;if(a[2] != 0)int_bit = 3;

		//获取小数位位数
		float_part = (uint32_t)(f*10000)%10000;
		b[0] = float_part/1000;if(b[0] != 0)float_bit = 1;
		b[1] = float_part%1000/100;if(b[1] != 0)float_bit = 2;
		b[2] = float_part%100/10;if(b[2] != 0)float_bit = 3;
		b[3] = float_part%10;if(b[3] != 0)float_bit = 4;
		
		if(int_bit == 3)	//整数位为3位
		{
			display_num(-int_part);
		}else if(int_bit == 2)	//整数位为2位
		{
			if(b[0] == 0)	//小数位为0
			{
				display_num(-int_part);
			}else{
				display_bit(16,3,0);	//显示"-"号
				display_bit(a[1],2,0);
				display_bit(a[0],1,1);
				display_bit(b[0],0,0);
			}
		}else if(int_bit == 1)  //整数有1位
		{
			if(b[1] == 0)
			{
				if(b[0] == 0)	//小数位全为0
				{
					display_num(-int_part);
				}else           //
				{
					display_bit(16,2,0);	//显示"-"号
					display_bit(a[0],1,1);
					display_bit(b[0],0,0);
				}
			}else		//小数后二位不为零
			{
				display_bit(16,3,0);	//显示"-"号
				display_bit(a[0],2,1);
				display_bit(b[0],1,0);
				display_bit(b[1],0,0);
			}
		}else if(int_bit == 0)		//整数位为0
		{
			if(float_bit > 2)float_bit=2;
			for(i=0; i<float_bit; i++){
				display_bit(b[(float_bit-1)-i],i,0);
			}
			display_bit(0,float_bit,1);
			display_bit(16,float_bit+1,0);
		}
	}
}

//========================================================================
// 描述: TM1650显示时间
// 参数: hour,min:时间;:1,小数点亮，0,小数点灭.
// 返回: none.
//========================================================================
void TM1650::display_time(uint8_t hour, uint8_t min, uint8_t point)
{
	hour = hour > 99 ? 99 : hour;
    min = min > 99 ? 99 : min;

	write_data(0x68, (uint8_t)tm1650_tab[hour/10]&(~0X80));
    if (point == 0)
    {
		write_data(0x6A, (uint8_t)tm1650_tab[hour%10]&(~0X80));
    }
    else
    {
    	write_data(0x6A, (uint8_t)tm1650_tab[hour%10] | 0x80);
    }
    write_data(0x6C, (uint8_t)tm1650_tab[min/10]&(~0X80));
	write_data(0x6E, (uint8_t)tm1650_tab[min%10]&(~0X80));
}

//========================================================================
// 描述: TM1650在指定位显示
// 参数: num:显示的数字;bit:指定位;point:1,小数点亮，0,小数点灭.
// 返回: none.
//========================================================================
void TM1650::display_bit(uint8_t num, uint8_t _bit,uint8_t point)
{
	if(num > 16)return;
	if(point==0){
        write_data(0x68+6-2*(_bit), (uint8_t)tm1650_tab[num]&(~0X80));
	}else{
        write_data(0x68+6-2*(_bit), (uint8_t)tm1650_tab[num]|0x80);
	}
}

//========================================================================
// 描述: TM1650清除指定位
// 参数: nbit:指定位.
// 返回: none.
//========================================================================
void TM1650::clear_bit(uint8_t _bit)
{
    write_data(0x68+6-2*(_bit),0);
}

//========================================================================
// 描述: TM1650写函数
// 参数: add:地址;dat:数据.
// 返回: none.
//========================================================================
void TM1650::write_data(uint8_t addr, uint8_t dat)
{
	if(_connected == 0){
		softiic.start(addr);
		softiic.write(dat);
		softiic.stop(); 
	}else{
	    char buf[1] = {0};
        buf[0] = dat;
        i2c_master_only_send(addr,buf,1);	
	}
}
#endif