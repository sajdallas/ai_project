#ifndef _ASR_DS1302_H
#define _ASR_DS1302_H


#include "asr.h"
#ifndef TW_ASR_PRO
#include "asr_delay.h"
#endif

#define WRITE_SEC 0X80
#define WRITE_MIN 0X82
#define WRITE_HOUR 0X84
#define WRITE_DAY 0X86
#define WRITE_WEEK 0X8A
#define WRITE_MONTH 0X88
#define WRITE_YEAR 0X8C
#define WRITE_PROTECT 0X8E

#define READ_SEC 0X81
#define READ_MIN 0X83
#define READ_HOUR 0X85
#define READ_DAY 0X87
#define READ_WEEK 0X8B
#define READ_MONTH 0X89
#define READ_YEAR 0X8D

typedef struct 
{
    uint8_t Second;
    uint8_t Minute;
    uint8_t Hour;
    uint8_t Week;
    uint8_t Day;
    uint8_t Month;
    uint8_t Year;
}SYSTEMTIME;    //定义的时间类型

/// @brief 
class DS1302
{
public:

    DS1302(uint8_t sck,uint8_t rst,uint8_t io);
    void init(void);
	
    void write_ds1302_byte(uint8_t dat);//写一个字节数据 
    void write_ds1302_byte(uint8_t addr,uint8_t dat);//向一个地址写一个字节数据 

    uint8_t read_ds1302_byte(); //读一个字节数据
    uint8_t read_ds1302_byte(uint8_t addr); //向一个地址读一个字节数据

    void set_rtc(uint8_t year,uint8_t mon,uint8_t date,uint8_t week);
    void set_rtc(uint8_t hour ,uint8_t min, uint8_t sec);
    void read_rtc(SYSTEMTIME *TIME);
    void DS1302_SetTime(uint8_t addr,uint8_t value);
    void start();//开始运行，秒置为0
    void stop();//停止运行，秒置为0

private:
    uint8_t _sck;
    uint8_t _rst;
    uint8_t _io;
};

DS1302::DS1302(uint8_t sck,uint8_t rst,uint8_t io)
{
    _sck = sck;
    _rst = rst;
    _io = io;
}


void DS1302::init(void)
{
    set_pin_to_gpio_mode(_sck);
	dpmu_set_io_pull((PinPad_Name)pinToFun[_sck],DPMU_IO_PULL_DOWN);
    set_pin_to_gpio_mode(_rst);
	dpmu_set_io_pull((PinPad_Name)pinToFun[_rst],DPMU_IO_PULL_DOWN);
    set_pin_to_gpio_mode(_io);
	dpmu_set_io_pull((PinPad_Name)pinToFun[_io],DPMU_IO_PULL_DOWN);

    pinMode(_sck,output);
    pinMode(_rst,output);
    pinMode(_io,output);
}

void DS1302::write_ds1302_byte(uint8_t dat)
{
    uint8_t i ;
    pinMode(_io,output);
    for(i = 0;i < 8;i++)
    {
        if(dat & 0x01)
        {
            digitalWrite(_io,1);
        }
        else
        {
            digitalWrite(_io,0);
        }
        digitalWrite(_sck,0);
        delayMicroseconds(2);
        digitalWrite(_sck,1);
        delayMicroseconds(2);
        dat >>= 1;    /////////////////////////////////////////////
    }

}

void DS1302::write_ds1302_byte(uint8_t addr,uint8_t dat)
{

    digitalWrite(_rst,0);
    delayMicroseconds(2);
    digitalWrite(_sck,0);
    delayMicroseconds(2);
    digitalWrite(_rst,1);
    delayMicroseconds(2);
    write_ds1302_byte(addr);
    write_ds1302_byte(dat);
    digitalWrite(_sck,1) ;
    delayMicroseconds(2);
    digitalWrite(_rst,0);
    delayMicroseconds(2);
}

uint8_t DS1302::read_ds1302_byte()
{
    uint8_t REDATA = 0;
    uint8_t i = 0;
    pinMode(_io,input);
    for(i = 0;i < 8;i++)
    {
        digitalWrite(_sck,1);
        delayMicroseconds(2);
        digitalWrite(_sck,0);
        delayMicroseconds(2);  
        REDATA >>= 1;      
        if(digitalRead(_io) == 1)
        {
            REDATA |= 0x80;   //最高位置一
        }
        else
        {
            REDATA &= 0x7F;                //最高位清零
        }
        
    }
    return (REDATA);
}

uint8_t DS1302::read_ds1302_byte(uint8_t addr)
{
    uint8_t TEMP = 0;
   // addr |= 0x01;//读操作最低位置为1

    digitalWrite(_rst,0);
    delayMicroseconds(2);
    digitalWrite(_sck,0);
    delayMicroseconds(2);
    digitalWrite(_rst,1);
    write_ds1302_byte(addr);
    TEMP = read_ds1302_byte();
    digitalWrite(_sck,1);
    delayMicroseconds(2);
    digitalWrite(_rst,0);
    delayMicroseconds(2);
    return (TEMP);
}
void DS1302::DS1302_SetTime(uint8_t addr,uint8_t value)
{
    write_ds1302_byte(WRITE_PROTECT,0x00);//关闭写保护
    write_ds1302_byte(addr, ((value/10)<<4 | (value%10))); 
    write_ds1302_byte(WRITE_PROTECT,0x80);//打开写保护
}
void DS1302::set_rtc(uint8_t year,uint8_t mon,uint8_t date,uint8_t week)
{
    DS1302_SetTime(WRITE_YEAR,year);
   // delayMicroseconds(3);
    DS1302_SetTime(WRITE_MONTH,mon);
  //  delayMicroseconds(3);
    DS1302_SetTime(WRITE_DAY,date);
  //  delayMicroseconds(3);   
    DS1302_SetTime(WRITE_WEEK,week);
   // delayMicroseconds(3); 
   
}
void DS1302::set_rtc(uint8_t hour ,uint8_t min, uint8_t sec)
{
    DS1302_SetTime(WRITE_HOUR,hour);
    //delayMicroseconds(3);      
    DS1302_SetTime(WRITE_MIN,min);
   // delayMicroseconds(3); 
    DS1302_SetTime(WRITE_SEC,sec);
   // delayMicroseconds(3); 
}
void DS1302::read_rtc(SYSTEMTIME *TIME)
{
    TIME->Year = (read_ds1302_byte(READ_YEAR) & 0x0f) + ((read_ds1302_byte(READ_YEAR) >> 4) * 10);
    delayMicroseconds(3);
    TIME->Month = (read_ds1302_byte(READ_MONTH) & 0x0f) + ((read_ds1302_byte(READ_MONTH) >> 4) * 10);
    delayMicroseconds(3);
    TIME->Day = (read_ds1302_byte(READ_DAY) & 0x0f) + ((read_ds1302_byte(READ_DAY) >> 4) * 10);
    delayMicroseconds(3);
    TIME->Hour = (read_ds1302_byte(READ_HOUR) & 0x0f) + ((read_ds1302_byte(READ_HOUR) >> 4) * 10);
    delayMicroseconds(3);
    TIME->Minute = (read_ds1302_byte(READ_MIN) & 0x0f) + ((read_ds1302_byte(READ_MIN) >> 4) * 10);
    delayMicroseconds(3);
    TIME->Second = (read_ds1302_byte(READ_SEC) & 0x0f) + ((read_ds1302_byte(READ_SEC) >> 4) * 10);
    delayMicroseconds(3);
    TIME->Week = (read_ds1302_byte(READ_WEEK) & 0x0f) + ((read_ds1302_byte(READ_WEEK) >> 4) * 10);
    delayMicroseconds(3);

}

void DS1302::start()
{
    uint8_t temp = 0;
    write_ds1302_byte(WRITE_PROTECT,0x00);//关闭写保护
    temp = read_ds1302_byte(READ_MONTH);

    write_ds1302_byte(WRITE_SEC,temp | 0x80);//启动计时，秒位7置为1
    write_ds1302_byte(WRITE_PROTECT,0x80);//打开写保护
}

void DS1302::stop()
{
    uint8_t temp = 0;
    write_ds1302_byte(WRITE_PROTECT,0x00);//关闭写保护
    temp = (read_ds1302_byte(READ_SEC) & 0x0f) + ((read_ds1302_byte(READ_SEC) >> 4) * 10);
    write_ds1302_byte(WRITE_SEC, 0x00); 
    write_ds1302_byte(WRITE_SEC,temp & 0x7f);//停止计时，秒位7置为0
    write_ds1302_byte(WRITE_PROTECT,0x80);//打开写保护
}

#endif