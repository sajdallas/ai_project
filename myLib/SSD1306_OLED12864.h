#ifndef _SSD1306_OLED12864_H
#define _SSD1306_OLED12864_H

#include "asr_softiic.h"
#include "twen_gfx.h"

#define OLED_CMD    0	//写命令
#define OLED_DATA   1	//写数据

#ifndef OLED_MAX_COLUMN
#define OLED_MAX_COLUMN	    128
#endif

#ifndef OLED_MAX_ROW
#define OLED_MAX_ROW		64
#endif

class SSD_OLED12864 : public TWEN_GFX
{
public:
    SSD_OLED12864(uint8_t sda_pin,uint8_t scl_pin);
    void begin(uint8_t _addr=0x3c);  //初始化
    void oled_clear(void) ;
	void oled_display(void) ;
	void draw_point(int16_t x, int16_t y, uint16_t pixel);

	void oled_show_char(int8_t x,int8_t y,int8_t chr,uint8_t size);
	void oled_show_string(int8_t x,int8_t y,int8_t *chr,uint8_t size);
	void oled_show_num(int8_t x,int8_t y,int32_t num,uint8_t size);
	void oled_show_float(int8_t x, int8_t y, float num, uint8_t precision,uint8_t size);
	void show_picture(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t* buf);

	//汉字显示函数（只适用于天问block使用）
    void OLED12864_draw_hanzi_12(uint8_t lenth, const uint8_t *hz,uint8_t x,uint8_t y);   //显示12*12汉字
    void OLED12864_draw_hanzi_16(uint8_t lenth, const uint8_t *hz,uint8_t x,uint8_t y);   //显示16*16汉字
    void OLED12864_draw_hanzi_24(uint8_t lenth, const uint8_t *hz,uint8_t x,uint8_t y);   //显示24*24汉字
    void OLED12864_draw_hanzi_32(uint8_t lenth, const uint8_t *hz,uint8_t x,uint8_t y);   //显示32*32汉字
 
private:
    KSoftIIC IIC;
	volatile uint8_t _oled_disbuffer[OLED_MAX_COLUMN][OLED_MAX_ROW/8];
	uint8_t SSD1306ADDR;
    uint8_t _sda_pin;
    uint8_t _scl_pin;
    void oled_wr_byte(uint8_t dat,uint8_t cmd);
    void oled_write_data(uint8_t IIC_Data);   //写入数据
	void oled_write_command(uint8_t IIC_Command); //写入命令
    
};
// 描述: 构造函数.
SSD_OLED12864::SSD_OLED12864(uint8_t sda_pin,uint8_t scl_pin): TWEN_GFX(OLED_MAX_COLUMN, OLED_MAX_ROW)
{
    _sda_pin = sda_pin;
	_scl_pin = scl_pin;
}
// 描述: 初始化函数.
void SSD_OLED12864::begin(uint8_t _addr=0x3c) //初始化
{
	SSD1306ADDR = (_addr << 1);

    IIC.begin(_sda_pin, _scl_pin);
    delay(200);

    oled_wr_byte(0xAE,OLED_CMD);//关闭显示
	oled_wr_byte(0x20,OLED_CMD);//---set low column address
	oled_wr_byte(0x10,OLED_CMD);//---set high column address
	oled_wr_byte(0xb0,OLED_CMD);//-not offset
	oled_wr_byte(0xc8,OLED_CMD);//-not offset
	
	oled_wr_byte(0x00,OLED_CMD);//---set low column address
	oled_wr_byte(0x10,OLED_CMD);//---set high column address
	oled_wr_byte(0x40,OLED_CMD);//-not offset

	oled_wr_byte(0x81,OLED_CMD);//设置对比度
	oled_wr_byte(0xff,OLED_CMD);

	oled_wr_byte(0xa1,OLED_CMD);//段重定向设置
	oled_wr_byte(0xa6,OLED_CMD);//
	
	oled_wr_byte(0xa8,OLED_CMD);//设置驱动路数
	oled_wr_byte(0x3f,OLED_CMD);
	oled_wr_byte(0xa4,OLED_CMD);
	
	oled_wr_byte(0xd3,OLED_CMD);
	oled_wr_byte(0x00,OLED_CMD);
	
	oled_wr_byte(0xd5,OLED_CMD);
	oled_wr_byte(0xf0,OLED_CMD);
	
	oled_wr_byte(0xd9,OLED_CMD);
	oled_wr_byte(0x22,OLED_CMD);
	
	oled_wr_byte(0xda,OLED_CMD);
	oled_wr_byte(0x12,OLED_CMD);
	
	oled_wr_byte(0xdb,OLED_CMD);
	oled_wr_byte(0x49,OLED_CMD);
	
	oled_wr_byte(0x8d,OLED_CMD);
	oled_wr_byte(0x14,OLED_CMD);
	
	oled_wr_byte(0xaf,OLED_CMD);
	oled_clear();
	set_bg_mode(1);
}
void SSD_OLED12864::oled_write_data(uint8_t IIC_Data)   //写入数据
{
    IIC.start(SSD1306ADDR);
    IIC.write(0X40);
    IIC.write(IIC_Data);
    IIC.stop();
}
void SSD_OLED12864::oled_write_command(uint8_t IIC_Command) //写入命令
{
    IIC.start(SSD1306ADDR);
    IIC.write(0X00);
    IIC.write(IIC_Command);
    IIC.stop();
}
void SSD_OLED12864::oled_wr_byte(uint8_t dat,uint8_t cmd)
{
    if(cmd)
    {
        oled_write_data(dat);
    }else{
        oled_write_command(dat);
	}
}

//========================================================================
// 描述: 清屏
// 参数: none.
// 返回: none.
//========================================================================
void SSD_OLED12864::oled_clear(void)  
{  
	uint8_t y,x;
	for(y=0;y<(OLED_MAX_ROW/8);y++)
	{
		for(x=0;x<OLED_MAX_COLUMN;x++)
		{
			_oled_disbuffer[x][y]=0x00;
		}
	}
}
//========================================================================
// 描述: 更新显示
// 参数: none.
// 返回: none.
//========================================================================
void SSD_OLED12864::oled_display(void)  
{  
    uint8_t x,y;
	
	for(y=0;y<(OLED_MAX_ROW/8);y++)  
	{  
		oled_wr_byte(0xb0+y,OLED_CMD);    //设置页地址（0-7）
		oled_wr_byte(0x02,OLED_CMD);      //设置显示位置-列低地址
		oled_wr_byte(0x10,OLED_CMD);      //设置显示位置-列高地址  
		for(x=0;x<OLED_MAX_COLUMN;x++)oled_wr_byte(_oled_disbuffer[x][y],OLED_DATA); 
	} 
    oled_wr_byte(0xaf,OLED_CMD);
}//更新显示
//========================================================================
// 描述: 在指定位置显示一个点
// 参数: x,y:坐标; pixel:亮或灭
// 返回: none.
//========================================================================
void SSD_OLED12864::draw_point(int16_t x, int16_t y, uint16_t pixel)
{ 
    if (x > OLED_MAX_COLUMN || y > OLED_MAX_ROW) {
        return;
    }
    else
    {
        if (pixel) {
            _oled_disbuffer[x][y / 8] |= (1 << (y % 8));
        }
        else {
            _oled_disbuffer[x][y / 8] &= ~(1 << (y % 8));
        }
    }
}
//========================================================================
// 描述: 在指定位置显示一个字符
// 参数: x,y:起点坐标; chr:要显示的字符;
// 返回: none.
//========================================================================

void SSD_OLED12864::oled_show_char(int8_t x,int8_t y,int8_t chr,uint8_t size)
{
	uint8_t temp;
	uint8_t j,i;
	set_text_size(size);
	set_cursor(x,y);
	write(chr);	
}
//========================================================================
// 描述: 显示一个字符串
// 参数: x,y:起点坐标;chr:要显示的字符串
// 返回: none.
//========================================================================
void SSD_OLED12864::oled_show_string(int8_t x,int8_t y,int8_t *chr,uint8_t size)
{
	uint8_t j=0;
	set_text_size(size);
	set_cursor(x,y);
	while (chr[j]!='\0')
	{	
		write(chr[j++]);	
	}
}
//========================================================================
// 描述: 在指定位置显示数字
// 参数: x,y:坐标; num:显示的数字; 
// 返回: none.
//========================================================================
void SSD_OLED12864::oled_show_num(int8_t x,int8_t y,int32_t num,uint8_t size)
{
	char men[10];
	sprintf(men,"%d",num);
	oled_show_string(x,y,(int8_t *)men,size);
} 

// ========================================================================
// 描述: 显示小数
// 参数: x,y:起点坐标;num:要显示的数值;precision:精度（1-4）.
// 返回: none.
// ========================================================================
void SSD_OLED12864::oled_show_float(int8_t x, int8_t y, float num, uint8_t precision,uint8_t size)
{
    uint8_t i;
    int32_t int_part,float_part;
	int_part = (int32_t)num/1;

    if(int_part>=0)			//整数部分
    {
		oled_show_num( x, y, int_part,size);
        for(i=1;i<10;i++)
        {
            int_part=int_part/10;
            if(int_part == 0)
            {
                break;
            }
        }
		switch(precision)
		{
			case 1:float_part = (int32_t)(num*10)%10;break;//保留1位精度
			case 2:float_part = (int32_t)(num*100)%100;break;//保留2位精度
			case 3:float_part = (int32_t)(num*1000)%1000;break;//保留3位精度
			case 4:float_part = (int32_t)(num*10000)%10000;break;//保留4位精度
			default:float_part = (int32_t)(num*10)%10;break;//保留1位精度1
		}
		if(float_part<0)
		{
			float_part = -float_part;
		}
		oled_show_char( x+(size/2)*i, y, '.',size);	//显示小数点
		oled_show_num( x+(size/2)*(i+1), y,float_part,size);	//显示小数部分
    }
    else
    {
		oled_show_num( x, y, int_part,size);
        for(i=1;i<10;i++)
        {
            int_part=int_part/10;
            if(int_part == 0)
            {
                break;
            }
        }
		switch(precision)
		{
			case 1:float_part = (int32_t)(num*10)%10;break;//保留1位精度
			case 2:float_part = (int32_t)(num*100)%100;break;//保留2位精度
			case 3:float_part = (int32_t)(num*1000)%1000;break;//保留3位精度
			case 4:float_part = (int32_t)(num*10000)%10000;break;//保留4位精度
			default:float_part = (int32_t)(num*10)%10;break;//保留1位精度
		}
		if(float_part<0)
		{
			float_part = -float_part;
		}
		oled_show_char( x+(size/2)*(i+1), y, '.',size);	//显示小数点
		oled_show_num( x+(size/2)*(i+2), y, float_part,size);	//显示小数部分
    }
} 

/***********************************************************************
 * @brief  显示图片.
 * @param  x0    x起始坐标.
 * @param  y0    y起始坐标.
 * @param  x1    x结束坐标.
 * @param  y1    y结束坐标.
 * @param  buf   图片数组指针.
 * @return none.
 **********************************************************************/
void SSD_OLED12864::show_picture(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t* buf)
{
    uint16_t m = 0;
	uint8_t j,i,k;
    uint8_t x1;
    uint8_t x2;
    uint8_t temp;

    uint8_t remainder_x;
    uint8_t remainder_y;
   
    x2 = ((w%8) ? (w/8+1):(w/8));//总用占用字节
    remainder_x = (OLED_MAX_COLUMN - x0) < w ? (w + x0 - OLED_MAX_COLUMN ): 0;
    w -= remainder_x;
    remainder_y = (OLED_MAX_ROW - y0) < h ? (h + y0 - OLED_MAX_ROW): 0;
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

void SSD_OLED12864::OLED12864_draw_hanzi_12(uint8_t lenth, const uint8_t *hz,uint8_t x,uint8_t y)   //显示12*12汉字
{
	set_cursor(x,y);
	draw_hanzi_12(lenth, hz);
}

void SSD_OLED12864::OLED12864_draw_hanzi_16(uint8_t lenth, const uint8_t *hz,uint8_t x,uint8_t y)   //显示12*12汉字
{
	set_cursor(x,y);
	draw_hanzi_16(lenth, hz);
}

void SSD_OLED12864::OLED12864_draw_hanzi_24(uint8_t lenth, const uint8_t *hz,uint8_t x,uint8_t y)   //显示12*12汉字
{
	set_cursor(x,y);
	draw_hanzi_24(lenth, hz);
}

void SSD_OLED12864::OLED12864_draw_hanzi_32(uint8_t lenth, const uint8_t *hz,uint8_t x,uint8_t y)   //显示12*12汉字
{
	set_cursor(x,y);
	draw_hanzi_32(lenth, hz);
}









#endif