#ifndef _TM1638_BOARD_H
#define _TM1638_BOARD_H

#include <ASR_TM1638.h>

//                               0,    1,   2,   3,   4,  5,   6,   7,   8,   9,   A,   B,   C,   D,   E,   F,   -,  NONE
const uint8_t nixietube_tab[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x40,0x00};
// const uint8_t nixietube_tab[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E,0xBF,0xFF};

class TM1638_BOARD : public TM1638{
public:
    TM1638_BOARD(uint8_t sckpin, uint8_t diopin, uint8_t stbpin);
    void begin();
	void set_brightness(uint8_t brightness = 3);
    void clear();
    void display_bit(uint8_t num, uint8_t digits,uint8_t point);
    void display_num(int32_t num);
    void display_float(float num, uint8_t precision);
    void display_time(uint8_t hour,uint8_t minute,uint8_t second);
    uint8_t read_key();
    void write_all_LED(uint8_t LED_flag);   
private:
    uint8_t _sckpin;
    uint8_t _diopin;
    uint8_t _stbpin;
};

//========================================================================
// 描述: 构造函数.
// 参数: sckpin:sck引脚; mosipin: mosi引脚; misopin：miso引脚.
// 返回: none.
//========================================================================
TM1638_BOARD::TM1638_BOARD(uint8_t sckpin, uint8_t diopin, uint8_t stbpin):TM1638(sckpin,diopin,stbpin){
    _sckpin = sckpin;
    _diopin = diopin;
    _stbpin = stbpin;
}

//========================================================================
// 描述: 初始化函数.
// 参数: none.
// 返回: none.
//========================================================================
void TM1638_BOARD::begin()
{
    TM1638::begin();
    Write_COM(0x8b);   //1000 1011b
}

//========================================================================
// 描述: 初始化函数.
// 参数: brightness:亮度(0-7)8级亮度可调.
// 返回: none.
//========================================================================
void TM1638_BOARD::set_brightness(uint8_t brightness)
{
	if(brightness<8){
        Write_COM(brightness+0x88); 
    }
}

//========================================================================
// 描述: 数码管清屏.
// 参数: none.
// 返回: none.
//========================================================================
void TM1638_BOARD::clear()
{
    uint8_t temp[16];
    for(int j=0;j<16;j++)
    {
        temp[j] = 0;
    }  
    Write_DATA(0,temp,16);
}

//========================================================================
// 描述: 在指定位显示数字.
// 参数: num:在指定位要显示数字; digits:在哪一位显示; point:小数点是否显示(1,显示;0,不显示).
// 返回: none.
//========================================================================
void TM1638_BOARD::display_bit(uint8_t num, uint8_t digits,uint8_t point)
{
    if(digits>=8)return;
    Write_DATA(14-2*digits,nixietube_tab[num]|(point<<7));
}

//========================================================================
// 描述: 显示数字.
// 参数: num:要显示的数字.
// 返回: none.
//========================================================================
void TM1638_BOARD::display_num(int32_t num)
{
    int32_t temp;
    uint8_t i;
    if(num>=0)
    {
        for(i=7;i>=0;i--)
        {
            temp = num%10;
            num = num/10;
            display_bit(temp, 7-i, 0);
            if(num == 0)
            {
                break;
            }
        }
    }
    else//负数
    {
        num = -num;
        for(i=7;i>=0;i--)
        {
            temp=num%10;
            num=num/10;
            display_bit(temp, 7-i, 0);
            if(num == 0)
            {
                i--;
                display_bit(16, 7-i, 0);//添加负号
                break;
            }
        }
    }
}

//========================================================================
// 描述: 数码管显示浮点数.
// 参数: 浮点数,精度(1,2,3,4).
// 返回: none.
//========================================================================
void TM1638_BOARD::display_float(float num, uint8_t precision)
{
    uint8_t i;
    uint8_t float_bit;
    int32_t int_part;
    int32_t temp;
    i = 0;
    int_part = (int32_t)num/1;
    if(num>=0)			
    {
        switch(precision)
        {
            case 1: //保留1位精度
                float_bit = (int32_t)(num*10)%10;
                display_bit(float_bit, 0, 0);
                i+=1;
            break;
            case 2: //保留2位精度
                float_bit = (int32_t)(num*10)%10;
                display_bit(float_bit, 1, 0);
                float_bit = (int32_t)(num*100)%10;
                display_bit(float_bit, 0, 0);
                i+=2;
            break;
            case 3: //保留3位精度
                float_bit = (int32_t)(num*10)%10;
                display_bit(float_bit, 2, 0);
                float_bit = (int32_t)(num*100)%10;
                display_bit(float_bit, 1, 0);
                float_bit = (int32_t)(num*1000)%10;
                display_bit(float_bit, 0, 0);
                i+=3;
            break;
            case 4: //保留4位精度
                float_bit = (int32_t)(num*10)%10;
                display_bit(float_bit, 3, 0);
                float_bit = (int32_t)(num*100)%10;
                display_bit(float_bit, 2, 0);
                float_bit = (int32_t)(num*1000)%10;
                display_bit(float_bit, 1, 0);
                float_bit = (int32_t)(num*10000)%10;
                display_bit(float_bit, 0, 0);
                i+=4;
            break;
            default:    //保留1位精度
                float_bit = (int32_t)(num*10)%10;
                display_bit(float_bit, 0, 0);
                i+=1;
            break;
        }
        temp=int_part%10;
        int_part=int_part/10;
        display_bit(temp, i, 1);
        if(int_part == 0)
        {
            return;
        }
        i++;
        for(;i<8;i++)
        {
            temp=int_part%10;
            int_part=int_part/10;
            display_bit(temp, i, 0);
            if(int_part == 0)
            {
                return;
            }
        }
    }else
    {
        num  = -num;
        int_part = -int_part;
        switch(precision)
        {
             case 1: //保留1位精度
                float_bit = (int32_t)(num*10)%10;
                display_bit(float_bit, 0, 0);
                i+=1;
            break;
            case 2: //保留2位精度
                float_bit = (int32_t)(num*10)%10;
                display_bit(float_bit, 1, 0);
                float_bit = (int32_t)(num*100)%10;
                display_bit(float_bit, 0, 0);
                i+=2;
            break;
            case 3: //保留3位精度
                float_bit = (int32_t)(num*10)%10;
                display_bit(float_bit, 2, 0);
                float_bit = (int32_t)(num*100)%10;
                display_bit(float_bit, 1, 0);
                float_bit = (int32_t)(num*1000)%10;
                display_bit(float_bit, 0, 0);
                i+=3;
            break;
            case 4: //保留4位精度
                float_bit = (int32_t)(num*10)%10;
                display_bit(float_bit, 3, 0);
                float_bit = (int32_t)(num*100)%10;
                display_bit(float_bit, 2, 0);
                float_bit = (int32_t)(num*1000)%10;
                display_bit(float_bit, 1, 0);
                float_bit = (int32_t)(num*10000)%10;
                display_bit(float_bit, 0, 0);
                i+=4;
            break;
            default:    //保留1位精度
                float_bit = (int32_t)(num*10)%10;
                display_bit(float_bit, 0, 0);
                i+=1;
            break;
        }
        temp=int_part%10;
        int_part=int_part/10;
        display_bit(temp, i, 1);
        if(int_part == 0)
        {
            if(i<=6){
                display_bit(16, i, 0);//添加负号
            }
            return;
        }
        i++;
        for(;i<8;i++)
        {
            temp=int_part%10;
            int_part=int_part/10;
            display_bit(temp, i, 0);
            if(int_part == 0)
            {
                if(i<=6){
                    display_bit(16, i, 0);//添加负号
                }
                return;
            }
        }
    }     
}

//========================================================================
// 描述: 数码管显示时间.
// 参数: 时，分，秒.
// 返回: none.
//========================================================================
void TM1638_BOARD::display_time(uint8_t hour,uint8_t minute,uint8_t second)
{

    display_bit(hour / 10, 7, 0);
    display_bit(hour % 10, 6, 0);
    display_bit(0x10, 5, 0);
    display_bit(minute / 10, 4, 0);
    display_bit(minute % 10, 3, 0);
    display_bit(0x10, 2, 0);
    display_bit(second / 10, 1, 0);
    display_bit(second % 10, 0, 0);
}

//========================================================================
// 描述: 读取按键.
// 参数: none.
// 返回: 被按下的按键值.
//========================================================================
uint8_t TM1638_BOARD::read_key()
{
    uint8_t key_value,i;
    digitalWrite(_diopin,1);
    key_value = TM1638::Read_key();
    for(i=0;i<8;i++)
		if((0x01<<i)==key_value)
			break;
	 if(i<8)
		return i+1; 
	 else
	    return key_value;
}

//========================================================================
// 描述: 控制全部LED函数.
// 参数: LED_flag:表示各个LED状态.
// 返回: none.
//========================================================================
void TM1638_BOARD::write_all_LED(uint8_t LED_flag)                                    
{
    uint8_t i;
    for(i=0;i<8;i++)
    {
        if(LED_flag&(1<<i))
            Write_DATA(2*i+1,1);
        else
            Write_DATA(2*i+1,0);
    }
}

#endif // _CH32V_TM1638_BOARD_H


