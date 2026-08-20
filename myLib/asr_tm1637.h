/********************************************************************************
* @File name: asr_tm1637.h
* @Author: hhdd
* @Version: 1.0
* @Date: 2022-3-21
* @Description: TM1637驱动的4位数码管库。
********************************************************************************/
#ifndef __TM1637_H_
#define __TM1637_H_

#include "asr.h"
#ifndef TW_ASR_PRO
#include "asr_delay.h"
#endif

//                      0    1     2   3    4    5     6   7    8    9    A    B    C    D    E    F
uint8_t tm1637_tab[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};

class TM1637{
public:
	TM1637(uint8_t sda,uint8_t scl);
	void begin(uint8_t brightness);
	void clear();	//清除
	void display_num(int32_t num);	//显示数字
	void display_float(float f); //显示小数
	void display_time(uint8_t hour, uint8_t min, uint8_t point); //显示时间
	void display_bit(uint8_t num, uint8_t _bit,uint8_t point); //在bit位显示数num;point为1，小数点亮,否则为灭
	void clear_bit(uint8_t _bit);
    void SetBrightness(uint8_t level );
	uint8_t read_key(); //读按键
private:
	uint8_t _sda;
	uint8_t _scl;
    void start();
    void stop();
    bool wait_ack();
    void send_byte(uint8_t dat);
	void write_com(uint8_t cmd);
	void write(uint8_t addr, uint8_t dat);
	void write(uint8_t reg, uint8_t* value, uint8_t len);
};

TM1637::TM1637(uint8_t sda,uint8_t scl)
{
    _sda = sda;
	_scl = scl;
}

//========================================================================
// 描述: TM1637读按键
// 参数: reg:寄存器地址;value:数据指针; len:数据长度.
// 返回: none.
//========================================================================
uint8_t TM1637::read_key()
{
    uint8_t rekey,i;
    start();
    send_byte(0x42);
    wait_ack();
    digitalWrite(_sda, 1);
    pinMode(_sda, input);
    for(i=0; i<8;i++) {
        digitalWrite(_scl, 0);
        rekey = rekey>>1;
        delayMicroseconds(30);
        digitalWrite(_scl, 1);
        delayMicroseconds(2);
        if(digitalRead(_sda) == 1){
            rekey= rekey | 0x80;
        }else{
            rekey= rekey | 0x00;
        }
        delayMicroseconds(30);
    }
    wait_ack();
    stop();
    pinMode(_sda, output);
    return rekey;
}

//========================================================================
// 描述: TM1637初始化
// 参数: none.
// 返回: none.
//========================================================================
void TM1637::begin(uint8_t brightness)
{
    pinMode(_sda, OUTPUT);
    pinMode(_scl, OUTPUT);
#if defined(TW_ASR_PRO)
    set_pin_to_gpio_mode(_sda);
    set_pin_to_gpio_mode(_scl);
#else
    if((_sda<=8) && (_sda >=5))
    {
        setPinFun(_sda,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(_sda,FIRST_FUNCTION);
    }
    if((_scl<=8) && (_scl >=5))
    {
        setPinFun(_scl,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(_scl,FIRST_FUNCTION);
    }
#endif
    digitalWrite(_sda,1);
    digitalWrite(_scl,1);
    if(brightness < 8){
		write_com(brightness | 0x88); 
	}else{
		write_com(0x8F);   //1000 1111b
	}
}

//========================================================================
// 描述: TM1637清除
// 参数: none.
// 返回: none.
//========================================================================
void TM1637::clear()
{
    uint8_t i;
	for(i=0; i < 6;i++)
	{
		write(i,0);  
	}
}

/**
 * @brief 调节亮度
 * @param  level            0~7
 */
void TM1637::SetBrightness(uint8_t level )
{
    if(level < 8){
		write_com(level | 0x88); 
	}else{
		write_com(0x8F);   //1000 1111b
	}
}
//========================================================================
// 描述: TM1637显示数字
// 参数: none.
// 返回: none.
//========================================================================
void TM1637::display_num(int32_t num)
{
    int32_t temp;
    uint8_t i;
    if(num>=0)
    {
        for(i=3;i>=0;i--)
        {
            temp = num%10;
            num = num/10;
            write(i,tm1637_tab[temp]);
            if(num == 0)
            {
                break;
            }
        }
    }
    else//负数
    {
        num = -num;
        for(i=3;i>=0;i--)
        {
            temp=num%10;
            num=num/10;
            write(i,tm1637_tab[temp]);
            if(num == 0)
            {
                i--;
                write(i,tm1637_tab[16]);//添加负号
                break;
            }
        }
    }
}


//========================================================================
// 描述: TM1637在指定位显示
// 参数: num:显示的数字;bit:指定位;point:1,小数点亮，0,小数点灭.
// 返回: none.
//========================================================================
void TM1637::display_bit(uint8_t num, uint8_t _bit,uint8_t point)
{
	uint8_t temp;
	if(num > 15)return;
	if(_bit>3)return;
	temp=tm1637_tab[num];

	if(point==0){
		temp&= ~(0X80);
		write((uint8_t)(3-_bit),temp);	
	}else{
		temp|=0x80;
		write((uint8_t)(3-_bit),temp);
	}
}


//========================================================================
// 描述: TM1637显示小数
// 参数: none.
// 返回: none.
//========================================================================
void TM1637::display_float(float f)
{
	uint8_t a[4];
	uint8_t b[4];
	uint8_t int_bit,float_bit;
	uint16_t int_part,float_part;
	uint8_t i;

	clear();

	if(f == 0)
	{
		write(3,tm1637_tab[0]);
	}
	else
	{
		if(f > 10000){clear();return;}
		if((f*10000) < 1){clear();return;}
		int_part = (uint16_t)f/1;
		if(int_part < 1)int_bit = 0;
		else
		{
			a[3] = int_part%10;if(a[3] != 0)int_bit = 1;//最低位
			a[2] = int_part%100/10;if(a[2] != 0)int_bit = 2;
			a[1] = int_part%1000/100;if(a[1] != 0)int_bit = 3;
			a[0] = int_part/1000;if(a[0] != 0)int_bit = 4;
		}

		float_part = (uint32_t)(f*10000)%10000;
		b[0] = float_part/1000;if(b[0] != 0)float_bit = 1;
		b[1] = float_part%1000/100;if(b[1] != 0)float_bit = 2;
		b[2] = float_part%100/10;if(b[2] != 0)float_bit = 3;
		b[3] = float_part%10;if(b[3] != 0)float_bit = 4;
		
		if(int_bit == 4)	//4位全为整数位
		{
			display_num(int_part);
			//write(3,tm1637_tab[a[3]]|0X80);
		}
		if(int_bit == 3)	//整数位为3位
		{
			if(b[0] == 0)
			{
				display_num(int_part);
				//write(3,tm1637_tab[a[3]]|0X80);
			}
			else
			{
				write(0,tm1637_tab[a[1]]);
				write(1,tm1637_tab[a[2]]);
				write(2,tm1637_tab[a[3]]|0X80);
				write(3,tm1637_tab[b[0]]);
			}
		}
		if(int_bit == 2)//整数有2位
		{
			if(b[1] == 0)
			{
				if(b[0] == 0)
				{
					display_num(int_part);
					//Write_DATA(3,tm1637_tab[a[3]]|0X80);
				}
				else
				{
					write(1,tm1637_tab[int_part/10]);
					write(2,tm1637_tab[int_part%10]|0X80);
					write(3,tm1637_tab[b[0]]);
				}
			}
			else//小数点后第二位不为零
			{
				write(0,tm1637_tab[a[2]]);
				write(1,tm1637_tab[a[3]]|0X80);
				write(2,tm1637_tab[b[0]]);
				write(3,tm1637_tab[b[1]]);
			}
		}
		if(int_bit == 1)//整数只有1位
		{
			if(b[2] == 0)
			{
				if(b[1] == 0)
				{
					if(b[0] == 0)
					{
						write(3,tm1637_tab[int_part]);
//						Write_DATA(3,tm1637_tab[int_part]|0X80);
					}
					else
					{
						write(2,tm1637_tab[int_part]|0X80);
						write(3,tm1637_tab[b[0]]);
					}
				}
				else
				{
					write(1,tm1637_tab[int_part]|0X80);
					write(2,tm1637_tab[b[0]]);
					write(3,tm1637_tab[b[1]]);
				}
			}
			else//小数点第三位不为零
			{
				write(0,tm1637_tab[a[3]]|0X80);
				write(1,tm1637_tab[b[0]]);
				write(2,tm1637_tab[b[1]]);
				write(3,tm1637_tab[b[2]]);
			}
		}
		if(int_bit == 0)
		{
			if(float_bit > 3)float_bit=3;
			for(i=0; i<float_bit; i++)
				write(3-i, tm1637_tab[b[(float_bit-1)-i]]);
			write(3-float_bit, tm1637_tab[0]|0x80);
		}
	}
}

//========================================================================
// 描述: TM1637显示时间
// 参数: hour,min:时间;:1,小数点亮，0,小数点灭.
// 返回: none.
//========================================================================
void TM1637::display_time(uint8_t hour, uint8_t min, uint8_t point)
{
	hour = hour > 99 ? 99 : hour;
    min = min > 99 ? 99 : min;

	write(0, tm1637_tab[hour/10]&= ~(0X80));
    if (point == 0)
    {
		write(1, tm1637_tab[hour%10] &= ~(0X80));
    }
    else
    {
    	write(1, tm1637_tab[hour%10] |= 0x80);
    }
    write(2, tm1637_tab[min/10]&= ~(0X80));
	write(3, tm1637_tab[min%10]&= ~(0X80));
}

//========================================================================
// 描述: TM1637清除指定位
// 参数: nbit:指定位.
// 返回: none.
//========================================================================
void TM1637::clear_bit(uint8_t _bit)
{
    write(3-(_bit-1),0);
}

//========================================================================
// 描述: 发送start信号.
// 参数: none.
// 返回: none.
//========================================================================
void TM1637::start()
{
    digitalWrite(_sda,1);
    digitalWrite(_scl,1);
    delayMicroseconds(4);
    digitalWrite(_sda,0);
    delayMicroseconds(4);
    digitalWrite(_scl,0);
}

//========================================================================
// 描述: 发送stop信号.
// 参数: none.
// 返回: none.
//========================================================================
void TM1637::stop()
{
    digitalWrite(_scl,0);
    delayMicroseconds(4);
    digitalWrite(_sda,0);
    delayMicroseconds(4);
    digitalWrite(_scl,1); 
    delayMicroseconds(4);
    digitalWrite(_sda,1);    
    delayMicroseconds(4); 
}

//========================================================================
// 描述: 等待ack信号.
// 参数: none.
// 返回: false，接收到应答信号; true，没有接收到应答信号.
//========================================================================
bool TM1637::wait_ack()
{
    uint8_t ucErrTime;
    digitalWrite(_sda,1);
    delayMicroseconds(1); 
    digitalWrite(_scl,1);
    delayMicroseconds(1); 
    pinMode(_sda,INPUT);
    while(digitalRead(_sda))
    {
        ucErrTime++;
        if(ucErrTime > 250){
            stop();
			pinMode(_sda,OUTPUT);
            return false;
        }
    }
    digitalWrite(_scl,0);
    pinMode(_sda,OUTPUT);
    return true;
}

//========================================================================
// 描述: 写一个字节数据.
// 参数: dat:写入的数据.
// 返回: none.
//========================================================================
void TM1637::send_byte(uint8_t dat)
{
    digitalWrite(_scl,0);
    for(uint8_t i=0; i<8; i++)
    {
        digitalWrite(_sda,dat&0x01);
        dat>>= 1;
        delayMicroseconds(2);
        digitalWrite(_scl,1);
        delayMicroseconds(2);
        digitalWrite(_scl,0);
        delayMicroseconds(2);
    }
}

//========================================================================
// 描述: TM1637发送命令字
// 参数: add:地址;dat:数据.
// 返回: none.
//========================================================================
void TM1637::write_com(uint8_t cmd)
{
    start();
    send_byte(cmd);
    wait_ack();
    stop();
}

//========================================================================
// 描述: TM1637写函数
// 参数: add:地址;dat:数据.
// 返回: none.
//========================================================================
void TM1637::write(uint8_t addr, uint8_t dat)
{
	start();    //开始信号
    send_byte(0x44);//发送器件写模式
    wait_ack();
	stop();  
	
	
    start();    //开始信号
    send_byte(addr|0xc0);//发送器件写地址
    wait_ack();
    send_byte(dat);                //发送寄存器地址
    wait_ack();
    stop();   //发送停止命令
	
	// start();    //开始信号
    // send_byte(0x8A);//发送亮度
    // wait_ack();
	// stop(); 
}

//========================================================================
// 描述: TM1637写函数
// 参数: reg:寄存器地址;value:数据指针; len:数据长度.
// 返回: none.
//========================================================================
void TM1637::write(uint8_t reg, uint8_t* value, uint8_t len)
{
    write_com(0x40);    //自动地址增加
    start();
    send_byte(reg|0xc0);
    wait_ack();
    for(int i=0;i<len;i++)
    {
        send_byte(value[i]);
        wait_ack();
    }
    stop();
}

#endif