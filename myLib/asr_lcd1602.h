
#ifndef _asr_lcd1602_H
#define _asr_lcd1602_H

#include <string.h>
#include <stdio.h>
#include "asr.h"
#include <stdlib.h>

#define LCD_BUSY        0x80    //用于检测LCD状态字中的LCD_BUSY标识


// // IO引脚定义
// #ifndef RS_pin
// #define RS_pin 		    9  //PB1
// #endif

// #ifndef	RS(x)
// #define RS(x) 		  digitalWrite(RS_pin,x)
// #endif

// #ifndef RW_pin
// #define RW_pin 		    10 //PB2
// #endif

// #ifndef	RW(x)
// #define RW(x) 		  digitalWrite(RW_pin,x)
// #endif


// #ifndef	E_pin
// #define E_pin		   11 //PB3
// #endif

// #ifndef	E(x)
// #define E(x) 		  digitalWrite(E_pin,x)
// #endif


// #ifndef	LCD1602_Data_gpio    
// #define LCD1602_Data_gpio	    HAL_PA_BASE  
// #endif



// #define Data_write(x) 	 (((gpio_register_t*)HAL_PA_BASE)->gpio_data[0xff] = (uint16_t)(x))

// #define Data_read 	     (uint8_t)(((gpio_register_t*)HAL_PA_BASE)->gpio_data[0xff])

// #if (LCD1602_Data_gpio == HAL_PA_BASE)
// #define LCD1602_Data_out() {scu_set_device_gate(LCD1602_Data_gpio,ENABLE);set_pin_to_gpio_mode(PA0);set_pin_to_gpio_mode(PA1);gpio_set_output_mode(LCD1602_Data_gpio,0xff);}
// #else
// #define LCD1602_Data_out() {scu_set_device_gate(LCD1602_Data_gpio,ENABLE);gpio_set_output_mode(LCD1602_Data_gpio,0xff);setPinFun(13,FIRST_FUNCTION);setPinFun(14,FIRST_FUNCTION);}
// #endif


// #if (LCD1602_Data_gpio == HAL_PA_BASE)
// #define LCD1602_Data_in() {scu_set_device_gate(LCD1602_Data_gpio,ENABLE);set_pin_to_gpio_mode(PA0);set_pin_to_gpio_mode(PA1);gpio_set_input_mode(LCD1602_Data_gpio,0xff);}
// #else
// #define LCD1602_Data_in() {scu_set_device_gate(LCD1602_Data_gpio,ENABLE);gpio_set_input_mode(LCD1602_Data_gpio,0xff);setPinFun(13,FIRST_FUNCTION);setPinFun(14,FIRST_FUNCTION);}
// #endif


class asr_lcd1602
{
public:
	asr_lcd1602(uint8_t RS,uint8_t RW,uint8_t E,gpio_base_t data);
	void init();			//1602初始化							
    void show_char(uint8_t x, uint8_t y, char c);	//1602显示一个字符
    void show_string(uint8_t x, uint8_t y, const char *str);	//1602显示字符串
    void show_num(uint8_t x,uint8_t y,int num);	//1602显示数字
    void show_float(uint8_t x, uint8_t y, float num, uint8_t precision);	//显示小数
    void clear();//1602清屏
	void Data_write(uint8_t x);	
	void LCD1602_Data_in();
	void LCD1602_Data_out();
    uint8_t Data_read();
	uint8_t read_status();
private:
    void RS(uint8_t x);
	void RW(uint8_t x);
	void E(uint8_t x);  
	uint8_t RS_pin,RW_pin,E_pin;
	gpio_base_t LCD1602_Data_gpio;
	void write_data(uint8_t dat);/* data */
	void write_command(uint8_t com);
	uint8_t read_data();
};

asr_lcd1602::asr_lcd1602(uint8_t RS,uint8_t RW,uint8_t E,gpio_base_t data)
{
	RS_pin = RS;
	RW_pin = RW;
	E_pin = E;
    LCD1602_Data_gpio  = data;
}

//========================================================================
// 描述: LCD1602引脚定义
// 参数: none.
// 返回: none.
//========================================================================
void asr_lcd1602::RS(uint8_t x)
{
    digitalWrite(RS_pin,x);
}

void asr_lcd1602::RW(uint8_t x)
{
    digitalWrite(RW_pin,x);
}

void asr_lcd1602::E(uint8_t x)
{
    digitalWrite(E_pin,x);
}

void asr_lcd1602::Data_write(uint8_t x)
{
	(((gpio_register_t*)LCD1602_Data_gpio)->gpio_data[0xff] = (uint16_t)(x));
}
uint8_t asr_lcd1602::Data_read()
{
	(uint8_t)(((gpio_register_t*)LCD1602_Data_gpio)->gpio_data[0xff]);
}
void asr_lcd1602::LCD1602_Data_in()
{
	#if (LCD1602_Data_gpio == HAL_PA_BASE||HAL_PB_BASE)
	scu_set_device_gate((uint32_t)LCD1602_Data_gpio,ENABLE);set_pin_to_gpio_mode(PA0);set_pin_to_gpio_mode(PA1);gpio_set_input_mode(LCD1602_Data_gpio,pin_all);
	#else
	scu_set_device_gate((uint32_t)LCD1602_Data_gpio,ENABLE);gpio_set_input_mode(LCD1602_Data_gpio,0xff);setPinFun(13,FIRST_FUNCTION);setPinFun(14,FIRST_FUNCTION);
	#endif

}
void asr_lcd1602::LCD1602_Data_out()
{
	#if (LCD1602_Data_gpio == HAL_PA_BASE||HAL_PB_BASE)
	scu_set_device_gate((uint32_t)LCD1602_Data_gpio,ENABLE);set_pin_to_gpio_mode(PA0);set_pin_to_gpio_mode(PA1);gpio_set_output_mode(LCD1602_Data_gpio,pin_all);
	#else
	scu_set_device_gate((uint32_t)LCD1602_Data_gpio,ENABLE);gpio_set_output_mode(LCD1602_Data_gpio,0xff);setPinFun(13,FIRST_FUNCTION);setPinFun(14,FIRST_FUNCTION);
	#endif
}
//========================================================================
// 描述: LCD1602读状态
// 参数: none.
// 返回: none.
//========================================================================
uint8_t asr_lcd1602::read_status()
{
	LCD1602_Data_out();
	
 	Data_write(0xff);
 	RS(0);
 	RW(1);
 	E(0);
 	E(0);
 	E(1);
	LCD1602_Data_in();
 	while (Data_read() & LCD_BUSY); //检测忙信号
 	return(Data_read());
}


/*********************************************************/
// 1602液晶写命令函数，cmd就是要写入的命令
/*********************************************************/
// void LcdWriteCmd(uchar cmd)
// { 
// 	LcdRs_P = 0;  //选择写指令
// 	LcdRw_P = 0; 	//选择写
// 	LcdEn_P = 0;  //E使能拉低
// 	P0=cmd;  //把指令送入P0
// 	DelayMs(2); //延时一小会儿，让1602准备接收数据
// 	LcdEn_P = 1;  //使能线电平变化上升沿，命令送入1602的8位数据口
// 	DelayMs(2);//延时，让数据发送
// 	LcdEn_P = 0;	//使能线拉低
// }




//========================================================================
// 描述: LCD1602写数据
// 参数: dat:写的数据.
// 返回: none.
//========================================================================
void asr_lcd1602::write_data(uint8_t dat)
{
	E(0);	 
	RS(1);	
	RW(0);	
	LCD1602_Data_out();
	Data_write(dat);	
	delay(1);

	E(1);
	delay(5);
	E(0);

	Data_write(dat<<4); 
	delay(1);

	E(1);
	delay(5);
	E(0);
}

//========================================================================
// 描述: LCD1602写指令
// 参数: com:写的指令.
// 返回: none.
//========================================================================
void asr_lcd1602::write_command(uint8_t com)
{
	E(0);
	RS(0);	
	RW(0);
	LCD1602_Data_out();
	Data_write(com);	
	delay(1);

	E(1);	
	delay(5);
	E(0);

	Data_write(com<<4); 
	delay(1);

	E(1);	 
	delay(5);
	E(0);
}

//========================================================================
// 描述: LCD1602读数据
// 参数: WCLCD:写的指令; LCD_BUSYC:0,忽略忙检测，1，需要忙检测.
// 返回: none.
//========================================================================
uint8_t asr_lcd1602::read_data()
{
	LCD1602_Data_in();
 	RS(1); 
 	RW(1);
 	E(0);
 	E(0);
 	E(1);
 	return(Data_read());
}

//========================================================================
// 描述: 初始化LCD1602
// 参数: none.
// 返回: none.
//========================================================================
void asr_lcd1602::init()
{
	LCD1602_Data_out();
 	Data_write(0X0);
    //引脚配置

	pinMode(RS_pin,OUTPUT);
	setPinFun(RS_pin,FIRST_FUNCTION);
	pinMode(RW_pin,OUTPUT);
	setPinFun(RW_pin,FIRST_FUNCTION);
    pinMode(E_pin,OUTPUT);
    setPinFun(E_pin,FIRST_FUNCTION);

 	write_command(0x38); 	//三次模式设置，不检测忙信号
 	delay(5);
 	write_command(0x38);
 	delay(5); 
 	write_command(0x38);
 	delay(5); 

 	write_command(0x32);
 	write_command(0x28);
 	write_command(0x0C);
 	write_command(0x06);
 	write_command(0x01);
	write_command(0x80);
}

//========================================================================
// 描述: 在指定位置显示数字
// 参数: x,y:坐标; Num:显示的数字; len:长度.
// 返回: none.
//========================================================================
void asr_lcd1602::clear()
{
	write_command(0x01);
	delay(2); 
} 

//========================================================================
// 描述: LCD1602按指定位置显示一个字符
// 参数: x,y:坐标; c:显示的字符.
// 返回: none.
//========================================================================
void asr_lcd1602::show_char(uint8_t x, uint8_t y, char c)
{
 	y &= 0x1;
 	x &= 0xF; 				//限制X不能大于15，Y不能大于1
 	if (y) x |= 0x40; 		//当要显示第二行时地址码+0x40;
 	x |= 0x80; 			    //算出指令码
 	write_command(x);  //这里不检测忙信号，发送地址码
 	write_data(c);
}

//========================================================================
// 描述: LCD1602按指定位置显示一个字符串
// 参数: x,y:坐标; str:显示的字符串.
// 返回: none.
//========================================================================
void asr_lcd1602::show_string(uint8_t x, uint8_t y, const char *str)
{
 	uint8_t len,i;

 	len = strlen(str);
 	y &= 0x1;
 	x &= 0xF; 				//限制X不能大于15，Y不能大于1
	
	for(i=0;i<len;i++)
	{
		show_char(x,y,str[i]);
		x++;
	}
}

//========================================================================
// 描述: 在指定位置显示数字
// 参数: x,y:坐标; num:显示的数字; 
// 返回: none.
//========================================================================
void asr_lcd1602::show_num(uint8_t x,uint8_t y,int num)
{
	char men[10];
    sprintf(men,"%d",num);
	show_string(x,y,men);
} 

//========================================================================
// 描述: 显示小数
// 参数: x,y:起点坐标;num:要显示的数值;precision:精度.
// 返回: none.
//========================================================================
void asr_lcd1602::show_float(uint8_t x, uint8_t y, float num, uint8_t precision)
{
  uint8_t i;
  uint8_t float_bit;
  int16_t int_part;
  int_part = (int16_t)num/1;

    if(num>=0)			//整数部分
    {
		show_num( x, y, int_part);
        for(i=1;i<10;i++)
        {
            int_part=int_part/10;
            if(int_part == 0)
            {
                break;
            }
        }
		num = num - int_part;
		show_char((uint8_t)(x+i), y, '.');	//显示小数点
		switch(precision)
		{
			case 1:									//保留1位精度
				float_bit = (int32_t)(num*10)%10;
				show_num((uint8_t)(x+i+1), y,(int16_t)float_bit);	
			break;
			case 2:									//保留2位精度
				float_bit = (int32_t)(num*10)%10;
				show_num((uint8_t)(x+i+1), y,(int16_t)float_bit);
				float_bit = (int32_t)(num*100)%10;
				show_num((uint8_t)(x+i+2), y,(int16_t)float_bit);
			break;
			case 3:									   //保留3位精度
				float_bit = (int32_t)(num*10)%10;
				show_num((uint8_t)(x+i+1), y,(int16_t)float_bit);
				float_bit = (int32_t)(num*100)%10;
				show_num((uint8_t)(x+i+2), y,(int16_t)float_bit);
				float_bit = (int32_t)(num*1000)%10;
				show_num((uint8_t)(x+i+3), y,(int16_t)float_bit);
			break;
			case 4:										//保留4位精度
				float_bit = (int32_t)(num*10)%10;			//显示小数部分第一位
				show_num((uint8_t)(x+i+1), y,(int16_t)float_bit);
				float_bit = (int32_t)(num*100)%10;		//显示小数部分第二位
				show_num((uint8_t)(x+i+2), y,(int16_t)float_bit);
				float_bit = (int32_t)(num*1000)%10;		//显示小数部分第三位
				show_num((uint8_t)(x+i+3), y,(int16_t)float_bit);
				float_bit = (int32_t)(num*10000)%10;		//显示小数部分第四位
				show_num((uint8_t)(x+i+4), y,(int16_t)float_bit);
			break;
			default:
				float_bit = (int32_t)(num*10)%10;
				show_num((uint8_t)(x+i+1), y,(int16_t)float_bit);	
			break;//保留1位精度
		}
    }
    else
    {
		i = 0;
		if(int_part == 0)
		{
			show_char((uint8_t)(x), y, '-');
			i++;
		}
		show_num( x+i, y, int_part);
        for(;i<10;i++)
        {
            int_part=int_part/10;
            if(int_part == 0)
            {
                break;
            }
        }
		num = -(num - int_part);
		show_char((uint8_t)(x+i+1), y, '.');	//显示小数点
		switch(precision)
		{
			case 1:									//保留1位精度
				float_bit = (int32_t)(num*10)%10;
				show_num((uint8_t)(x+i+2), y,(int16_t)float_bit);	
			break;
			case 2:									//保留2位精度
				float_bit = (int32_t)(num*10)%10;
				show_num((uint8_t)(x+i+2), y,(int16_t)float_bit);
				float_bit = (int32_t)(num*100)%10;
				show_num((uint8_t)(x+i+3), y,(int16_t)float_bit);
			break;
			case 3:									   //保留3位精度
				float_bit = (int32_t)(num*10)%10;
				show_num((uint8_t)(x+i+2), y,(int16_t)float_bit);
				float_bit = (int32_t)(num*100)%10;
				show_num((uint8_t)(x+i+3), y,(int16_t)float_bit);
				float_bit = (int32_t)(num*1000)%10;
				show_num((uint8_t)(x+i+4), y,(int16_t)float_bit);
			break;
			case 4:										//保留4位精度
				float_bit = (int32_t)(num*10)%10;			//显示小数部分第一位
				show_num((uint8_t)(x+i+2), y,(int16_t)float_bit);
				float_bit = (int32_t)(num*100)%10;		//显示小数部分第二位
				show_num((uint8_t)(x+i+3), y,(int16_t)float_bit);
				float_bit = (int32_t)(num*1000)%10;		//显示小数部分第三位
				show_num((uint8_t)(x+i+4), y,(int16_t)float_bit);
				float_bit = (int32_t)(num*10000)%10;		//显示小数部分第四位
				show_num((uint8_t)(x+i+5), y,(int16_t)float_bit);
			break;
			default:
				float_bit = (int32_t)(num*10)%10;
				show_num((uint8_t)(x+i+2), y,(int16_t)float_bit);	
			break;//保留1位精度
		}
    }
}  
#endif      //LCD1602.h