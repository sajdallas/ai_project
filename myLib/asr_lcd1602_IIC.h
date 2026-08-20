#ifndef _asr_LCD1602_IIC_h_
#define _asr_LCD1602_IIC_h_

#include "asr_softiic.h"
#include <inttypes.h>
#include "Print.h" 

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04  // Enable bit
#define Rw 0x02  // Read/Write bit
#define Rs 0x01  // Register select bit
// #define En B00000100  // Enable bit
// #define Rw B00000010  // Read/Write bit
// #define Rs B00000001  // Register select bit

class CH32V_IIC : public Print {
public:
	CH32V_IIC(uint8_t sda, uint8_t scl);
	void begin(uint8_t addr = 0x38);	//初始化
	void clear();	//清屏
	void setCursor(uint8_t, uint8_t); 	//设置显示的位置（列，行）
	void printstr(String c);	//打印函数
	void blink_on();	//开启光标闪烁
	void blink_off();	//关闭光标闪烁
	void cursor_on(); //开启下划线闪烁
	void cursor_off();   //关闭下划线闪烁
	void noDisplay();	//关闭显示
	void display();	//开启显示
	void noBacklight();	//关闭背光
	void backlight();	//开启背光

	void home();
	void noBlink();	//开启光标闪烁
	void blink();	//关闭光标闪烁
	void noCursor();	//开启下划线闪烁
	void cursor();	//关闭下划线闪烁
	void scrollDisplayLeft();
	void scrollDisplayRight();
	void printLeft();
	void printRight();
	void leftToRight();
	void rightToLeft();
	void shiftIncrement();
	void shiftDecrement();
	void autoscroll();
	void noAutoscroll(); 
	void createChar(uint8_t, uint8_t[]);
	
	virtual size_t write(uint8_t);
	void command(uint8_t);
	
	//compatibility API function aliases
	void setBacklight(uint8_t new_val);		//0关闭背光; 其它值:打开背光.
	void load_custom_character(uint8_t char_num, uint8_t *rows);
	//void printstr(const char[]);

	//Unsupported API functions (not implemented in this library)
	uint8_t status();
	void setContrast(uint8_t new_val);
	uint8_t keypad();
	void setDelay(int,int);
	void on();
	void off();
	uint8_t init_bargraph(uint8_t graphtype);
	void draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end);
	void draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end);

private:
	KSoftIIC HD_1602;
	void send(uint8_t, uint8_t);
	void write4bits(uint8_t);
	void expanderWrite(uint8_t);
	void pulseEnable(uint8_t);
	void begin_start(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS );
	uint8_t _Addr;
	uint8_t _displayfunction;
	uint8_t _displaycontrol;
	uint8_t _displaymode;
	uint8_t _numlines;
	uint8_t _cols;
	uint8_t _rows;
	uint8_t _backlightval;
  
	uint8_t SCL_pin,SDA_pin;
	uint8_t i2c_addr;
};

//========================================================================
// 描述: 写入一个字节数据.
// 参数: none
// 返回: none.
//========================================================================
inline size_t CH32V_IIC::write(uint8_t value) {
	send(value, Rs);
	return 0;
}

//========================================================================
// 描述: 引脚初始化.
// 参数: none
// 返回: none.
//========================================================================
CH32V_IIC::CH32V_IIC(uint8_t sda, uint8_t scl)
{	
	SDA_pin=sda;
	SCL_pin=scl;

	pinMode(SCL_pin,OUTPUT);
	pinMode(SDA_pin,OUTPUT);
    setPinFun(SCL_pin,FIRST_FUNCTION);
    setPinFun(SDA_pin,FIRST_FUNCTION);

    
	
	_cols = 16;
	_rows = 2;
	_backlightval = LCD_BACKLIGHT;
}

//========================================================================
// 描述: 初始化.
// 参数: none
// 返回: none.
//========================================================================
void CH32V_IIC::begin(uint8_t addr){
	i2c_addr = (addr<<1);
	HD_1602.begin(SDA_pin,SCL_pin);  
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	begin_start(_cols, _rows);  
}

//========================================================================
// 描述: 初始化（内部调用）.
// 参数: none
// 返回: none.
//========================================================================
void CH32V_IIC::begin_start(uint8_t cols, uint8_t lines, uint8_t dotsize) {
	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delay(50); 
  
	// Now we pull both RS and R/W low to begin commands
	//expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	delay(1000);

	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46
	
	// we start in 8bit mode, try to set 4 bit mode
	write4bits(0x03 << 4);
	delayMicroseconds(4500); // wait min 4.1ms

	// second try
	write4bits(0x03 << 4);
	delayMicroseconds(4500); // wait min 4.1ms

	// third go!
	write4bits(0x03 << 4); 
	delayMicroseconds(150);

	// finally, set to 4-bit interface
	write4bits(0x02 << 4); 

	// delayMicroseconds(2000); 

	// set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);  
	
	//delayMicroseconds(2000); 
	
	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();
	
	//delayMicroseconds(2000); // wait min 4.1ms
	
	// clear it off
	clear();
	
	//delayMicroseconds(2000); // wait min 4.1ms
	
	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);
		
	home();
}

//========================================================================
// 描述: 清屏.
// 参数: none
// 返回: none.
//========================================================================
void CH32V_IIC::clear(){
	command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

//========================================================================
// 描述: 将光标位置设置为零.
// 参数: none
// 返回: none.
//========================================================================
void CH32V_IIC::home(){
	command(LCD_RETURNHOME);  // set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

//========================================================================
// 描述: 设置坐标.
// 参数: col，row：列，行.
// 返回: none.
//========================================================================
void CH32V_IIC::setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > _numlines ) {
		row = _numlines-1;    // we count rows starting w/0
	}
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

//========================================================================
// 描述: 关闭显示.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

//========================================================================
// 描述: 开启显示.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::display() {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

//========================================================================
// 描述: 关闭下划线光标.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

//========================================================================
// 描述: 开启下划线光标.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::cursor() {
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

//========================================================================
// 描述: 关闭闪烁的光标.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

//========================================================================
// 描述: 打开闪烁的光标.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::blink() {
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

//========================================================================
// 描述: 左滚动显示.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::scrollDisplayLeft(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

//========================================================================
// 描述: 右滚动显示.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::scrollDisplayRight(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

//========================================================================
// 描述: 文本从左到右显示.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

//========================================================================
// 描述: 文本从右到左显示.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

//========================================================================
// 描述: 左对齐.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

//========================================================================
// 描述: 右对齐.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

//========================================================================
// 描述: 生成字符.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		write(charmap[i]);
	}
}

//========================================================================
// 描述: 关闭背光.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::noBacklight(void) {
	_backlightval=LCD_NOBACKLIGHT;
	expanderWrite(0);
}

//========================================================================
// 描述: 打开背光.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::backlight(void) {
	_backlightval=LCD_BACKLIGHT;
	expanderWrite(0);
}

//========================================================================
// 描述: 写命令.
// 参数: none.
// 返回: none.
//========================================================================
inline void CH32V_IIC::command(uint8_t value) {
	send(value, 0);
}

//========================================================================
// 描述: 发送数据或命令.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::send(uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
    write4bits((highnib)|mode);
	write4bits((lownib)|mode); 
}

//========================================================================
// 描述: 写入4位数据.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::write4bits(uint8_t value) {
	expanderWrite(value);
	//delayMicroseconds(1);
	pulseEnable(value);
}

//========================================================================
// 描述: 写入4位数据（内部调用）.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::expanderWrite(uint8_t _data){                                         
    HD_1602.start(i2c_addr | KI2C_WRITE);
    HD_1602.write((int)(_data) | _backlightval);
    HD_1602.stop();  
}

//========================================================================
// 描述: 脉冲使能.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::pulseEnable(uint8_t _data){
	expanderWrite(_data | En);	// En high
	delayMicroseconds(1);		// enable pulse must be >450ns
	
	expanderWrite(_data & ~En);	// En low
	delayMicroseconds(50);		// commands need > 37us to settle
} 


//========================================================================
// 描述: 开启下划线光标.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::cursor_on(){
	cursor();
}

//========================================================================
// 描述: 开启下划线光标.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::cursor_off(){
	noCursor();
}

//========================================================================
// 描述: 开启光标闪烁.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::blink_on(){
	blink();
}

//========================================================================
// 描述: 关闭光标闪烁.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::blink_off(){
	noBlink();
}

//========================================================================
// 描述: 生成字符.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::load_custom_character(uint8_t char_num, uint8_t *rows){
	createChar(char_num, rows);
}

//========================================================================
// 描述: 设置背光.
// 参数: new_val:0关闭背光; 其它值:打开背光.
// 返回: none.
//========================================================================
void CH32V_IIC::setBacklight(uint8_t new_val){
	if(new_val){
		backlight();		// turn backlight on
	}else{
		noBacklight();		// turn backlight off
	}
}

//========================================================================
// 描述: 打印字符.
// 参数: none.
// 返回: none.
//========================================================================
void CH32V_IIC::printstr(String c){
	print(c);
}

// unsupported API functions
void CH32V_IIC::off(){}
void CH32V_IIC::on(){}
void CH32V_IIC::setDelay (int cmdDelay,int charDelay) {}
uint8_t CH32V_IIC::status(){return 0;}
uint8_t CH32V_IIC::keypad (){return 0;}
uint8_t CH32V_IIC::init_bargraph(uint8_t graphtype){return 0;}
void CH32V_IIC::draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end){}
void CH32V_IIC::draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_row_end){}
void CH32V_IIC::setContrast(uint8_t new_val){}

#endif  //asr_lcd1602_IIC.h
