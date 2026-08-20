#ifndef _GC9A01A_H
#define _GC9A01A_H

#include "asr.h"
#include "twen_gfx.h"

#ifndef GC9A01A_RGB_GBR_ORDER
#define GC9A01A_RGB_GBR_ORDER    1
#endif

//彩屏引脚宏定义
#ifndef GC9A01A_SCK_PIN
#define GC9A01A_SCK_PIN             P2_5
#define GC9A01A_SCK_GPIO_PIN        gpio_pin_5
#endif

#ifndef GC9A01A_SDA_PIN
#define GC9A01A_SDA_PIN             P2_6
#define GC9A01A_SDA_GPIO_PIN        gpio_pin_6
#endif

#ifndef GC9A01A_CS_PIN
#define GC9A01A_CS_PIN               P0_0
#define GC9A01A_CS_GPIO_PIN          gpio_pin_0
#endif

#ifndef GC9A01A_A0_PIN
#define GC9A01A_A0_PIN               P0_1
#define GC9A01A_A0_GPIO_PIN          gpio_pin_1
#endif

#ifndef GC9A01A_RES_PIN
#define GC9A01A_RES_PIN              P0_2
#define GC9A01A_RES_GPIO_PIN         gpio_pin_2
#endif

#define GC9A01A_CS_LOW()    		*_cs_reg = 0x00
#define GC9A01A_CS_HIGH()   		*_cs_reg = GC9A01A_CS_GPIO_PIN

#define GC9A01A_A0_LOW()    		*_dc_reg = 0x00
#define GC9A01A_A0_HIGH()   		*_dc_reg = GC9A01A_A0_GPIO_PIN

#define GC9A01A_RES_LOW()    		*_rst_reg = 0x00
#define GC9A01A_RES_HIGH()   		*_rst_reg = GC9A01A_RES_GPIO_PIN

#define GC9A01A_SCK_SAMPLE() 		*_sck_reg = GC9A01A_SCK_GPIO_PIN;
#define GC9A01A_SCK_UPDATE(d, m) 	*_t_reg = (d & m) ? (GC9A01A_SDA_GPIO_PIN):0x00;

//颜色
#define GC9A01A_LCD_WHITE         	 0xFFFF
#define GC9A01A_LCD_BLACK         	 0x0000	  
#define GC9A01A_LCD_BLUE             0x001F  
#define GC9A01A_LCD_BRED             0XF81F
#define GC9A01A_LCD_GBLUE            0X07FF
#define GC9A01A_LCD_RED           	 0xF800
#define GC9A01A_LCD_MAGENTA       	 0xF81F
#define GC9A01A_LCD_GREEN         	 0x07E0
#define GC9A01A_LCD_CYAN          	 0x7FFF
#define GC9A01A_LCD_YELLOW        	 0xFFE0
#define GC9A01A_LCD_BROWN            0XBC40 //棕色
#define GC9A01A_LCD_BRRED            0XFC07 //棕红色
#define GC9A01A_LCD_GRAY             0X8430 //灰色
#define GC9A01A_LCD_DARKBLUE      	 0X01CF	//深蓝色
#define GC9A01A_LCD_LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GC9A01A_LCD_GRAYBLUE       	 0X5458 //灰蓝色
#define GC9A01A_LCD_LIGHTGREEN     	 0X841F //浅绿色
#define GC9A01A_LCD_LGRAY            0XC618 //浅灰色(PANNEL),窗体背景色
#define GC9A01A_LCD_LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define GC9A01A_LCD_LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

#define GC9A01A_USE_HORIZONTAL       0   //设置默认显示方向

//LCD重要参数集
typedef struct
{
	uint16_t width;		//LCD 宽度
	uint16_t height;	//LCD 高度
	uint16_t id;			//LCD ID
	uint8_t  dir;			//彩屏显示方向
	/* 保存真正显示区域的地址信息 */
	// uint16_t x_start_width; 	//x方向起始地址到x起始边界的距离
	// uint16_t x_end_width; 	//x方向结束地址到x起始边界的距离
	// uint16_t y_start_width;	//y方向起始地址到y起始边界的距离
	// uint16_t y_end_width; 	//y方向结束地址到y起始边界的距离
}_gc9a01a_dev;

class GC9A01A : public TWEN_GFX
{
public:
	GC9A01A(uint16_t w, uint16_t h, int8_t blk_pin=-1);
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
	
	_gc9a01a_dev lcddev;	//管理LCD重要参数

	void write_8bit(uint8_t dat);	//写入8位数据
	void write_16bit(uint16_t Data);  //写入16位数据
	void write_cmd(uint8_t Reg);	//写命令
};

/**
 * @brief 屏幕长宽设置
 * @param w     宽度
 * @param h     高度
 * @param blk_pin   背光
 */
GC9A01A::GC9A01A(uint16_t w, uint16_t h, int8_t blk_pin):TWEN_GFX(w, h)
{
	lcddev.width = w;
	lcddev.height = h;
	if (blk_pin != -1)
	{
		_blk_pin = blk_pin;
	}
}

/**
 * @brief LCD开始写入数据
 */
void GC9A01A::start_write()
{
	GC9A01A_CS_LOW();
}

/**
 * @brief LCD结束数据写入
 */
void GC9A01A::end_write()
{
	GC9A01A_CS_HIGH();
}

/**
 * @brief LCD写入8位数据
 * @param dat       写入的字节数据
 */
void GC9A01A::write_8bit(uint8_t dat)
{
    GC9A01A_SCK_UPDATE(dat, 0x80)
    GC9A01A_SCK_SAMPLE();

    GC9A01A_SCK_UPDATE(dat, 0x40)
    GC9A01A_SCK_SAMPLE();

    GC9A01A_SCK_UPDATE(dat, 0x20)
    GC9A01A_SCK_SAMPLE();

    GC9A01A_SCK_UPDATE(dat, 0x10)
    GC9A01A_SCK_SAMPLE();

    GC9A01A_SCK_UPDATE(dat, 0x08)
    GC9A01A_SCK_SAMPLE();

    GC9A01A_SCK_UPDATE(dat, 0x04)
    GC9A01A_SCK_SAMPLE();

    GC9A01A_SCK_UPDATE(dat, 0x02)
    GC9A01A_SCK_SAMPLE();

    GC9A01A_SCK_UPDATE(dat, 0x01)
    GC9A01A_SCK_SAMPLE();
}

/**
 * @brief LCD写入16位数据
 * @param Data       写入的数据
 */
void GC9A01A::write_16bit(uint16_t Data)
{
    write_8bit(Data>>8);
	write_8bit(Data);
}

/**
 * @brief LCD写入命令
 * @param Reg       写入的命令
 */
void GC9A01A::write_cmd(uint8_t Reg)	 
{	
	GC9A01A_A0_LOW();
	write_8bit(Reg);
    GC9A01A_A0_HIGH();
}

/**
 * @brief LCD设置窗口
 * @param x0       窗口起始坐标的x值
 * @param y0       窗口起始坐标的y值
 * @param x1       窗口结束坐标的x值
 * @param y1       窗口结束坐标的y值
 */
void GC9A01A::set_windows(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	if(lcddev.dir == 0){
		write_cmd(0x2A);
		write_16bit(x0);
		write_16bit(x1); 
		write_cmd(0x2B);
		write_16bit(y0);
		write_16bit(y1);	
	}else if(lcddev.dir == 1){
		write_cmd(0x2A);
		write_16bit(x0);
		write_16bit(x1);
		write_cmd(0x2B);
		write_16bit(y0);
		write_16bit(y1);	
	}else if(lcddev.dir == 2){
		write_cmd(0x2A);
		write_16bit(x0);
		write_16bit(x1);
		write_cmd(0x2B);
		write_16bit(y0);
		write_16bit(y1);		
	}else if(lcddev.dir == 3){
		write_cmd(0x2A);
		write_16bit(x0);
		write_16bit(x1);
		write_cmd(0x2B);
		write_16bit(y0);
		write_16bit(y1); 	
	}else{
		return;
	}	
	write_cmd(0x2C);							
}

/**
 * @brief 初始化
 */
void GC9A01A::init()
{
	pinMode(GC9A01A_CS_PIN,output);
	pinMode(GC9A01A_A0_PIN,output);
	pinMode(GC9A01A_RES_PIN,output);
	pinMode(GC9A01A_SCK_PIN,output);
	pinMode(GC9A01A_SDA_PIN,output);

 #if defined(TW_ASR_PRO)
	set_pin_to_gpio_mode(GC9A01A_CS_PIN);
	set_pin_to_gpio_mode(GC9A01A_A0_PIN);
	set_pin_to_gpio_mode(GC9A01A_RES_PIN);
	set_pin_to_gpio_mode(GC9A01A_SCK_PIN);
	set_pin_to_gpio_mode(GC9A01A_SDA_PIN);
    if(GC9A01A_CS_PIN<16)
	{
		_cs_reg = (uint32_t*)(HAL_PA_BASE + 0X1000 * (GC9A01A_CS_PIN/8)) + GC9A01A_CS_GPIO_PIN;
	}
	else if(GC9A01A_CS_PIN<24)
	{
		_cs_reg = (uint32_t*)(HAL_PC_BASE ) + GC9A01A_CS_GPIO_PIN;
	}else
	{
		_cs_reg = (uint32_t*)(HAL_PD_BASE ) + GC9A01A_CS_GPIO_PIN;

	}

    if(GC9A01A_A0_PIN<16)
	{
		_dc_reg = (uint32_t*)(HAL_PA_BASE + 0X1000 * (GC9A01A_A0_PIN/8)) + GC9A01A_A0_GPIO_PIN;
	}
	else if(GC9A01A_A0_PIN<24)
	{
		_dc_reg = (uint32_t*)(HAL_PC_BASE ) + GC9A01A_A0_GPIO_PIN;
	}else
	{
		_dc_reg = (uint32_t*)(HAL_PD_BASE ) + GC9A01A_A0_GPIO_PIN;

	}	

    if(GC9A01A_RES_PIN<16)
	{
		_rst_reg = (uint32_t*)(HAL_PA_BASE + 0X1000 * (GC9A01A_RES_PIN/8)) + GC9A01A_RES_GPIO_PIN;
	}
	else if(GC9A01A_RES_PIN<24)
	{
		_rst_reg = (uint32_t*)(HAL_PC_BASE ) + GC9A01A_RES_GPIO_PIN;
	}else
	{
		_rst_reg = (uint32_t*)(HAL_PD_BASE ) + GC9A01A_RES_GPIO_PIN;

	}	

    if(GC9A01A_SCK_PIN<16)
	{
		_sck_reg = (uint32_t*)(HAL_PA_BASE + 0X1000 * (GC9A01A_SCK_PIN/8)) + GC9A01A_SCK_GPIO_PIN;
	}
	else if(GC9A01A_SCK_PIN<24)
	{
		_sck_reg = (uint32_t*)(HAL_PC_BASE )+GC9A01A_SCK_GPIO_PIN ;
	}else
	{
		_sck_reg = (uint32_t*)(HAL_PD_BASE )+GC9A01A_SCK_GPIO_PIN ;

	}	
    if(GC9A01A_SDA_PIN<16)
	{
		_t_reg = (uint32_t*)(HAL_PA_BASE + 0X1000 * (GC9A01A_SDA_PIN/8)) + (GC9A01A_SCK_GPIO_PIN|GC9A01A_SDA_GPIO_PIN);
	}
	else if(GC9A01A_CS_PIN<24)
	{
		_t_reg = (uint32_t*)(HAL_PC_BASE )+ (GC9A01A_SCK_GPIO_PIN|GC9A01A_SDA_GPIO_PIN); ;
	}else
	{
		_t_reg = (uint32_t*)(HAL_PD_BASE )+ (GC9A01A_SCK_GPIO_PIN|GC9A01A_SDA_GPIO_PIN); ;

	}	
#else
	_cs_reg = (uint32_t*)(HAL_GPIO0_BASE + 0X1000 * (GC9A01A_CS_PIN/8)) + GC9A01A_CS_GPIO_PIN;
	_dc_reg = (uint32_t*)(HAL_GPIO0_BASE + 0X1000 * (GC9A01A_A0_PIN/8)) + GC9A01A_A0_GPIO_PIN;
	_rst_reg = (uint32_t*)(HAL_GPIO0_BASE + 0X1000 * (GC9A01A_RES_PIN/8)) + GC9A01A_RES_GPIO_PIN;
    _sck_reg = (uint32_t*)(HAL_GPIO0_BASE + 0X1000 * (GC9A01A_SCK_PIN/8)) + GC9A01A_SCK_GPIO_PIN;
    _t_reg = (uint32_t*)(HAL_GPIO0_BASE + 0X1000 * (GC9A01A_SDA_PIN/8)) + (GC9A01A_SCK_GPIO_PIN|GC9A01A_SDA_GPIO_PIN); 

    if((GC9A01A_CS_PIN<=8) && (GC9A01A_CS_PIN >=5))
    {
        setPinFun(GC9A01A_CS_PIN,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(GC9A01A_CS_PIN,FIRST_FUNCTION);
    }
    if((GC9A01A_A0_PIN<=8) && (GC9A01A_A0_PIN >=5))
    {
        setPinFun(GC9A01A_A0_PIN,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(GC9A01A_A0_PIN,FIRST_FUNCTION);
    }
    if((GC9A01A_RES_PIN<=8) && (GC9A01A_RES_PIN >=5))
    {
        setPinFun(GC9A01A_RES_PIN,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(GC9A01A_RES_PIN,FIRST_FUNCTION);
    }
    if((GC9A01A_SCK_PIN<=8) && (GC9A01A_SCK_PIN >=5))
    {
        setPinFun(GC9A01A_SCK_PIN,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(GC9A01A_SCK_PIN,FIRST_FUNCTION);
    }
    if((GC9A01A_SDA_PIN<=8) && (GC9A01A_SDA_PIN >=5))
    {
        setPinFun(GC9A01A_SDA_PIN,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(GC9A01A_SDA_PIN,FIRST_FUNCTION);
    }
#endif
    GC9A01A_RES_HIGH();
    delay(50);
	GC9A01A_RES_LOW();
	delay(100);
	GC9A01A_RES_HIGH();
	delay(50);

	start_write();
    write_cmd(0xEF);
	write_cmd(0xEB);
	write_8bit(0x14); 
	
    write_cmd(0xFE);			 
	write_cmd(0xEF); 

	write_cmd(0xEB);	
	write_8bit(0x14); 

	write_cmd(0x84);			
	write_8bit(0x40); 

	write_cmd(0x85);			
	write_8bit(0xFF); 

	write_cmd(0x86);			
	write_8bit(0xFF); 

	write_cmd(0x87);			
	write_8bit(0xFF);

	write_cmd(0x88);			
	write_8bit(0x0A);

	write_cmd(0x89);			
	write_8bit(0x21); 

	write_cmd(0x8A);			
	write_8bit(0x00); 

	write_cmd(0x8B);			
	write_8bit(0x80); 

	write_cmd(0x8C);			
	write_8bit(0x01); 

	write_cmd(0x8D);			
	write_8bit(0x01); 

	write_cmd(0x8E);			
	write_8bit(0xFF); 

	write_cmd(0x8F);			
	write_8bit(0xFF); 


	write_cmd(0xB6);
	write_8bit(0x00);
	write_8bit(0x20);

	write_cmd(0x36);    //Set as vertical screen
	write_8bit(0x08);

	write_cmd(0x3A);			
	write_8bit(0x05); 


	write_cmd(0x90);			
	write_8bit(0x08);
	write_8bit(0x08);
	write_8bit(0x08);
	write_8bit(0x08); 

	write_cmd(0xBD);			
	write_8bit(0x06);
	
	write_cmd(0xBC);			
	write_8bit(0x00);	

	write_cmd(0xFF);			
	write_8bit(0x60);
	write_8bit(0x01);
	write_8bit(0x04);

	write_cmd(0xC3);			
	write_8bit(0x13);
	write_cmd(0xC4);			
	write_8bit(0x13);

	write_cmd(0xC9);			
	write_8bit(0x22);

	write_cmd(0xBE);			
	write_8bit(0x11); 

	write_cmd(0xE1);			
	write_8bit(0x10);
	write_8bit(0x0E);

	write_cmd(0xDF);			
	write_8bit(0x21);
	write_8bit(0x0c);
	write_8bit(0x02);

	write_cmd(0xF0);   
	write_8bit(0x45);
	write_8bit(0x09);
	write_8bit(0x08);
	write_8bit(0x08);
	write_8bit(0x26);
 	write_8bit(0x2A);

 	write_cmd(0xF1);    
 	write_8bit(0x43);
 	write_8bit(0x70);
 	write_8bit(0x72);
 	write_8bit(0x36);
 	write_8bit(0x37);  
 	write_8bit(0x6F);


 	write_cmd(0xF2);   
 	write_8bit(0x45);
 	write_8bit(0x09);
 	write_8bit(0x08);
 	write_8bit(0x08);
 	write_8bit(0x26);
 	write_8bit(0x2A);

 	write_cmd(0xF3);   
 	write_8bit(0x43);
 	write_8bit(0x70);
 	write_8bit(0x72);
 	write_8bit(0x36);
 	write_8bit(0x37); 
 	write_8bit(0x6F);

	write_cmd(0xED);	
	write_8bit(0x1B); 
	write_8bit(0x0B); 

	write_cmd(0xAE);			
	write_8bit(0x77);
	
	write_cmd(0xCD);			
	write_8bit(0x63);		


	write_cmd(0x70);			
	write_8bit(0x07);
	write_8bit(0x07);
	write_8bit(0x04);
	write_8bit(0x0E); 
	write_8bit(0x0F); 
	write_8bit(0x09);
	write_8bit(0x07);
	write_8bit(0x08);
	write_8bit(0x03);

	write_cmd(0xE8);			
	write_8bit(0x34);

	write_cmd(0x62);			
	write_8bit(0x18);
	write_8bit(0x0D);
	write_8bit(0x71);
	write_8bit(0xED);
	write_8bit(0x70); 
	write_8bit(0x70);
	write_8bit(0x18);
	write_8bit(0x0F);
	write_8bit(0x71);
	write_8bit(0xEF);
	write_8bit(0x70); 
	write_8bit(0x70);

	write_cmd(0x63);			
	write_8bit(0x18);
	write_8bit(0x11);
	write_8bit(0x71);
	write_8bit(0xF1);
	write_8bit(0x70); 
	write_8bit(0x70);
	write_8bit(0x18);
	write_8bit(0x13);
	write_8bit(0x71);
	write_8bit(0xF3);
	write_8bit(0x70); 
	write_8bit(0x70);

	write_cmd(0x64);			
	write_8bit(0x28);
	write_8bit(0x29);
	write_8bit(0xF1);
	write_8bit(0x01);
	write_8bit(0xF1);
	write_8bit(0x00);
	write_8bit(0x07);

	write_cmd(0x66);			
	write_8bit(0x3C);
	write_8bit(0x00);
	write_8bit(0xCD);
	write_8bit(0x67);
	write_8bit(0x45);
	write_8bit(0x45);
	write_8bit(0x10);
	write_8bit(0x00);
	write_8bit(0x00);
	write_8bit(0x00);

	write_cmd(0x67);			
	write_8bit(0x00);
	write_8bit(0x3C);
	write_8bit(0x00);
	write_8bit(0x00);
	write_8bit(0x00);
	write_8bit(0x01);
	write_8bit(0x54);
	write_8bit(0x10);
	write_8bit(0x32);
	write_8bit(0x98);

	write_cmd(0x74);			
	write_8bit(0x10);	
	write_8bit(0x85);	
	write_8bit(0x80);
	write_8bit(0x00); 
	write_8bit(0x00); 
	write_8bit(0x4E);
	write_8bit(0x00);					
	
    write_cmd(0x98);			
	write_8bit(0x3e);
	write_8bit(0x07);

	write_cmd(0x35);	
	write_cmd(0x21);

	write_cmd(0x11);
	delay(120);
	write_cmd(0x29);
	delay(20);
    end_write(); 
	set_direction(GC9A01A_USE_HORIZONTAL);
}

/**
 * @brief LCD背光控制
 * @param bright    0:关闭背光;其他值:打开背光.    
 */
void GC9A01A::backlight(uint8_t bright)
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
 * @brief 设置显示方向
 * @param direction    0~3.    
 */
void GC9A01A::set_direction(uint8_t direction)
{
	lcddev.dir = direction;
	start_write();
	switch (lcddev.dir) {
		case 0:
			lcddev.width = lcddev.width;
			lcddev.height = lcddev.height;
			/* BIT7:MY; BIT6:MX; BIT5:MV(行列交换); BIT4:ML; BIT3:0,RGB,1,BGR; BIT2:MH */
			write_cmd(0x36);
			write_8bit((0 << 7)|(0 << 6)|(0 << 5)|(0 << 4)|(GC9A01A_RGB_GBR_ORDER <<3));			
			break;
		case 1:
			lcddev.width = lcddev.height;
			lcddev.height = lcddev.width;
			write_cmd(0x36);
			write_8bit((0 << 7)|(1<<6)|(1 << 5)|(0 << 4)|(GC9A01A_RGB_GBR_ORDER << 3));
			break;
		case 2:
			lcddev.width = lcddev.width;
			lcddev.height = lcddev.height;
			write_cmd(0x36);
			write_8bit((1 << 7)|(1 << 6)|(0 << 5)|(0 << 4)|(GC9A01A_RGB_GBR_ORDER << 3));
			break;
		case 3:
			lcddev.width = lcddev.height;
			lcddev.height = lcddev.width;
			write_cmd(0x36);
			write_8bit((1 << 7)|(0 << 6)|(1 << 5)|(0 << 4)|(GC9A01A_RGB_GBR_ORDER << 3));
			break;
		default:
			break;
	}
	TWEN_GFX::set_direction(lcddev.dir);
	end_write();
}

/**
 * @brief LCD清屏
 * @param color    清屏的颜色.    
 */
void GC9A01A::clear(uint16_t color)
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
 * @brief LCD画点
 * @param x    x坐标值.    
 * @param y    y坐标值.    
 * @param color    颜色值.    
 */
void GC9A01A::draw_point(int16_t x, int16_t y, uint16_t color)
{
	start_write();
	set_windows(x, y, x, y);//设置光标位置 
	write_8bit(color>>8);
	write_8bit(color);
	end_write();
}

/**
 * @brief LCD填充矩形
 * @param x0       矩形起始坐标的x值
 * @param y0       矩形起始坐标的y值
 * @param x1       矩形结束坐标的x值
 * @param y1       矩形结束坐标的y值
 * @param color    颜色值.    
 */
void GC9A01A::fill_rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t color)
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
 * @brief    LCD显示图片
 * @param    x      图片起始坐标的x值
 * @param    y      图片起始坐标的y值
 * @param    width      图片宽度
 * @param    height     图片高度
 * @param    pic        图片数组
 */
void GC9A01A::show_picture(uint16_t x,uint16_t y,uint16_t width,uint16_t height,const uint8_t pic[])
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

#endif  //GC9A01A.h