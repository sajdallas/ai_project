/**
 * @file st7735.h
 * @brief ST7735驱动的TFT彩屏。
 * @details 
 * @version 0.0.2
 * @date 2023-02-17
 * @author hhdd
 * @copyright Copyright (c) 2021 TWen51 Technology Co., Ltd.
 * 
 */
#ifndef _ST7735_H
#define _ST7735_H

#include "twen_gfx.h"

#ifndef ST7735S_RGB_GBR_ORDER
#define ST7735S_RGB_GBR_ORDER    	1
#endif

//彩屏引脚宏定义
#ifndef ST7735_SCK_PIN
#define ST7735_SCK_PIN              P2_5
#define ST7735_SCK_GPIO_PIN         gpio_pin_5
#endif

#ifndef ST7735_MOSI_PIN
#define ST7735_MOSI_PIN             P2_6
#define ST7735_MOSI_GPIO_PIN        gpio_pin_6
#endif

#ifndef ST7735_CS_PIN
#define ST7735_CS_PIN               P0_0
#define ST7735_CS_GPIO_PIN          gpio_pin_0
#endif

#ifndef ST7735_DC_PIN
#define ST7735_DC_PIN               P0_1
#define ST7735_DC_GPIO_PIN          gpio_pin_1
#endif

#ifndef ST7735_RST_PIN
#define ST7735_RST_PIN              P0_2
#define ST7735_RST_GPIO_PIN         gpio_pin_2
#endif

#define ST7735_CS_LOW()    			*_cs_reg = 0x00
#define ST7735_CS_HIGH()   			*_cs_reg = ST7735_CS_GPIO_PIN

#define ST7735_DC_LOW()    			*_dc_reg = 0x00
#define ST7735_DC_HIGH()   			*_dc_reg = ST7735_DC_GPIO_PIN

#define ST7735_RST_LOW()    		*_rst_reg = 0x00
#define ST7735_RST_HIGH()   		*_rst_reg = ST7735_RST_GPIO_PIN

#define ST7735_SCK_SAMPLE() 		*_sck_reg = ST7735_SCK_GPIO_PIN;
#define ST7735_SCK_UPDATE(d, m) 	*_t_reg = (d & m) ? (ST7735_MOSI_GPIO_PIN):0x00;

//颜色
#define TFT_LCD_WHITE         	 0xFFFF
#define TFT_LCD_BLACK         	 0x0000	  
#define TFT_LCD_BLUE             0x001F  
#define TFT_LCD_BRED             0XF81F
#define TFT_LCD_GBLUE            0X07FF
#define TFT_LCD_RED           	 0xF800
#define TFT_LCD_MAGENTA       	 0xF81F
#define TFT_LCD_GREEN         	 0x07E0
#define TFT_LCD_CYAN          	 0x7FFF
#define TFT_LCD_YELLOW        	 0xFFE0
#define TFT_LCD_BROWN            0XBC40 //棕色
#define TFT_LCD_BRRED            0XFC07 //棕红色
#define TFT_LCD_GRAY             0X8430 //灰色
#define TFT_LCD_DARKBLUE      	 0X01CF	//深蓝色
#define TFT_LCD_LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define TFT_LCD_GRAYBLUE       	 0X5458 //灰蓝色
#define TFT_LCD_LIGHTGREEN     	 0X841F //浅绿色
#define TFT_LCD_LGRAY            0XC618 //浅灰色(PANNEL),窗体背景色
#define TFT_LCD_LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define TFT_LCD_LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

#define USE_HORIZONTAL      	 0   //设置默认显示方向

//LCD重要参数集
typedef struct
{
	uint16_t width;		//LCD 宽度
	uint16_t height;	//LCD 高度
	uint16_t id;			//LCD ID
	uint8_t  dir;			//彩屏显示方向
	/* 保存真正显示区域的地址信息 */
	uint16_t x_start_width; 	//x方向起始地址到x起始边界的距离
	uint16_t x_end_width; 	//x方向结束地址到x起始边界的距离
	uint16_t y_start_width;	//y方向起始地址到y起始边界的距离
	uint16_t y_end_width; 	//y方向结束地址到y起始边界的距离
}_lcd_dev;

class SPITFT : public TWEN_GFX
{
public:
	SPITFT(uint16_t w, uint16_t h, int8_t blk_pin=-1);
	void init();
	void start_write();	//开始写入
	void end_write();	//结束写入
	void backlight(uint8_t bright);	//控制背光
	void set_direction(uint8_t direction);	//设置显示方向
	void set_windows(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1); //设置显示窗口
	void clear(uint16_t color);	//清屏
	void draw_point(int16_t x, int16_t y, uint16_t color); //画点
	void show_picture(uint16_t x,uint16_t y,uint16_t width,uint16_t height,const uint8_t pic[]);	//显示图片

	void fill_rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t color);//填充矩形
private:
	uint8_t _blk_pin;
	volatile uint32_t * _cs_reg = NULL;
	volatile uint32_t * _rst_reg = NULL;
	volatile uint32_t * _dc_reg = NULL;   
	volatile uint32_t * _sck_reg = NULL;
	volatile uint32_t * _t_reg = NULL;  
	
	_lcd_dev lcddev;	//管理LCD重要参数

	void write_8bit(uint8_t dat);	//写入8位数据
	void write_16bit(uint16_t Data);  //写入16位数据
	void write_cmd(uint8_t Reg);	//写命令
};

/**
 * @brief  引脚配置
 * @param  w	彩屏宽度
 * @param  h	彩屏高度
 * @param  blk_pin	背光引脚(-1代表不需要背光引脚)
 */
SPITFT::SPITFT(uint16_t w, uint16_t h, int8_t blk_pin):TWEN_GFX(w, h)
{
	lcddev.width = w;
	lcddev.height = h;
	if (blk_pin != -1)
	{
		_blk_pin = blk_pin;
	}
}

/**
 * @brief  LCD开始写入数据
 */
void SPITFT::start_write()
{
	ST7735_CS_LOW();
}

/**
 * @brief  LCD结束写入数据
 */
void SPITFT::end_write()
{
	ST7735_CS_HIGH();
}

/**
 * @brief  LCD写入8位数据.
 * @param  dat	写入的字节数据
 */
void SPITFT::write_8bit(uint8_t dat)
{
    ST7735_SCK_UPDATE(dat, 0x80)
    ST7735_SCK_SAMPLE();

    ST7735_SCK_UPDATE(dat, 0x40)
    ST7735_SCK_SAMPLE();

    ST7735_SCK_UPDATE(dat, 0x20)
    ST7735_SCK_SAMPLE();

    ST7735_SCK_UPDATE(dat, 0x10)
    ST7735_SCK_SAMPLE();

    ST7735_SCK_UPDATE(dat, 0x08)
    ST7735_SCK_SAMPLE();

    ST7735_SCK_UPDATE(dat, 0x04)
    ST7735_SCK_SAMPLE();

    ST7735_SCK_UPDATE(dat, 0x02)
    ST7735_SCK_SAMPLE();

    ST7735_SCK_UPDATE(dat, 0x01)
    ST7735_SCK_SAMPLE();
}

/**
 * @brief  LCD写入16位数据.
 * @param  dat	写入的数据
 */
void SPITFT::write_16bit(uint16_t Data)
{
    write_8bit(Data>>8);
	write_8bit(Data);
}

/**
 * @brief  LCD写入命令.
 * @param  Reg	写入的命令
 */
void SPITFT::write_cmd(uint8_t Reg)	 
{	
	ST7735_DC_LOW();
	write_8bit(Reg);
    ST7735_DC_HIGH();
}

/**
 * @brief  LCD设置窗口.
 * @param  x0	窗口起始x坐标
 * @param  y0	窗口起始y坐标
 * @param  x1	窗口起始x坐标
 * @param  y1	窗口起始y坐标
 */
void SPITFT::set_windows(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	if(lcddev.dir == 0){
		write_cmd(0x2A);
		write_16bit(x0+lcddev.x_start_width);
		write_16bit(x1+lcddev.x_start_width);
		write_cmd(0x2B);
		write_16bit(y0+lcddev.y_start_width);
		write_16bit(y1+lcddev.y_start_width);	
	}else if(lcddev.dir == 1){
		write_cmd(0x2A);
		write_16bit(x0+lcddev.y_start_width);
		write_16bit(x1+lcddev.y_start_width);
		write_cmd(0x2B);
		write_16bit(y0+lcddev.x_end_width);
		write_16bit(y1+lcddev.x_end_width);	
	}else if(lcddev.dir == 2){
		write_cmd(0x2A);
		write_16bit(x0+lcddev.x_end_width);
		write_16bit(x1+lcddev.x_end_width);
		write_cmd(0x2B);
		write_16bit(y0+lcddev.y_end_width);
		write_16bit(y1+lcddev.y_end_width);		
	}else if(lcddev.dir == 3){
		write_cmd(0x2A);
		write_16bit(x0+lcddev.y_end_width);
		write_16bit(x1+lcddev.y_end_width);
		write_cmd(0x2B);
		write_16bit(y0+lcddev.x_start_width);
		write_16bit(y1+lcddev.x_start_width);	
	}else{
		return;
	}	
	write_cmd(0x2C);							
}

/**
 * @brief  初始化.
 */
void SPITFT::init()
{
	pinMode(ST7735_CS_PIN,output);
	pinMode(ST7735_DC_PIN,output);
	pinMode(ST7735_RST_PIN,output);
	pinMode(ST7735_SCK_PIN,output);
	pinMode(ST7735_MOSI_PIN,output);

 #if defined(TW_ASR_PRO)
	set_pin_to_gpio_mode(ST7735_CS_PIN);
	set_pin_to_gpio_mode(ST7735_DC_PIN);
	set_pin_to_gpio_mode(ST7735_RST_PIN);
	set_pin_to_gpio_mode(ST7735_SCK_PIN);
	set_pin_to_gpio_mode(ST7735_MOSI_PIN);
    if(ST7735_CS_PIN<16)
	{
		_cs_reg = (uint32_t*)(HAL_PA_BASE + 0X1000 * (ST7735_CS_PIN/8)) + ST7735_CS_GPIO_PIN;
	}
	else if(ST7735_CS_PIN<24)
	{
		_cs_reg = (uint32_t*)(HAL_PC_BASE ) + ST7735_CS_GPIO_PIN;
	}else
	{
		_cs_reg = (uint32_t*)(HAL_PD_BASE ) + ST7735_CS_GPIO_PIN;

	}

    if(ST7735_DC_PIN<16)
	{
		_dc_reg = (uint32_t*)(HAL_PA_BASE + 0X1000 * (ST7735_DC_PIN/8)) + ST7735_DC_GPIO_PIN;
	}
	else if(ST7735_DC_PIN<24)
	{
		_dc_reg = (uint32_t*)(HAL_PC_BASE ) + ST7735_DC_GPIO_PIN;
	}else
	{
		_dc_reg = (uint32_t*)(HAL_PD_BASE ) + ST7735_DC_GPIO_PIN;

	}	

    if(ST7735_RST_PIN<16)
	{
		_rst_reg = (uint32_t*)(HAL_PA_BASE + 0X1000 * (ST7735_RST_PIN/8)) + ST7735_RST_GPIO_PIN;
	}
	else if(ST7735_RST_PIN<24)
	{
		_rst_reg = (uint32_t*)(HAL_PC_BASE ) + ST7735_RST_GPIO_PIN;
	}else
	{
		_rst_reg = (uint32_t*)(HAL_PD_BASE ) + ST7735_RST_GPIO_PIN;

	}	

    if(ST7735_SCK_PIN<16)
	{
		_sck_reg = (uint32_t*)(HAL_PA_BASE + 0X1000 * (ST7735_SCK_PIN/8)) + ST7735_SCK_GPIO_PIN;
	}
	else if(ST7735_SCK_PIN<24)
	{
		_sck_reg = (uint32_t*)(HAL_PC_BASE )+ST7735_SCK_GPIO_PIN ;
	}else
	{
		_sck_reg = (uint32_t*)(HAL_PD_BASE )+ST7735_SCK_GPIO_PIN ;

	}	
    if(ST7735_MOSI_PIN<16)
	{
		_t_reg = (uint32_t*)(HAL_PA_BASE + 0X1000 * (ST7735_MOSI_PIN/8)) + (ST7735_SCK_GPIO_PIN|ST7735_MOSI_GPIO_PIN);
	}
	else if(ST7735_MOSI_PIN<24)
	{
		_t_reg = (uint32_t*)(HAL_PC_BASE )+ (ST7735_SCK_GPIO_PIN|ST7735_MOSI_GPIO_PIN); ;
	}else
	{
		_t_reg = (uint32_t*)(HAL_PD_BASE )+ (ST7735_SCK_GPIO_PIN|ST7735_MOSI_GPIO_PIN); ;

	}	
#else
	_cs_reg = (uint32_t*)(HAL_GPIO0_BASE + 0X1000 * (ST7735_CS_PIN/8)) + ST7735_CS_GPIO_PIN;
	_dc_reg = (uint32_t*)(HAL_GPIO0_BASE + 0X1000 * (ST7735_DC_PIN/8)) + ST7735_DC_GPIO_PIN;
	_rst_reg = (uint32_t*)(HAL_GPIO0_BASE + 0X1000 * (ST7735_RST_PIN/8)) + ST7735_RST_GPIO_PIN;
    _sck_reg = (uint32_t*)(HAL_GPIO0_BASE + 0X1000 * (ST7735_SCK_PIN/8)) + ST7735_SCK_GPIO_PIN;
    _t_reg = (uint32_t*)(HAL_GPIO0_BASE + 0X1000 * (ST7735_MOSI_PIN/8)) + (ST7735_SCK_GPIO_PIN|ST7735_MOSI_GPIO_PIN); 

    if((ST7735_CS_PIN<=8) && (ST7735_CS_PIN >=5))
    {
        setPinFun(ST7735_CS_PIN,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(ST7735_CS_PIN,FIRST_FUNCTION);
    }
    if((ST7735_DC_PIN<=8) && (ST7735_DC_PIN >=5))
    {
        setPinFun(ST7735_DC_PIN,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(ST7735_DC_PIN,FIRST_FUNCTION);
    }
    if((ST7735_RST_PIN<=8) && (ST7735_RST_PIN >=5))
    {
        setPinFun(ST7735_RST_PIN,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(ST7735_RST_PIN,FIRST_FUNCTION);
    }
    if((ST7735_SCK_PIN<=8) && (ST7735_SCK_PIN >=5))
    {
        setPinFun(ST7735_SCK_PIN,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(ST7735_SCK_PIN,FIRST_FUNCTION);
    }
    if((ST7735_MOSI_PIN<=8) && (ST7735_MOSI_PIN >=5))
    {
        setPinFun(ST7735_MOSI_PIN,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(ST7735_MOSI_PIN,FIRST_FUNCTION);
    }
#endif
	if((lcddev.width == 80)&&(lcddev.height == 160)){  //132*162
		lcddev.x_start_width = 26;
		lcddev.x_end_width = 26;
		lcddev.y_start_width = 1;
		lcddev.y_end_width = 1;		
	}else if((lcddev.width == 128)&&(lcddev.height == 128)){  //128*160
		lcddev.x_start_width = 0;
		lcddev.x_end_width = 0;
		lcddev.y_start_width = 0;
		lcddev.y_end_width = 32;
	}else if((lcddev.width == 128)&&(lcddev.height == 160)){  //128*160
		lcddev.x_start_width = 0;
		lcddev.x_end_width = 0;
		lcddev.y_start_width = 0;
		lcddev.y_end_width = 0;
	}else{
		//Add your own  screen 
	}	

    ST7735_RST_HIGH();
    delay(50);
	ST7735_RST_LOW();
	delay(100);
	ST7735_RST_HIGH();
	delay(50);

	start_write();
    write_cmd(0x11); //Exit Sleep
    delay(50);	
    write_cmd(0xB1);
    write_8bit(0x02);
    write_8bit(0x35);
    write_8bit(0x36);

    write_cmd(0xB2);
    write_8bit(0x02);
    write_8bit(0x35);
    write_8bit(0x36);

    write_cmd(0xB3);
    write_8bit(0x02);
    write_8bit(0x35);
    write_8bit(0x36);
    write_8bit(0x02);
    write_8bit(0x35);
    write_8bit(0x36);
		
    //------------------------------------End ST7735S Frame Rate-----------------------------------------//
    write_cmd(0xB4); //Dot inversion
    write_8bit(0x03); //03

    write_cmd(0xC0);
    write_8bit(0xa2);
    write_8bit(0x02);
    write_8bit(0x84);

    write_cmd(0xC1);
    write_8bit(0XC5);

    write_cmd(0xC2);
    write_8bit(0x0D);
    write_8bit(0x00);

    write_cmd(0xC3);
    write_8bit(0x8D);
    write_8bit(0x2A);

    write_cmd(0xC4);
    write_8bit(0x8D);
    write_8bit(0xEE);
    //---------------------------------End ST7735S Power Sequence-------------------------------------//
    write_cmd(0xC5); //VCOM
    write_8bit(0x03);
	
	write_cmd(0x3a); //Set Color Format
	write_8bit(0x05);
    //------------------------------------ST7735S Gamma Sequence-----------------------------------------//
    write_cmd(0xE0);
    write_8bit(0x12);
    write_8bit(0x1C);
    write_8bit(0x10);
    write_8bit(0x18);
    write_8bit(0x33);
    write_8bit(0x2c);
    write_8bit(0x25);
    write_8bit(0x28);
    write_8bit(0x28);
    write_8bit(0x27);
    write_8bit(0x2f);
    write_8bit(0x3C);
    write_8bit(0x00);
    write_8bit(0x03);
    write_8bit(0x03);
    write_8bit(0x10);

    write_cmd(0xE1);
    write_8bit(0x12);
    write_8bit(0x1c);
    write_8bit(0x10);
    write_8bit(0x18);
    write_8bit(0x2d);
    write_8bit(0x28);
    write_8bit(0x23);
    write_8bit(0x28);
    write_8bit(0x28);
    write_8bit(0x26);
    write_8bit(0x2f);
    write_8bit(0x3B);
    write_8bit(0x00);
    write_8bit(0x03);
    write_8bit(0x03);
    write_8bit(0x10);

	// write_cmd(0x30);             
	// write_8bit(0x00);             
	// write_8bit(0x00);             
	// write_8bit(0x00);             
	// write_8bit(0x80);

	if((lcddev.width==80)&&(lcddev.height==160))
	{
		write_cmd(0x21); //Display Inversion On
	}	 
	write_cmd(0x12);
	write_cmd(0x29); // Display On
	set_direction(USE_HORIZONTAL);
}

/**
 * @brief  背光控制.
 * @param  bright	0:关闭背光;其他值:打开背光.
 */
void SPITFT::backlight(uint8_t bright)
{
    pinMode(_blk_pin,output);
    if((_blk_pin<=8) && (_blk_pin >=5))
    {
        setPinFun(_blk_pin,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(_blk_pin,FIRST_FUNCTION);
    }
	if(bright == 0)
	{
		digitalWrite(_blk_pin,0);
	}else{
		digitalWrite(_blk_pin,1);
	}
}

/**
 * @brief  设置显示方向.
 * @param  direction	取值为0-3.
 * 		      0 - 0度
 * 			  1 - 90度
 * 			  2 - 180度
 * 			  3 - 270度.
 */
void SPITFT::set_direction(uint8_t direction)
{
	lcddev.dir = direction;
	start_write();
	switch (lcddev.dir) {
		case 0:
			lcddev.width = lcddev.width;
			lcddev.height = lcddev.height;
			/* BIT7:MY; BIT6:MX; BIT5:MV(行列交换); BIT4:ML; BIT3:0,RGB,1,BGR; BIT2:MH */
			write_cmd(0x36);
			write_8bit((0 << 7)|(0 << 6)|(0 << 5)|(0 << 4)|(ST7735S_RGB_GBR_ORDER <<3));			
			break;
		case 1:
			lcddev.width = lcddev.height;
			lcddev.height = lcddev.width;
			write_cmd(0x36);
			write_8bit((0 << 7)|(1<<6)|(1 << 5)|(0 << 4)|(ST7735S_RGB_GBR_ORDER << 3));
			break;
		case 2:
			lcddev.width = lcddev.width;
			lcddev.height = lcddev.height;
			write_cmd(0x36);
			write_8bit((1 << 7)|(1 << 6)|(0 << 5)|(0 << 4)|(ST7735S_RGB_GBR_ORDER << 3));
			break;
		case 3:
			lcddev.width = lcddev.height;
			lcddev.height = lcddev.width;
			write_cmd(0x36);
			write_8bit((1 << 7)|(0 << 6)|(1 << 5)|(0 << 4)|(ST7735S_RGB_GBR_ORDER << 3));
			break;
		default:
			break;
	}
	TWEN_GFX::set_direction(lcddev.dir);
	end_write();
}


/**
 * @brief  LCD清屏.
 * @param  color	清成的颜色.
 */
void SPITFT::clear(uint16_t color)
{
	uint16_t i, j;
	start_write();
	set_windows(0, 0, lcddev.width - 1, lcddev.height - 1);
	for (i = 0; i < lcddev.width; i++)
	{
		for (j = 0; j < lcddev.height; j++)
		{
		    write_8bit(color>>8);
	        write_8bit(color);
		}
	}
	end_write();
}

/**
 * @brief  画点.
 * @param  x	点的x坐标.
 * @param  y	点的y坐标.
 * @param  color	颜色.
 */
void SPITFT::draw_point(int16_t x, int16_t y, uint16_t color)
{
	start_write();
	set_windows(x, y, x, y);//设置光标位置 
	write_8bit(color>>8);
	write_8bit(color);
	end_write();
}

/**
 * @brief  填充矩形.
 * @param  x0	矩形起点的x坐标.
 * @param  y0	矩形起点的y坐标.
 * @param  x1	矩形终点的x坐标.
 * @param  y1	矩形终点的y坐标.
 * @param  color	颜色值.
 */
void SPITFT::fill_rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t color)
{
	uint16 i, j;
	uint16 width = x1 - x0 + 1; 		//得到填充的宽度
	uint16 height = y1 - y0 + 1;		//高度
	start_write();
	set_windows(x0, y0, x1, y1);//设置显示窗口
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			write_8bit(color>>8);
			write_8bit(color); 
		}
	}
	set_windows(0, 0, lcddev.width - 1, lcddev.height - 1);//恢复窗口设置为全屏
	end_write();
}

/**
 * @brief  显示图片.
 * @param  x	图片起点的x坐标.
 * @param  y	图片起点的y坐标.
 * @param  width	图片宽度.
 * @param  height	图片高度.
 * @param  pic[]	图片数组.
 */
void SPITFT::show_picture(uint16_t x,uint16_t y,uint16_t width,uint16_t height,const uint8_t pic[])
{
	uint16_t i,j;
	uint32_t k=0;
	start_write();
	set_windows(x,y,x+width-1,y+height-1);
	for(i=0;i<width;i++)
	{
		for(j=0;j<height;j++)
		{
			write_8bit(pic[k*2+1]);
			write_8bit(pic[k*2]);
			k++;
		}
	}	
	end_write();		
}

#endif  //asr_st7735.h