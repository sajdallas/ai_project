#ifndef _DS3231_H_
#define _DS3231_H_ 

#include "asr.h"
#include "asr_softiic.h"

//设备地址
#define     DS3231_ADDRESS              0x68
//寄存器地址
#define     DS3231_SEC_REG              0x00
#define     DS3231_MIN_REG              0x01
#define     DS3231_HOUR_REG             0x02
#define     DS3231_WDAY_REG             0x03
#define     DS3231_MDAY_REG             0x04
#define     DS3231_MONTH_REG            0x05
#define     DS3231_YEAR_REG             0x06

#define     DS3231_AL1SEC_REG           0x07
#define     DS3231_AL1MIN_REG           0x08
#define     DS3231_AL1HOUR_REG          0x09
#define     DS3231_AL1WDAY_REG          0x0A

#define     DS3231_AL2MIN_REG           0x0B
#define     DS3231_AL2HOUR_REG          0x0C
#define     DS3231_AL2WDAY_REG          0x0D

#define     DS3231_CONTROL_REG          0x0E
#define     DS3231_STATUS_REG           0x0F
#define     DS3231_AGING_OFFSET_REG     0x10
#define     DS3231_TMP_UP_REG           0x11
#define     DS3231_TMP_LOW_REG          0x12

//
#define     DS3231_EVERYSECOND          0x01
#define     DS3231_EVERYMINUTE          0x02
#define     DS3231_EVERYHOUR            0x03

//
#define     DS3231_HARDIIC              0   
#define     DS3231_SOFTIIC              1

class DS3231
{  
public:
    DS3231(uint8_t addr = DS3231_ADDRESS);
    DS3231(uint8_t sdapin, uint8_t sclpin, uint8_t addr = DS3231_ADDRESS);
    void begin(); //初始化
    void write_rtc(); //写入时间
    void read_rtc(); //读出时间
    uint8_t get_weekday(uint16_t year,uint8_t month, uint8_t day);  //通过年月日获取星期
    void enable_interrupts_mode1(uint8_t periodicity); //设置闹钟中断模式1
    void enable_interrupts_mode2(uint8_t hh24, uint8_t mm, uint8_t ss); //设置闹钟中断模式2
    void disable_interrupts(); //禁止中断
    void clear_interrupt(); //清除中断标志
    void convert_temperature(); //转换温度
    float get_temperature(); //获取温度
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t weekday;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

private:
    KSoftIIC softiic;
    uint8_t _addr;
    uint8_t _sdapin;
    uint8_t _sclpin;
    uint8_t _connect;
    void write_byte(uint8_t reg, uint8_t value);
    uint8_t read_byte(uint8_t reg);
    void write_nbyte(uint16_t addr, uint8_t *pbuf, uint16_t len);
    void read_nbyte(uint16_t addr, uint8_t *pbuf, uint16_t len);
    uint8_t bcd2bin(uint8_t val);
    uint8_t bin2bcd(uint8_t val);
};

DS3231::DS3231(uint8_t addr = DS3231_ADDRESS)
{
    _addr = addr;
    _connect = DS3231_HARDIIC;
}

DS3231::DS3231(uint8_t sdapin, uint8_t sclpin,uint8_t addr = DS3231_ADDRESS)
{
    _addr = addr;
    _connect = DS3231_SOFTIIC;
    _sdapin = sdapin;
    _sclpin = sclpin;
}

//========================================================================
// 描述: 写数据.
// 参数: addr:写入的寄存器地址; p:写入的数据.
// 返回: none.
//========================================================================
void DS3231::write_byte(uint8_t reg, uint8_t value) 
{
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = value;
    if(_connect == DS3231_HARDIIC){
#if defined(tskKERNEL_VERSION_NUMBER)
        i2c_master_only_send(_addr,buf,2);
#endif
    }else{
#if defined(tskKERNEL_VERSION_NUMBER)
        vTaskSuspendAll();
#endif
        softiic.start((_addr<<1)|KI2C_WRITE);
        softiic.write(buf[0]);
        softiic.write(buf[1]);
        softiic.stop();
#if defined(tskKERNEL_VERSION_NUMBER)
        ( void ) xTaskResumeAll();
#endif
    }
}

//========================================================================
// 描述: 读取数据.
// 参数: addr:读取的寄存器地址.
// 返回: 读到的数据.
//========================================================================
uint8_t DS3231::read_byte(uint8_t reg) 
{
    uint8_t buf[2] = {0};
    buf[0] = reg;
    if(_connect == DS3231_HARDIIC){
#if defined(tskKERNEL_VERSION_NUMBER)
        i2c_master_send_recv(_addr,buf,1,1);
#endif
    }else{
#if defined(tskKERNEL_VERSION_NUMBER)
        vTaskSuspendAll();
#endif
        softiic.start((_addr<<1)|KI2C_WRITE);
        softiic.write((uint8_t)reg);
        softiic.restart((_addr<<1)|KI2C_READ);
        buf[0] = softiic.read(1);
        softiic.stop();
#if defined(tskKERNEL_VERSION_NUMBER)
        ( void ) xTaskResumeAll();
#endif
    }
    return buf[0];
}

//========================================================================
// 描述: 写多个字节数据.
// 参数: addr:写入的寄存器地址; p:写入的数据; number:需要写的数据个数.
// 返回: none.
//========================================================================
void DS3231::write_nbyte(uint16_t addr, uint8_t *pbuf, uint16_t len)
{
    uint8_t i;
    for(i=0;i<len;i++)
    {
        write_byte(addr, *pbuf);
        // delay(5);
        addr++;
        pbuf++;
    }
}

//========================================================================
// 描述: 读取多个数据.
// 参数: addr:读取的寄存器地址; p:读回来的数据; number:需要读的数据个数.
// 返回: none.
//========================================================================
void DS3231::read_nbyte(uint16_t addr, uint8_t *pbuf, uint16_t len)
{
    uint8_t i;
    for(i=0;i<len;i++)
    {
        *pbuf = read_byte(addr);
        addr++;
        pbuf++;
    }
}

//========================================================================
// 描述: 初始化.
// 参数: none.
// 返回: none.
//========================================================================
void DS3231::begin()
{
    uint8_t value = 0;

    if(_connect == DS3231_HARDIIC){
#if defined(tskKERNEL_VERSION_NUMBER)
   #if defined(TW_ASR_PRO)
        dpmu_set_io_reuse(PA2,THIRD_FUNCTION);
        dpmu_set_io_reuse(PA3,THIRD_FUNCTION);//配置IIC引脚
        iic_polling_init(IIC0,100,0,LONG_TIME_OUT);
    #else
        i2c_io_init();
        i2c_master_init(IIC0,100,LONG_TIME_OUT);
    #endif
#endif
    }else{
        softiic.begin(_sdapin,_sclpin);
    }
    write_byte(DS3231_CONTROL_REG,0x1c);    //CONTROL Register Address
    // delay(10);
    value = read_byte(DS3231_HOUR_REG);  
    value &= 0xBF;
    write_byte(DS3231_HOUR_REG, value);  //设置小时模式为24h
    // delay(10);
}

//========================================================================
// 描述: 根据年月日获取星期.
// 参数: 年月日.
// 返回: none.
//========================================================================
uint8_t DS3231::get_weekday(uint16_t year,uint8_t month, uint8_t day)
{
    int c,y,w;
    uint8_t dow;
  	int m = month;
    int d = day;  // 根据月份对年份和月份进行调整
    if(m <= 2)
    {
        year -= 1;
        m += 12;
    }
    c =year / 100; // 取得年份前两位
    y =year % 100; // 取得年份后两位
    w = (int)(c/4) - 2*c + y + (int)(y/4) + (int)(13*(m+1)/5) + d - 1;   // 根据泰勒公式计算星期
    if((w%7)==0){
       dow=7;
    }else{
      dow=w%7;
    }
    return dow;
}

//========================================================================
// 描述: bcd转bin.
// 参数: none.
// 返回: none.
//========================================================================
uint8_t DS3231::bcd2bin(uint8_t val) 
{ 
    return val - 6 * (val >> 4); 
}

//========================================================================
// 描述: bin转bcd.
// 参数: none.
// 返回: none.
//========================================================================
uint8_t DS3231::bin2bcd(uint8_t val) 
{ 
    return val + 6 * (val / 10); 
}

//========================================================================
// 描述: 写rtc数据.
// 参数: 年，月，日，星期(1-7)，时，分，秒.
// 返回: none.
//========================================================================
void DS3231::write_rtc()
{
    uint8_t  tmp[7];
    year = year - 2000;

    tmp[0] = bin2bcd(second);      //秒
    tmp[1] = bin2bcd(minute);      //分
    tmp[2] = bin2bcd(hour & 0xBF);   //小时
    tmp[3] = weekday;                 //星期
    tmp[4] = bin2bcd(day);            //日
    tmp[5] = bin2bcd(month);        //月
    tmp[6] = bin2bcd(year);          //年

    write_nbyte(DS3231_SEC_REG, tmp, 7);
}

//========================================================================
// 描述: 读取rtc数据.
// 参数: none.
// 返回: none.
//========================================================================
void DS3231::read_rtc()
{
    uint8_t  tmp[7];

    read_nbyte(DS3231_SEC_REG, tmp, 7);
    
    second = bcd2bin(tmp[0]);    //秒钟
    minute = bcd2bin(tmp[1]);    //分钟
    hour   = bcd2bin(tmp[2] & 0x3F);    //小时

    weekday = tmp[3];                    //星期
    day = bcd2bin(tmp[4]);                //日
    month = bcd2bin(tmp[5]);              //月
    year = 2000 + bcd2bin(tmp[6]);        //年
}

//========================================================================
// 描述: 使能中断.
// 参数: DS3231_EVERYSECOND:每秒触发;DS3231_EVERYMINUTE:每分钟触发;DS3231_EVERYHOUR:每小时触发.
// 返回: none.
//========================================================================
void DS3231::enable_interrupts_mode1(uint8_t periodicity)
{
    write_byte(DS3231_CONTROL_REG, 0x1d);     //闹钟1中断使能
        
    switch(periodicity) 
    {
        case DS3231_EVERYSECOND:
            write_byte(DS3231_AL1SEC_REG,  0x80 ); //set AM1
            write_byte(DS3231_AL1MIN_REG,  0x80 ); //set AM2
            write_byte(DS3231_AL1HOUR_REG, 0x80 ); //set AM3
            write_byte(DS3231_AL1WDAY_REG, 0x80 ); //set AM4
            break;

        case DS3231_EVERYMINUTE:
            write_byte(DS3231_AL1SEC_REG,  0x00 ); //Clr AM1
            write_byte(DS3231_AL1MIN_REG,  0x80 ); //set AM2
            write_byte(DS3231_AL1HOUR_REG, 0x80 ); //set AM3
            write_byte(DS3231_AL1WDAY_REG, 0x80 ); //set AM4
            break;

        case DS3231_EVERYHOUR:
            write_byte(DS3231_AL1SEC_REG,  0x00 ); //Clr AM1
            write_byte(DS3231_AL1MIN_REG,  0x00 ); //Clr AM2
            write_byte(DS3231_AL1HOUR_REG, 0x80 ); //Set AM3
            write_byte(DS3231_AL1WDAY_REG, 0x80 ); //set AM4
            break;
    }
}

//========================================================================
// 描述: 使能中断.
// 参数: 设置闹钟的时间
//         hh24:小时(24小时制); mm:分钟; ss:秒.
// 返回: none.
//========================================================================
void DS3231::enable_interrupts_mode2(uint8_t hh24, uint8_t mm, uint8_t ss)
{
    write_byte(DS3231_CONTROL_REG, 0x1d);     //闹钟1中断使能

    write_byte(DS3231_AL1SEC_REG,  0x00 | bin2bcd(ss) ); //Clr AM1
    write_byte(DS3231_AL1MIN_REG,  0x00 | bin2bcd(mm)); //Clr AM2
    write_byte(DS3231_AL1HOUR_REG, (0x00 | (bin2bcd(hh24) & 0xBF))); //Clr AM3
    write_byte(DS3231_AL1WDAY_REG, 0x80 ); //set AM4
}

//========================================================================
// 描述: 禁止中断.
// 参数: none.
// 返回: none.
//========================================================================
void DS3231::disable_interrupts()
{
    uint8_t value;
    write_byte(DS3231_CONTROL_REG,0x1c);    //CONTROL Register Address
    delay(10);
    value = read_byte(DS3231_HOUR_REG);  
    value &= 0xBF;
    write_byte(DS3231_HOUR_REG, value);  //设置小时模式为24h
    delay(10);
}

//========================================================================
// 描述: 清除中断标志.
// 参数: none.
// 返回: none.
//========================================================================
void DS3231::clear_interrupt()
{
    // Clear interrupt flag 
    uint8_t statusReg;
    statusReg = read_byte(DS3231_STATUS_REG) & 0xFE;
    write_byte(DS3231_STATUS_REG, statusReg);
}

//========================================================================
// 描述: 温度转换.
// 参数: none.
// 返回: none.
//========================================================================
void DS3231::convert_temperature()
{
    // Set CONV 
    uint8_t ctReg;
    ctReg = read_byte(DS3231_CONTROL_REG) |  0x20;
    write_byte(DS3231_CONTROL_REG,ctReg);            //转换温度
 
    //wait until CONV is cleared. Indicates new temperature value is available in register.
    do
    {
       //do nothing
    } while ((read_byte(DS3231_CONTROL_REG) & 0x20) == 0x20 ); 
 
}

//========================================================================
// 描述: 获取温度.
// 参数: none.
// 返回: none.
//========================================================================
float DS3231::get_temperature()
{
    float   fTemperatureCelsius;
    uint8_t   tUBYTE;
    uint8_t   tLRBYTE;
    tUBYTE  = read_byte(DS3231_TMP_UP_REG);  //Two's complement form
    tLRBYTE = read_byte(DS3231_TMP_LOW_REG); //Fractional part

    if(tUBYTE & 0x80) //check if -ve number
    {
       tUBYTE  ^= 0xff;  
       tUBYTE  += 0x1;
       fTemperatureCelsius = tUBYTE + ((tLRBYTE >> 6) * 0.25);
       fTemperatureCelsius = fTemperatureCelsius * -1;
    }
    else
    {
        fTemperatureCelsius = tUBYTE + ((tLRBYTE >> 6) * 0.25); 
    }
 
    return (fTemperatureCelsius);
}

#endif