#ifndef _SH1106_h_
#define _SH1106_h_

#include "twen_gfx.h"
#include "string.h"
#include "stdio.h"
#include "asr_softiic.h"



class SH1106: public TWEN_GFX
{
public:
    SH1106(int16_t w, int16_t h);
	SH1106(int16_t w, int16_t h,uint8_t sda_pin,uint8_t scl_pin);
	void begin(uint8_t _addr = 0x3c);  //初始化
	void clear(uint8_t dian);   //清屏
	uint8_t display(void);  //更新显示
    void draw_point(int16_t x, int16_t y,uint16_t dian);
    void show_picture(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t* buf);
private:
    KSoftIIC softiic;
    volatile uint8_t disbuffer[128][8];
    uint8_t SH1106_ADDR;
    int16_t _w;
    int16_t _h;
    uint8_t _sda_pin;
    uint8_t _scl_pin;
    uint8_t writec(uint8_t icommand);   //写入命令
    uint8_t writed(uint8_t idata);  //写入数据
};
/**
 * @brief 硬件IIC
 * @param  w                
 * @param  h                
 */
SH1106::SH1106(int16_t w, int16_t h): TWEN_GFX(w, h)
{
    _w = w;
    _h = h; 
}
/**
 * @brief 软件IIC
 * @param  w                x轴像素点
 * @param  h                y轴像素点
 * @param  sda_pin          SDA引脚
 * @param  scl_pin          SCL引脚
 */
SH1106::SH1106(int16_t w, int16_t h,uint8_t sda_pin,uint8_t scl_pin): TWEN_GFX(w, h)
{
    _w = w;
    _h = h;
    _sda_pin = sda_pin;
    _scl_pin = scl_pin;
}

//========================================================================
// 描述: 初始化.
// 参数: none.
// 返回: none.
//========================================================================
void SH1106::begin(uint8_t _addr)
{
    delay(300);//初始化之前的延时很重要！
    SH1106_ADDR = _addr ;
#ifdef SH1106_USE_HARDIIC
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
#else
    softiic.begin(_sda_pin, _scl_pin);
#endif   
    writec(0xae);//--turn off oled panel
    writec(0x00);//---set low column address
    writec(0x10);//---set high column address
    writec(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    writec(0x81);//--set contrast control register
    writec(0xFF); // Set SEG Output Current Brightness,CF
    writec(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    writec(0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    writec(0xa6);//--set normal display
    writec(0xa8);//--set multiplex ratio(1 to 64)
    writec(0x3f);//--1/64 duty
    writec(0xd3);//-set display offset  Shift Mapping RAM Counter (0x00~0x3F)
    writec(0x00);//-not offset
    writec(0xd5);//--set display clock divide ratio/oscillator frequency
    writec(0x50);//--set divide ratio, Set Clock as 100 Frames/Sec-80
    writec(0xd9);//--set pre-charge period
    writec(0x22);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    writec(0xda);//--set com pins hardware configuration
    writec(0x12);
    writec(0xdb);//--set vcomh
    writec(0x40);//Set VCOM Deselect Level
    writec(0xa4);// Disable Entire Display On (0xa4/0xa5)
    writec(0xa6);// Disable Inverse Display On (0xa6/a7) 
    writec(0xaf);//--turn on oled panel
    set_bg_mode(1);
}

//========================================================================
// 描述: 写命令.
// 参数: none.
// 返回: none.
//========================================================================
uint8_t SH1106::writec(uint8_t icommand)
{
    uint8_t buf[2];
    buf[0] = 0x80;
    buf[1] = icommand;
#if defined(SH1106_USE_HARDIIC) 
#if defined(tskKERNEL_VERSION_NUMBER)
        i2c_master_only_send(SH1106_ADDR,buf,2);
#endif
#else

    softiic.start(SH1106_ADDR<<1);
    softiic.write(0x80);
    softiic.write(icommand);
    softiic.stop();

#endif
    return true;
}

//========================================================================
// 描述: 写数据.
// 参数: none.
// 返回: none.
//========================================================================
uint8_t SH1106::writed(uint8_t idata)
{
    uint8_t buf[2];
    buf[0] = 0x40;
    buf[1] = idata;
#if defined(SH1106_USE_HARDIIC) 
#if defined(tskKERNEL_VERSION_NUMBER)
        i2c_master_only_send(SH1106_ADDR,buf,2);
#endif
#else

    softiic.start(SH1106_ADDR<<1);
    softiic.write(0x40);
    softiic.write(idata);
    softiic.stop();

#endif
    return true;
}

//========================================================================
// 描述: 清屏.
// 参数: none.
// 返回: none.
//========================================================================
void SH1106::clear(uint8_t dian)
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

//========================================================================
// 描述: 更新显示.
// 参数: none.
// 返回: none.
//========================================================================
uint8_t SH1106::display(void)
{
	uint8_t x,y;
    for(y=0;y<(_h/8);y++)
    {
        writec(0xb0+y);
        writec(0x02);
        writec(0x10);
        for(x=0;x<_w;x++)
        {
            writed(disbuffer[x][y]);
        }
    }
    /*  for(y=0;y<8;y++)
     {
       writec(0xb0+y);
       writec(0x01);
       writec(0x10);
       for(x=0;x<128;x++){
         writed(disbuffer[x][y]);
        }
     } */

    return true;
}

//========================================================================
// 描述: 画点.
// 参数: x,y:位置; dian:亮或灭.
// 返回: none.
//========================================================================
void SH1106::draw_point(int16_t x, int16_t y,uint16_t dian)
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
//========================================================================
// 描述: 显示图片.
// 参数: x0,y0:起始坐标; x1,y1:结束坐标;buf:图片数组.
// 返回: none.
//========================================================================
void SH1106::show_picture(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t* buf)
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
#endif