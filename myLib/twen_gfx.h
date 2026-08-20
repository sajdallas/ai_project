/**
 * @file twen_gfx.h
 * @brief 各类屏幕显示顶层类库。
 * @details 该库用于freertos系统。
 * @version 0.0.1
 * @date 2023-02-17
 * @author hhdd
 * @copyright Copyright (c) 2021 TWen51 Technology Co., Ltd.
 * 
 */

#ifndef _TWEN_GFX_H
#define _TWEN_GFX_H

#include "stdlib.h"
#include "Stream.h"
#include "Print.h"
#include "twen_font.h"
#include "QRCode.h"

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

class TWEN_GFX: public Print
{
public:
    TWEN_GFX(int16_t w, int16_t h); //宽度和高度初始化

    virtual void draw_point(int16_t x, int16_t y, uint16_t color){}; //画点函数，必须在子类中重写
	virtual void start_write(){};	//开始写入
	virtual void end_write(){};	//结束写入
	virtual void set_direction(uint8_t direction);	//设置显示方向

	void show_char(int16_t x,int16_t y, uint8_t chr, uint16_t font_color, uint16_t background_color, uint8_t size);//显示字符
	void set_text_size(uint8_t s)	//设置文本字体大小
	{
		textsize_x = s/2;
        textsize_y = s;
	}
	void set_cursor(int16_t x, int16_t y) //设置文本光标位置
	{
		cursor_x = x;
		cursor_y = y;
	}
    int16_t get_cursor_x(void) const {return cursor_x;} //获取光标的X轴位置
    int16_t get_cursor_y(void) const {return cursor_y;} //获取光标的X轴位置
	void set_text_color(uint16_t c, uint16_t bg) //设置文本字体颜色与自定义背景颜色
	{
		textcolor = c;
		textbgcolor = bg;
	}
    void set_bg_mode(uint8_t mode){_bg_mode = mode;}

	//设置文本是否超过屏幕宽度自动换行到下一行。
	void set_text_wrap(bool w) { wrap = w; }
	using Print::write;
	virtual size_t write(uint8_t);

    void draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);   //画线
    void draw_Vline(int16_t x, int16_t y, int16_t h,uint16_t color); //画垂直线
    void draw_Hline(int16_t x, int16_t y, int16_t w,uint16_t color); //画水平线

    void draw_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);  //画矩形框
    void fill_rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t color);//填充矩形

    void draw_circle(int16_t x0, int16_t y0, int16_t r,uint16_t color); //画圆
    void fill_circle(int16_t x0, int16_t y0, int16_t r,uint16_t color);//填充圆

    void draw_circle_helper(int16_t x0, int16_t y0, int16_t r,uint8_t cornername, uint16_t color); //用来协助画圆角矩形
    void fill_circle_helper(int16_t x0, int16_t y0, int16_t r,uint8_t corners, int16_t delta,uint16_t color); //用来协助填充圆角矩形   

    void draw_round_rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t radius, uint16_t color);  //画圆角矩形
    void fill_round_rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t radius, uint16_t color);  //填充圆角矩形

    void draw_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color); //画三角形
    void fill_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color); //填充三角形

	//汉字显示函数（只适用于天问block使用）
    void draw_hanzi_12(uint8_t lenth, const uint8_t *hz);   //显示12*12汉字
    void draw_hanzi_16(uint8_t lenth, const uint8_t *hz);   //显示16*16汉字
    void draw_hanzi_24(uint8_t lenth, const uint8_t *hz);   //显示24*24汉字
    void draw_hanzi_32(uint8_t lenth, const uint8_t *hz);   //显示32*32汉字

	
	void show_qrcode(uint8_t x, uint8_t y, uint8_t version, uint8_t ecc, const char *data);//显示二维码

private:
    int16_t WIDTH;  //彩屏的宽和高
    int16_t HEIGHT;
    int16_t _width;     
    int16_t _height;
    uint8_t rotation;   //彩屏显示方向
    bool wrap; //x轴自动换行
	int16_t cursor_x;	//光标位置
	int16_t cursor_y;
	uint8_t textsize_x;	//文本字体x轴宽度
    uint8_t textsize_y;	//文本字体y轴宽度
	uint16_t textcolor;	//字体颜色
	uint16_t textbgcolor; //字体的背景颜色
    uint8_t _bg_mode;
};

/**
 * @brief  初始化
 * @param  w	彩屏宽度
 * @param  h	彩屏高度
 */
TWEN_GFX::TWEN_GFX(int16_t w, int16_t h) : WIDTH(w), HEIGHT(h) 
{
    _width = WIDTH;
    _height = HEIGHT;
    rotation = 0;
    cursor_y = cursor_x = 0;
    textsize_x = 6;
	textsize_y = 12;
	textcolor = 0xFFFF;
	textbgcolor = 0xF800;
    wrap = true;
    _bg_mode = 0;
}

/**
 * @brief  设置显示方向.
 * @param  direction	取值为0-3.
 * 		      0 - 0度
 * 			  1 - 90度
 * 			  2 - 180度
 * 			  3 - 270度.
 */
void TWEN_GFX::set_direction(uint8_t direction)
{
    rotation = (direction & 3);
    switch (rotation) {
    case 0:
    case 2:
        _width = WIDTH;
        _height = HEIGHT;
        break;
    case 1:
    case 3:
        _width = HEIGHT;
        _height = WIDTH;
        break;
    }
}

/**
 * @brief  在指定位置显示一个字符.
 * @param  x	字符起点x坐标
 * @param  y	字符起点y坐标
 * @param  char	显示的字符
 * @param  font_color	字符的颜色值
 * @param  background_color	字符的背景色
 * @param  size	字符大小(12/16/24/32)
 */
void TWEN_GFX::show_char(int16_t x,int16_t y, uint8_t chr, uint16_t font_color, uint16_t background_color, uint8_t size)
{  
	uint8_t *temp = NULL;
	uint8_t j,i;
	uint16_t z;
	uint8_t row_bit; //保存一行所占用的位数（不足一字节以一字节计）
	chr=chr-' ';						  //得到偏移后的值
	if(size==12){
		row_bit = 8;
		#if defined(ASC2_12)
		temp = (uint8_t *)&asc2_1206[chr][0]; //调用1206字体
		#endif
	}else if(size==16){
		row_bit = 8;
		#if defined(ASC2_16)
		temp = (uint8_t *)&asc2_1608[chr][0]; //调用1608字体 
		#endif
	}else if(size==24){
		row_bit = 16;
		#if defined(ASC2_24)
		temp = (uint8_t *)&asc2_2412[chr][0]; //调用2412字体 
		#endif
	}else if(size==32){
		row_bit = 16;
		#if defined(ASC2_32)
		temp = (uint8_t *)&asc2_3216[chr][0]; //调用3216字体 
		#endif
	}else{
		return;	//不存在该字体则直接返回
	} 

	if(x < 0)
	{
		for(j=0;j<size;j++)
		{
			for(i=0;i<size/2;i++)
		    {   
				if(((x + i) >= 0))
				{   
					if((x+i-size/2) > _width)return;
					if((y+size) > _height)return;
					if(!_bg_mode) //非叠加方式
					{
						z = j*row_bit+i; 
						if(temp[z/8]&(0x01<<(z%8))){
							draw_point(x+i,y+j,font_color);
						}
						else{
							draw_point(x+i,y+j,background_color);
						}
					}
					else
					{     
						z = j*row_bit+i;        
						if(temp[z/8]&(0x01<<(z%8))){
							draw_point(x+i,y+j,font_color);	
						}
					}
				}
		    }
		}	
	}else{
		for(j=0;j<size;j++)
		{
			for(i=0;i<size/2;i++)
		    {      
				if((x+i-size/2) > _width)return;
				if((y+size) > _height)return;
				if(!_bg_mode) //非叠加方式
				{
					z = j*row_bit+i; 
					if(temp[z/8]&(0x01<<(z%8))){
						draw_point(x+i,y+j,font_color);	
					}
					else{
						draw_point(x+i,y+j,background_color);	 
					}
				}
				else
				{        
					z = j*row_bit+i;     
					if(temp[z/8]&(0x01<<(z%8))){
						draw_point(x+i,y+j,font_color);	
					}
				}
		    }
		}	
	}   	 	  
}

/**
 * @brief  重写print的write函数.
 * @param  c	要写的字符
 */
size_t TWEN_GFX::write(uint8_t c) 
{
	if(c == '\n'){
		cursor_x = 0;  //x坐标清零
		cursor_y += textsize_y; //y坐标增加一个字体长度
	}else if(c != '\r'){
    	if (wrap && ((cursor_x + textsize_x) > _width)) //如果字体超过右边边框
		{ 
        	cursor_x = 0;     //x坐标清零
        	cursor_y += textsize_y; //y坐标增加一个字体高度
    	}
		show_char(cursor_x,cursor_y, c, textcolor, textbgcolor, textsize_y);
		cursor_x += textsize_x; //x坐标增加一个字体宽度
	}
	return 1;
}

/**
 * @brief  绘制线段.
 * @param  x0	起始x坐标.
 * @param  y0	起始y坐标.
 * @param  x1	终点x坐标.
 * @param  y1	终点y坐标.
 * @param  color	颜色值.
 */
void TWEN_GFX::draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		_swap_int16_t(x0, y0);
		_swap_int16_t(x1, y1);
	}

	if (x0 > x1) {
		_swap_int16_t(x0, x1);
		_swap_int16_t(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (; x0 <= x1; x0++) {
		if (steep) {
			draw_point(y0, x0, color);
		} else {
			draw_point(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

/**
 * @brief  画垂直线.
 * @param  x	起始x坐标.
 * @param  y	起始y坐标.
 * @param  h	线段高度.
 * @param  color	颜色值.
 */
void TWEN_GFX::draw_Vline(int16_t x, int16_t y, int16_t h,uint16_t color) 
{
    draw_line(x, y, x, y+h-1, color);
}

/**
 * @brief  画水平线.
 * @param  x	起始x坐标.
 * @param  y	起始y坐标.
 * @param  w	线段宽度.
 * @param  color	颜色值.
 */
void TWEN_GFX::draw_Hline(int16_t x, int16_t y, int16_t w,uint16_t color)
{
    draw_line(x, y, x + w - 1, y, color);
}

/**
 * @brief  在指定位置绘制矩形框.
 * @param  x0	起始x坐标.
 * @param  y0	起始y坐标.
 * @param  x1	终点x坐标.
 * @param  y2	终点y坐标.
 * @param  color	颜色值.
 */
void TWEN_GFX::draw_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
	draw_line(x0, y0, x1, y0,color);
	draw_line(x0, y0, x0, y1,color);
	draw_line(x0, y1, x1, y1,color);
	draw_line(x1, y0, x1, y1,color);
}

/**
 * @brief  在指定位置填充矩形.
 * @param  x0	起始x坐标.
 * @param  y0	起始y坐标.
 * @param  x1	终点x坐标.
 * @param  y2	终点y坐标.
 * @param  color	颜色值.
 */
void TWEN_GFX::fill_rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t color)
{
    for (int16_t i = x0; i < x1; i++) {
        draw_Vline(i, y0, abs(y1 - y0), color); //画垂直线
    }
}

/**
 * @brief  协助画圆工具函数.
 */
void TWEN_GFX::draw_circle_helper(int16_t x0, int16_t y0, int16_t r,uint8_t cornername, uint16_t color) 
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x4) {
            draw_point(x0 + x, y0 + y, color);
            draw_point(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            draw_point(x0 + x, y0 - y, color);
            draw_point(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            draw_point(x0 - y, y0 + x, color);
            draw_point(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            draw_point(x0 - y, y0 - x, color);
            draw_point(x0 - x, y0 - y, color);
        }
    }
}

/**
 * @brief  协助填充圆工具函数.
 */
void TWEN_GFX::fill_circle_helper(int16_t x0, int16_t y0, int16_t r,uint8_t corners, int16_t delta,uint16_t color) 
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    int16_t px = x;
    int16_t py = y;

    delta++; // Avoid some +1's in the loop

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (x < (y + 1)) {
            if (corners & 1)
                draw_Vline(x0 + x, y0 - y, 2 * y + delta, color);
            if (corners & 2)
                draw_Vline(x0 - x, y0 - y, 2 * y + delta, color);
        }
        if (y != py) {
            if (corners & 1)
                draw_Vline(x0 + py, y0 - px, 2 * px + delta, color);
            if (corners & 2)
                draw_Vline(x0 - py, y0 - px, 2 * px + delta, color);
            py = y;
        }
        px = x;
    }
}

/**
 * @brief  画圆.
 * @param  x0 	圆心x坐标.
 * @param  y0 	圆心y坐标.
 * @param  color 	颜色值.
 */
void TWEN_GFX::draw_circle(int16_t x0, int16_t y0, int16_t r,uint16_t color) 
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    draw_point(x0, y0 + r, color);
    draw_point(x0, y0 - r, color);
    draw_point(x0 + r, y0, color);
    draw_point(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) 
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        draw_point(x0 + x, y0 + y, color);
        draw_point(x0 - x, y0 + y, color);
        draw_point(x0 + x, y0 - y, color);
        draw_point(x0 - x, y0 - y, color);
        draw_point(x0 + y, y0 + x, color);
        draw_point(x0 - y, y0 + x, color);
        draw_point(x0 + y, y0 - x, color);
        draw_point(x0 - y, y0 - x, color);
    }
}

/**
 * @brief  填充圆.
 * @param  x0 	圆心x坐标.
 * @param  y0 	圆心y坐标.
 * @param  color 	颜色值.
 */
void TWEN_GFX::fill_circle(int16_t x0, int16_t y0, int16_t r,uint16_t color) 
{
  draw_Vline(x0, y0 - r, 2 * r + 1, color);
  fill_circle_helper(x0, y0, r, 3, 0, color);
}

/**
 * @brief  画圆角矩形.
 * @param  x0 	起点x坐标.
 * @param  y0 	起点y坐标.
 * @param  x1 	终点x坐标.
 * @param  y1 	终点y坐标.
 * @param  radius 	圆角半径.
 * @param  color 	颜色值.
 */
void TWEN_GFX::draw_round_rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t radius, uint16_t color)
{
    int16_t w = abs(x1-x0);
    int16_t h = abs(y1-y0);
    int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
    if (radius > max_radius)
        radius = max_radius;
    // smarter version
    draw_Hline(x0 + radius, y0, w - 2 * radius, color);         // Top
    draw_Hline(x0 + radius, y0 + h - 1, w - 2 * radius, color); // Bottom
    draw_Vline(x0, y0 + radius, h - 2 * radius, color);         // Left
    draw_Vline(x0 + w - 1, y0 + radius, h - 2 * radius, color); // Right
    // draw four corners
    draw_circle_helper(x0 + radius, y0 + radius, radius, 1, color);
    draw_circle_helper(x0 + w - radius - 1, y0 + radius, radius, 2, color);
    draw_circle_helper(x0 + w - radius - 1, y0 + h - radius - 1, radius, 4, color);
    draw_circle_helper(x0 + radius, y0 + h - radius - 1, radius, 8, color);
}

/**
 * @brief  填充圆角矩形.
 * @param  x0 	起点x坐标.
 * @param  y0 	起点y坐标.
 * @param  x1 	终点x坐标.
 * @param  y1 	终点y坐标.
 * @param  radius 	圆角半径.
 * @param  color 	颜色值.
 */
void TWEN_GFX::fill_round_rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t radius, uint16_t color)
{
    int16_t w = abs(x1-x0);
    int16_t h = abs(y1-y0);
    int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
    if (radius > max_radius)
        radius = max_radius;
    // smarter version
    fill_rectangle(x0 + radius, y0, x1 - radius, y1, color);

    // draw four corners
    fill_circle_helper(x0 + w - radius - 1, y0 + radius, radius, 1, h - 2 * radius - 1, color);
    fill_circle_helper(x0 + radius, y0 + radius, radius, 2, h - 2 * radius - 1, color);
}

/**
 * @brief  画三角形.
 * @param  x0 	端点1的x坐标.
 * @param  y0 	端点1的y坐标.
 * @param  x1 	端点2的x坐标.
 * @param  y1 	端点2的y坐标.
 * @param  x2 	端点3的x坐标.
 * @param  y2 	端点3的y坐标.
 * @param  color 	颜色值.
 */
void TWEN_GFX::draw_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color) 
{
  draw_line(x0, y0, x1, y1, color);
  draw_line(x1, y1, x2, y2, color);
  draw_line(x2, y2, x0, y0, color);
}

/**
 * @brief  填充三角形.
 * @param  x0 	端点1的x坐标.
 * @param  y0 	端点1的y坐标.
 * @param  x1 	端点2的x坐标.
 * @param  y1 	端点2的y坐标.
 * @param  x2 	端点3的x坐标.
 * @param  y2 	端点3的y坐标.
 * @param  color 	颜色值.
 */
void TWEN_GFX::fill_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color) 
{
    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        _swap_int16_t(y0, y1);
        _swap_int16_t(x0, x1);
    }
    if (y1 > y2) {
        _swap_int16_t(y2, y1);
        _swap_int16_t(x2, x1);
    }
    if (y0 > y1) {
        _swap_int16_t(y0, y1);
        _swap_int16_t(x0, x1);
    }

    if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if (x1 < a)
            a = x1;
        else if (x1 > b)
            b = x1;
        if (x2 < a)
            a = x2;
        else if (x2 > b)
            b = x2;
        draw_Hline(a, y0, b - a + 1, color);
        return;
    }

    int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
            dx12 = x2 - x1, dy12 = y2 - y1;
    int32_t sa = 0, sb = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if (y1 == y2)
        last = y1; // Include y1 scanline
    else
        last = y1 - 1; // Skip it

    for (y = y0; y <= last; y++) {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if (a > b)
            _swap_int16_t(a, b);
        draw_Hline(a, y, b - a + 1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = (int32_t)dx12 * (y - y1);
    sb = (int32_t)dx02 * (y - y0);
    for (; y <= y2; y++) {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if (a > b)
            _swap_int16_t(a, b);
        draw_Hline(a, y, b - a + 1, color);
    }
}

/**
 * @brief  在指定位置显示12*12字体汉字.
 * @param  lenth 	长度.
 * @param  hz 		汉字的指针.
 * @note
 * 		需要自己汉字取模，或者通过天问Block自动取模.
 * 	
 */
void TWEN_GFX::draw_hanzi_12(uint8_t lenth, const uint8_t *hz)
{
	uint8_t zz,i,j;
	int16_t zs;
	if(cursor_x < 0)
	{
		zs = (0-cursor_x) / 12;	
		for(zz = zs; zz < lenth; zz++)		//要显示的字体循环
		{
            if((wrap) && ((cursor_x + 12) > _width))
            {
                    cursor_x = 0;     //x坐标清零
                    cursor_y += 12;                
            }
			for(i = 0;i < 12;i++)
			{
				for(j = 0;j < 12;j++)
				{
					if((cursor_x + j -12) > _width)return;
					if((cursor_y + i -12) > _height)return;
					if (!((cursor_x + j) < 0)){
						if(!_bg_mode)		//无叠加
						{
							if(hz[zz*24 + i * 2 + j/8] & (0x80 >> (j % 8))){
                                draw_point(cursor_x + j,cursor_y + i,textcolor);
							}
							else{
                                draw_point(cursor_x + j,cursor_y + i,textbgcolor);
							} 
						}
						else
						{
							if(hz[zz*24 + i * 2 + j/8] & (0x80>>(j % 8))){
                                draw_point(cursor_x + j,cursor_y + i,textcolor);
							}
						}
					}
				}
			}
            cursor_x += 12;
		}
	}
	else
	{
		for(zz = 0; zz < lenth; zz++)		//要显示的字体循环
		{
			if((wrap) && ((cursor_x + 12) > _width))
            {
                    cursor_x = 0;     //x坐标清零
                    cursor_y += 12;                
            }
			for(i = 0;i < 12;i++)
			{
				for(j = 0;j < 12;j++)
				{
					if((cursor_x + j -12) > _width)return;                
					if((cursor_y + i -12) > _height)return;
					if(!_bg_mode)		//无叠加
					{
						if(hz[zz*24 + i * 2 + j/8] & (0x80 >> (j % 8))){
                            draw_point(cursor_x + j,cursor_y + i,textcolor);
						}
						else{
                            draw_point(cursor_x + j,cursor_y + i,textbgcolor);
						}
					}
					else
					{
						if(hz[zz*24 + i * 2 + j/8] & (0x80>>(j % 8))){
							draw_point(cursor_x + j,cursor_y + i,textcolor);					
						}
					}
				}
			}
            cursor_x+=12;
		}
	}
}

/**
 * @brief  在指定位置显示16*16字体汉字.
 * @param  lenth 	长度.
 * @param  hz 		汉字的指针.
 * @note
 * 		需要自己汉字取模，或者通过天问Block自动取模.
 */
void TWEN_GFX::draw_hanzi_16(uint8_t lenth, const uint8_t *hz)
{
	uint8_t zz,i,j;
	int16_t zs;
	if(cursor_x < 0)
	{
		zs = (0-cursor_x) / 16;	
		for(zz = zs; zz < lenth; zz++)		//要显示的字体循环
		{
            if((wrap) && ((cursor_x + 16) > _width))
            {
                    cursor_x = 0;     //x坐标清零
                    cursor_y += 16;                
            }
			for(i = 0;i < 16;i++)
			{
				for(j = 0;j < 16;j++)
				{
					if((j + cursor_x - 16) > _width)return;
					if((cursor_y + i -16) > _height)return;
					if (!((cursor_x + j) < 0)){
						if(!_bg_mode)		//无叠加
						{
							if(hz[zz*32+i * 2 + j/8] & (0x80 >> (j % 8))){
                                draw_point(cursor_x + j,cursor_y + i,textcolor);	
							}
							else{
                                draw_point(cursor_x + j,cursor_y + i,textbgcolor);	
							}
						}
						else
						{
							if(hz[zz*32 + i * 2 + j/8] & (0x80>>(j % 8))){
								draw_point(cursor_x + j,cursor_y + i,textcolor);							
							}
						}
					}
				}
			}
            cursor_x += 16;
		}
	}
	else
	{
		for(zz = 0; zz < lenth; zz++)		//要显示的字体循环
		{
            if((wrap) && ((cursor_x + 16) > _width))
            {
                    cursor_x = 0;     //x坐标清零
                    cursor_y += 16;                
            }
			for(i = 0;i < 16;i++)
			{
				for(j = 0;j < 16;j++)
				{
					if((j + cursor_x -16) > _width)return;
					if((cursor_y + i -16) > _height)return;

					if(!_bg_mode)		//无叠加
					{
						if(hz[zz*32+i * 2 + j/8] & (0x80 >> (j % 8))){
							draw_point(cursor_x + j,cursor_y + i,textcolor);	
						}
						else{
							draw_point(cursor_x + j,cursor_y + i,textbgcolor);
						}
					}
					else
					{
						if(hz[zz*32 + i * 2 + j/8] & (0x80>>(j % 8))){
							draw_point(cursor_x + j,cursor_y + i,textcolor);	
						}
					}
				}
			}
            cursor_x += 16;
		}
	}
}

/**
 * @brief  在指定位置显示24*24字体汉字.
 * @param  lenth 	长度.
 * @param  hz 		汉字的指针.
 * @note
 * 		需要自己汉字取模，或者通过天问Block自动取模.
 */
void TWEN_GFX::draw_hanzi_24(uint8_t lenth, const uint8_t *hz)
{
	uint8_t zz,i,j;
	int16_t zs;
	if(cursor_x < 0)
	{
		zs = (0-cursor_x) / 24;	
		for(zz = zs; zz < lenth; zz++)		//要显示的字体循环
		{
            if((wrap) && ((cursor_x + 24) > _width))
            {
                    cursor_x = 0;     //x坐标清零
                    cursor_y += 24;                
            }            
			for(i = 0;i < 24;i++)
			{
				for(j = 0;j < 24;j++)
				{
					if((j + cursor_x -24) > _width)return;
					if((cursor_y + i -24) > _height)return;
					if (!((cursor_x + j) < 0))
					{
						if(!_bg_mode)		//无叠加
						{
							if(hz[zz*72+i * 3 + j/8] & (0x80 >> (j % 8))){
                                draw_point(cursor_x + j,cursor_y + i,textcolor);
							}
							else {
                                draw_point(cursor_x + j,cursor_y + i,textbgcolor);
							}
						}
						else
						{
							if(hz[zz*72 + i * 3 + j/8] & (0x80>>(j % 8))){
								draw_point(cursor_x + j,cursor_y + i,textcolor);
							}
						}
					}
				}
			}
            cursor_x+=24;
		}
	}
	else
	{
		for(zz = 0; zz < lenth; zz++)		//要显示的字体循环
		{
            if((wrap) && ((cursor_x + 24) > _width))
            {
                    cursor_x = 0;     //x坐标清零
                    cursor_y += 24;                
            }  
			for(i = 0;i < 24;i++)
			{
				for(j = 0;j < 24;j++)
				{
					if((j + cursor_x -24) > _width)return;
					if((cursor_y + i -24) > _height)return;

					if(!_bg_mode)		//无叠加
					{
						if(hz[zz*72+i * 3 + j/8] & (0x80 >> (j % 8))){
							draw_point(cursor_x + j,cursor_y + i,textcolor);
						}
						else{
							draw_point(cursor_x + j,cursor_y + i,textbgcolor);
						}
					}
					else
					{
						if(hz[zz*72 + i * 3 + j/8] & (0x80>>(j % 8))){
							draw_point(cursor_x + j,cursor_y + i,textcolor);
						}
					}
				}
			}
            cursor_x+=24;
		}
	}
}

/**
 * @brief  在指定位置显示32*32字体汉字.
 * @param  lenth 	长度.
 * @param  hz 		汉字的指针.
 * @note
 * 		需要自己汉字取模，或者通过天问Block自动取模.		
 */
void TWEN_GFX::draw_hanzi_32(uint8_t lenth, const uint8_t *hz)
{
	uint8_t zz,i,j;
	int16_t zs;
	if(cursor_x < 0)
	{
		zs = (0-cursor_x) / 32;	
		for(zz = zs; zz < lenth; zz++)		//要显示的字体循环
		{
            if((wrap) && ((cursor_x + 32) > _width))
            {
                    cursor_x = 0;     //x坐标清零
                    cursor_y += 32;                
            }
			for(i = 0;i < 32;i++)
			{
				for(j = 0;j < 32;j++)
				{
					if((j + cursor_x -34) > _width)return;
					if((cursor_y + i -34) > _height)return;
					if (!((cursor_x + j) < 0)){
						if(!_bg_mode)		//无叠加
						{
							if (hz[zz * 128 + j * 4 + i/ 8] & (0x80 >> (i % 8))){
                                draw_point(cursor_x + j,cursor_y + i,textcolor);
							}
							else{ 
								draw_point(cursor_x + j,cursor_y + i,textbgcolor);
							}
						}
						else
						{
							if (hz[zz * 128 + j * 4 + i/ 8] & (0x80 >> (i % 8))){
								draw_point(cursor_x + j,cursor_y + i,textcolor);
							}
						}
					}
				}
			}
            cursor_x+=32;
		}
	}
	else
	{
		for(zz = 0; zz < lenth; zz++)		//要显示的字体循环
		{
            if((wrap) && ((cursor_x + 32) > _width))
            {
                    cursor_x = 0;     //x坐标清零
                    cursor_y += 32;                
            }
			for(i = 0;i < 32;i++)
			{
				for(j = 0;j < 32;j++)
				{
					if((j + cursor_x -32) > _width)return;
					if((cursor_y + i -32) > _height)return;

					if(!_bg_mode)		//无叠加
					{
						if (hz[zz * 128 + j * 4 + i/ 8] & (0x80 >> (i % 8))){
							draw_point(cursor_x + j,cursor_y + i,textcolor);
						}
						else{
							draw_point(cursor_x + j,cursor_y + i,textbgcolor);
						}
					}
					else
					{
						if (hz[zz * 128 + j * 4 + i/ 8] & (0x80 >> (i % 8))){
							draw_point(cursor_x + j,cursor_y + i,textcolor);
						}
					}
				}
			}
            cursor_x+=32;
		}
	}
}
/**
 * @brief 彩屏显示二维码.
 * @param x	二维码起始x坐标.
 * @param y	二维码起始y坐标.
 * @param version	版本(1 - 40)
 * 			1	-	21*21
 * 			2	-	25*25
 * 			3	-	29*29
 * 			4   -	33*33
 * 			5	-   37*37
 * 			6	- 	41*41
 * 			7   -   45*45
 * 			8	-   49*49
 * 			9   -   53*53
 * 			10	-	57*57
 * 			11	-	61*61
 * 			12	-	65*65
 * 			13  -   69*69
 * 			14	-	73*73
 * 			15	-	77*77
 * 			16	-	81*81
 * 			17	-	85*85
 * 			18	-	89*89
 * 			19	-	93*93
 * 			20	-	101*101
 * 			21	-	101*101
 * 			22	-	105*105
 * 			23	-	109*109
 * 			24	-	113*113
 * 			25	-	117*117
 * 			26	-	121*121
 * 			27	-	125*125
 * 			28	-	129*129
 * 			29	-	133*133
 * 			30	-	137*137
 * 			31	-	141*141
 * 			32	-	145*145
 * 			33	-	149*149
 * 			34	-	153*153
 * 			35	-	157*157
 * 			36	-	161*161
 * 			37	-	165*165
 * 			38	-	169*169
 * 			39	-	173*173
 * 			40	-	177*177
 * @param ecc	纠错等级(0 - 3)
 * @param data	信息指针
 * @note
 * 		该函数为freertos操作系统用。
 */ 
void TWEN_GFX::show_qrcode(uint8_t x, uint8_t y, uint8_t version, uint8_t ecc, const char *data)
{
  QRCode qrcode;
  uint8_t *qrcodeData;
  qrcodeData = (uint8 *)pvPortMalloc(qrcode_getBufferSize(version));
  qrcode_initText(&qrcode, qrcodeData, version, ecc, data);
  
  for (uint8_t i = 0; i < qrcode.size; i++) {
    for (uint8_t j = 0; j < qrcode.size; j++) {
      if(qrcode_getModule(&qrcode, j, i)){
        draw_point((i + x),(j + y),(0x0000));
      }
      else{
        draw_point((i + x),(j + y),(0xFFFF));
      }
    }
  }
  vPortFree(qrcodeData);
}

#endif