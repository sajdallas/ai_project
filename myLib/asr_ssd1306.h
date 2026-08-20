/**
 * @file asr_ssd1306.h
 * @brief 
 * @author LI (com)
 * @version 1.0
 * @date 2023-11-08
 * 
 * @copyright Copyright (c) 2023 haohaodada.com
 * 
 */
#ifndef _SSD1306_h_
#define _SSD1306_h_

#include "asr_softiic.h"
#include "twen_gfx.h"
#include "string.h"
#include "stdio.h"

class SSD1306 : public TWEN_GFX
{
public:
	SSD1306(int16_t w, int16_t h, uint8_t sda_pin,uint8_t scl_pin);
    SSD1306(int16_t w, int16_t h);
	void begin(uint8_t _addr=0x3c);  ////初始化
	void clear(uint8_t dian=0);   ////清屏
	uint8_t display(void);  //显示

    void draw_point(int16_t x, int16_t y,uint16_t dian);    //画点函数
    void show_picture(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t* buf);
private:
    KSoftIIC softiic;
	volatile unsigned char disbuffer[128][8];
	uint8_t SSD1306ADDR=0x3c;
    uint8_t SDA_pin;
    uint8_t SCL_pin;
    int16_t _w;
    int16_t _h;
    uint8_t _connected;
    uint8_t writec(uint8_t icommand);   //写入命令
	uint8_t writed(uint8_t idata);  //写入数据
};

/**
 * @brief 构造函数(软件IIC)
 * @param  w                屏幕分辨率x
 * @param  h                屏幕分辨率y
 * @param  sda_pin          SDA引脚
 * @param  scl_pin          scl引脚
 */
SSD1306::SSD1306(int16_t w, int16_t h, uint8_t sda_pin,uint8_t scl_pin) : TWEN_GFX(w, h)
{
    _w = w;
    _h = h;
    SDA_pin=sda_pin;
	SCL_pin=scl_pin;
    _connected = 0;
}
/**
 * @brief 构造函数(硬件IIC)
 * @param  w                屏幕分辨率x
 * @param  h                屏幕分辨率y
 */
SSD1306::SSD1306(int16_t w, int16_t h) : TWEN_GFX(w, h)
{
    _w = w;
    _h = h;
    _connected = 1;
}

/**
 * @brief 初始化函数
 * @param  _addr            iic地址（默认为0x3c）
 */
void SSD1306::begin(uint8_t _addr)
{
    delay(300);//初始化之前的延时很重要！
    if(_connected == 0){
        softiic.begin(SDA_pin, SCL_pin);
        SSD1306ADDR = (_addr<<1);
    }else{  //使用硬件IIC
    #if defined(TW_ASR_PRO)
        dpmu_set_io_reuse(PA2,THIRD_FUNCTION);
        dpmu_set_io_reuse(PA3,THIRD_FUNCTION);//配置IIC引脚
        iic_polling_init(IIC0,100,0,LONG_TIME_OUT);
    #else
        i2c_io_init();
        i2c_master_init(IIC0,100,LONG_TIME_OUT);
    #endif
        SSD1306ADDR = _addr;
    }

    if( (_w == 128)&&(_h == 64) )
    {
        writec(0xAE);//--turn off oled panel
        writec(0x00);//---set low column address
        writec(0x10);//---set high column address
        writec(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
        writec(0x81);//--set contrast control register
        writec(0xCF); // Set SEG Output Current Brightness
        writec(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
        writec(0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
        writec(0xa6);//--set normal display
        writec(0xa8);//--set multiplex ratio(1 to 64)
        writec(0x3f);//--1/64 duty
        writec(0xd3);//-set display offset  Shift Mapping RAM Counter (0x00~0x3F)
        writec(0x00);//-not offset
        writec(0xd5);//--set display clock divide ratio/oscillator frequency
        writec(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
        writec(0xd9);//--set pre-charge period
        writec(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
        writec(0xda);//--set com pins hardware configuration
        writec(0x12);
        writec(0xdb);//--set vcomh
        writec(0x40);//Set VCOM Deselect Level
        writec(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
        writec(0x00);//
        writec(0x8d);//--set Charge Pump enable/disable
        writec(0x14);//--set(0x10) disable
        writec(0xa4);// Disable Entire Display On (0xa4/0xa5)
        writec(0xa6);// Disable Inverse Display On (0xa6/a7) 
        writec(0xaf);//--turn on oled panel
    }else if( (_w == 128)&&(_h == 32) )
    {
        writec(0xAE);//关闭显示	
        writec(0x40);//---set low column address
        writec(0xB0);//---set high column address
        writec(0xC8);//-not offset
        writec(0x81);//设置对比度
        writec(0xff);
        writec(0xa1);//段重定向设置
        writec(0xa6);//
        writec(0xa8);//设置驱动路数
        writec(0x1f);
        writec(0xd3);
        writec(0x00);
        writec(0xd5);
        writec(0xf0);
        writec(0xd9);
        writec(0x22);
        writec(0xda);
        writec(0x02);
        writec(0xdb);
        writec(0x49);
        writec(0x8d);
        writec(0x14);
        writec(0xaf);//--turn on oled panel
    }else if( (_w == 72)&&(_h == 40) )
    {
        writec(0xAE); /*display off*/
        writec(0xD5); /*set osc division*/
        writec(0xF0);
        writec(0xA8); /*multiplex ratio*/
        writec(0x27); /*duty = 1/40*/
        writec(0xD3); /*set display offset*/
        writec(0x00);
        writec(0x40); /*Set Display Start Line */
        writec(0x8d); /*set charge pump enable*/
        writec(0x14);
        writec(0x20); /*Set page address mode*/
        writec(0x02);
        writec(0xA1); /*set segment remap*/
        writec(0xC8); /*Com scan direction*/
        writec(0xDA); /*set COM pins*/
        writec(0x12);
        writec(0xAD); /*Internal IREF Setting*/
        writec(0x30);
        writec(0x81); /*contract control*/
        writec(0xfF); /*128*/
        writec(0xD9); /*set pre-charge period*/
        writec(0x22);
        writec(0xdb); /*set vcomh*/
        writec(0x20);
        writec(0xA4); /*Set Entire Display On/Off*/
        writec(0xA6); /*normal / reverse*/
        writec(0x0C); /*set lower column address*/
        writec(0x11); /*set higher column address*/ 
    }else
    {
        // Other screen varieties
    }
    set_bg_mode(1);
}

/**
 * @brief 写入命令
 * @param  icommand         命令.
 * @return none  
 */
uint8_t SSD1306::writec(uint8_t icommand)
{
    if(_connected == 0){
        softiic.start(SSD1306ADDR|KI2C_WRITE);
        softiic.write(0x80);
        softiic.write(icommand);
        softiic.stop();
    }else{
        char buf[2] = {0};
        buf[0] = 0x80;
        buf[1] = icommand;
        i2c_master_only_send(SSD1306ADDR,buf,2);
    }
    return true;
}

/**
 * @brief 写入数据
 * @param  idata            数据.
 * @return none 
 */
uint8_t SSD1306::writed(uint8_t idata)
{
    if(_connected == 0){
        softiic.start(SSD1306ADDR|KI2C_WRITE);
        softiic.write(0x40);
        softiic.write(idata);
        softiic.stop();
    }else{
        char buf[2] = {0};
        buf[0] = 0x40;
        buf[1] = idata;
        i2c_master_only_send(SSD1306ADDR,buf,2);
    }
    return true;
}

/**
 * @brief 清屏
 * @param  dian             0(缓存全清0) 1(缓存全清1)
 */
void SSD1306::clear(uint8_t dian)
{
	unsigned char x,y;
    if(dian){
        for(y=0;y<(_h/8);y++)
        {
            for(x=0;x<_w;x++){
                disbuffer[x][y]=0xff;
            }
        }
    }else{
        for(y=0;y<(_h/8);y++)
        {
            for(x=0;x<_w;x++){
                disbuffer[x][y]=0x00;
            }
        }
    }
}

/**
 * @brief 更新显示
 * @return none 
 */
uint8_t SSD1306::display(void)
{
	uint8_t x,y;
   // taskENTER_CRITICAL();//进入临界区
    for(y=0;y<(_h/8);y++)
    {
        writec(0xb0+y);
        writec(0x00);
        writec(0x10);
        for(x=0;x<_w;x++)
        {
            writed(disbuffer[x][y]);
        }
    }
   // taskEXIT_CRITICAL();
   
	// writec(0x21);
	// writec(0);
	// writec(_w - 1);
	// writec(0x22);
	// writec(0);
	// writec(_h/8 - 1); 
	// softiic.start(SSD1306ADDR|KI2C_WRITE);
	// softiic.write(0x40);    //д
	// for(y=0;y<(_h/8);y++)
	// {
	// 	for(x=0;x<_w;x++){
    //         softiic.write(disbuffer[x][y]);
	// 	}
	// }
	// softiic.stop();
	return true;
}

/**
 * @brief 画点
 * @param  x                X坐标
 * @param  y                Y坐标
 * @param  dian             1,亮，0灭.
 */
void SSD1306::draw_point(int16_t x, int16_t y,uint16_t dian)
{ 
    if ((x > (_w-1)) || (y > (_h-1))) {
        return;
    }
    else 
    {
        if (dian) {
            disbuffer[x][y / 8] |= (1 << (y % 8));
        }
        else {
            disbuffer[x][y / 8] &= ~(1 << (y % 8));
        }
    }
} 

/**
 * @brief 显示图片
 * @param  x0               X起始坐标
 * @param  y0               Y起始坐标
 * @param  x1               X结束坐标
 * @param  y1               Y结束坐标
 * @param  buf              图片数组地址
 */
void SSD1306::show_picture(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t* buf)
{
    uint16_t m = 0;
	uint8_t j,i,k;
    uint8_t x1;
    uint8_t x2;
    uint8_t temp;

    uint8_t remainder_x;
    uint8_t remainder_y;
   
    x2 = ((w%8) ? (w/8+1):(w/8));//总用占用字节
    remainder_x = (_w - x0) < w ? (w + x0 - _w ): 0;
    w -= remainder_x;
    remainder_y = (_h - y0) < h ? (h + y0 - _h): 0;
    h -= remainder_y;
    x1 = ((w%8) ? (w/8+1):(w/8));//占用字节

   
   	for(i=0;i<h;i++)
	{
		for(j=0;j<x1;j++)
		{
            temp = buf[m++];
            for(k=0;k<8;k++)
            {
                if(j*8+k == w)
                    break;
                if(temp & 0x80)
                {
                   draw_point(x0+j*8+k, y0+i, 1);
                }
                else
                {
                    draw_point(x0+j*8+k, y0+i, 0);    
                }
                temp <<= 1;
            }
		}
       m += (x2 - x1);
	}
}

#endif  //ssd1306.h