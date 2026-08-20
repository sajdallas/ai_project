/**
 *  该库为TM1638芯片内部寄存器读写库
 */
#ifndef    _TM1638_H_
#define    _TM1638_H_ 

#include <asr.h>

class TM1638{
public:
    TM1638(uint8_t sckpin, uint8_t diopin, uint8_t stbpin);
    void begin(); //TM1638初始化
    void  Write(uint8_t  DATA) ;//写函数
    uint8_t Read() ;//读函数
    void  Write_COM(uint8_t cmd);
    uint8_t  Read_key(void);
    void  Write_DATA(uint8_t add,uint8_t DATA); 
    void  Write_DATA(uint8_t add,uint8_t* date,uint8_t len) ;
    void  Write_allLED(uint8_t LED_flag); 
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
TM1638::TM1638(uint8_t sckpin, uint8_t diopin, uint8_t stbpin)
{
    _sckpin = sckpin;
    _diopin = diopin;
    _stbpin = stbpin;
}

//========================================================================
// 描述: 初始化函数.
// 参数: none.
// 返回: none.
//========================================================================
void TM1638::begin()
{
    set_pin_to_gpio_mode(_sckpin);
	dpmu_set_io_pull((PinPad_Name)pinToFun[_sckpin],DPMU_IO_PULL_UP);
    set_pin_to_gpio_mode(_diopin);
	dpmu_set_io_pull((PinPad_Name)pinToFun[_diopin],DPMU_IO_PULL_DISABLE);
    set_pin_to_gpio_mode(_stbpin);
	dpmu_set_io_pull((PinPad_Name)pinToFun[_stbpin],DPMU_IO_PULL_UP);
    pinMode(_sckpin, output);
    pinMode(_diopin, output);
    pinMode(_stbpin, output);
}

//========================================================================
// 描述: 写函数.
// 参数: sckpin:sck引脚; mosipin: mosi引脚; misopin：miso引脚.
// 返回: 读到的数据.
//========================================================================
void  TM1638::Write(uint8_t  DATA)                        //写数据函数
{
    uint8_t i;
    pinMode(_diopin,output);
    for(i=0;i<8;i++)
    {
        digitalWrite(_sckpin,0);
        if(DATA&0X01)
            digitalWrite(_diopin,1);
        else
            digitalWrite(_diopin,0);
        DATA>>=1;
        digitalWrite(_sckpin,1);
    }
}


//========================================================================
// 描述: 读函数.
// 参数: none.
// 返回: 读到的数据.
//========================================================================
uint8_t  TM1638::Read()
{
    unsigned char i;
    unsigned char temp=0;;        
    pinMode(_diopin,input);//设置为输入
    digitalWrite(_diopin,1);
    for(i=0;i<8;i++)
    {
        temp>>=1;
        digitalWrite(_sckpin,0);
        delay1us();
        delay1us();
        // digitalWrite(_sckpin,1);
        if(digitalRead(_diopin)==1){
            temp|=0x80;
        }
        digitalWrite(_sckpin,1);        
    }
    return temp;
}

//========================================================================
// 描述: 发送命令字.
// 参数: cmd:命令.
// 返回: none.
//========================================================================
void TM1638::Write_COM(uint8_t cmd)
{
    digitalWrite(_stbpin,0);
    Write(cmd);
    digitalWrite(_stbpin,1);
}

//========================================================================
// 描述: 读键扫数据寄存器.
// 参数: none.
// 返回: none.
//========================================================================
uint8_t TM1638::Read_key(void)
{
    uint8_t c[4],i,key_value=0;
    digitalWrite(_stbpin,0);
    Write(0x42);  //读键扫数据 命令
    for(i=0;i<4;i++)                
    {
        c[i] = Read();
    }
    digitalWrite(_stbpin,1);   
    for(i=0;i<4;i++)    //4个字节数据合成一个字节
    {
        key_value|=c[i]<<i;
    }        
	return key_value; 
}

//========================================================================
// 描述: 指定地址写入数据.
// 参数: add:显示地址(00H-0FH); DATA:数据.
// 返回: none.
//========================================================================
void TM1638::Write_DATA(uint8_t add,uint8_t DATA)
{
    Write_COM(0x44); //0100 0100b
    digitalWrite(_stbpin,0);
    Write(0xc0|add);
    Write(DATA);
    digitalWrite(_stbpin,1);
}

//========================================================================
// 描述: 从指定地址开始写入多个数据.
// 参数: add:起始显示地址(00H-0FH); dat:数据指针; len:长度.
// 返回: none.
//========================================================================
void TM1638::Write_DATA(uint8_t add,uint8_t* dat,uint8_t len)
{
    Write_COM(0x40); //0100 0000b
    digitalWrite(_stbpin,0);
    Write(0xc0|add);
    for(int i=0;i<len;i++)
    {
        Write(dat[i]);
    }
    digitalWrite(_stbpin,1);
}

#endif  //tm1638.h
