#ifndef _CH32V_SOFTIIC_H_
#define _CH32V_SOFTIIC_H_

#include "asr.h"
#ifndef TW_ASR_PRO
#include "asr_delay.h"
#endif

// #define KI2C_DELAY_USEC 1

//读位
uint8_t const KI2C_READ = 1;

//写位
uint8_t const KI2C_WRITE = 0;

//------------------------------------------------------------------------------
class KIICBase {
public:
    //读函数
    //last:应答控制位
    virtual uint8_t read(uint8_t last) = 0;
    //重新开始信号
    //addr:设备地址
    virtual bool restart(uint8_t addr) = 0;
    //起始信号
    //addr:设备地址
    virtual bool start(uint8_t addr) = 0;
    //停止信号
    virtual void stop(void) = 0;
    //写函数
    //last:要写的数据
    virtual bool write(uint8_t data) = 0;
};
//---------------------------------------------------------------------

class KSoftIIC : public KIICBase {
public:
    KSoftIIC();
    void begin(uint8_t sdapin, uint8_t sclpin); //SOFTIIC初始化
    uint8_t read(uint8_t last); //读函数
    bool restart(uint8_t addr); //重复起始信号
    bool start(uint8_t addr); //起始信号
    void stop(void);  //停止信号
    bool write(uint8_t b);  //写函数
private:
    //SoftI2cMaster() {}
    uint8_t _sdapin;
    uint8_t _sclpin;
};

//========================================================================
// 描述: 构造函数.
// 参数: none
// 返回: none.
//========================================================================
KSoftIIC::KSoftIIC()
{}

//========================================================================
// 描述: 初始化函数.
// 参数: sdapin：SDA引脚; sclpin: scl引脚.
// 返回: none.
//========================================================================
void KSoftIIC::begin(uint8_t sdapin,uint8_t sclpin)
{
    _sdapin = sdapin;
    _sclpin = sclpin;
    pinMode(_sdapin,output);
    pinMode(_sclpin,output);
#if defined(TW_ASR_PRO)
    dpmu_set_io_open_drain(pinToFun[sdapin],ENABLE);
    dpmu_set_io_open_drain(pinToFun[sclpin],ENABLE);
    set_pin_to_gpio_mode(_sdapin);
    set_pin_to_gpio_mode(_sclpin);
#else
    if((_sdapin<=8) && (_sdapin >=5))
    {
        setPinFun(_sdapin,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(_sdapin,FIRST_FUNCTION);
    }
    if((_sclpin<=8) && (_sclpin >=5))
    {
        setPinFun(_sclpin,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(_sclpin,FIRST_FUNCTION);
    }
#endif
    digitalWrite(_sdapin,1);
    digitalWrite(_sclpin,1);
}

//========================================================================
// 描述: SOFTIIC发送起始信号.
// 参数: addr：设备地址.
// 返回: none.
//========================================================================
bool KSoftIIC::start(uint8_t addr)
{
	digitalWrite(_sdapin, 0);
	delayMicroseconds(5);
	digitalWrite(_sclpin, 0);
	delayMicroseconds(5);
	return write(addr);
}

//========================================================================
// 描述: SOFTIIC发送重复起始信号.
// 参数: none.
// 返回: none.
//========================================================================
bool KSoftIIC::restart(uint8_t addr)
{
	digitalWrite(_sdapin, 1);
	delayMicroseconds(5);
	digitalWrite(_sclpin, 1);
	delayMicroseconds(5);
	return start(addr);
}

//========================================================================
// 描述: SOFTIIC发送停止信号.
// 参数: none.
// 返回: none.
//========================================================================
void KSoftIIC::stop()
{
	digitalWrite(_sdapin,0);
	delayMicroseconds(5);
	digitalWrite(_sclpin,1);
	delayMicroseconds(5);
	digitalWrite(_sdapin,1);
	delayMicroseconds(5);
}

//========================================================================
// 描述: SOFTIIC读函数.
// 参数: last:0表示应答,1表示非应答.
// 返回: 读取到的数据.
//========================================================================
uint8_t KSoftIIC::read(uint8_t last) {
  uint8_t b = 0;
  digitalWrite(_sdapin, 1);
#if defined(TW_ASR_PRO)
  dpmu_set_io_open_drain(pinToFun[_sdapin],DISABLE);
#endif
  pinMode(_sdapin, input);
  for (uint8_t i = 0; i < 8; i++) {
    b <<= 1;
    delayMicroseconds(5);
    digitalWrite(_sclpin, 1);
	delayMicroseconds(5);
    if (digitalRead(_sdapin)) b |= 1;
    digitalWrite(_sclpin, 0);
    delayMicroseconds(5);
  }
#if defined(TW_ASR_PRO)
  dpmu_set_io_open_drain(pinToFun[_sdapin],ENABLE);
#endif
  pinMode(_sdapin, output);
  digitalWrite(_sdapin, last);
  digitalWrite(_sclpin, 1);
  delayMicroseconds(5);
  digitalWrite(_sclpin, 0);
  delayMicroseconds(5);
  digitalWrite(_sdapin, 0);
  return b;
}

//========================================================================
// 描述: 写函数.
// 参数: data：数据.
// 返回: 0,没有接收到应答;1,接收到应答.
//========================================================================
bool KSoftIIC::write(uint8_t data) {
  for (uint8_t m = 0X80; m != 0; m >>= 1) {
    if(m&data){
        digitalWrite(_sdapin, 1);
	  }else{
        digitalWrite(_sdapin, 0);
	  }
    digitalWrite(_sclpin, 1);
    delayMicroseconds(5);
    digitalWrite(_sclpin, 0);
    delayMicroseconds(5);
  }
  // get Ack or Nak
  pinMode(_sdapin, input);
  // enable pullup
  digitalWrite(_sdapin, 1);
  delayMicroseconds(5);
  digitalWrite(_sclpin, 1);
  delayMicroseconds(5);
  uint8_t rtn = digitalRead(_sdapin);
  digitalWrite(_sclpin, 0);
  delayMicroseconds(5);
  pinMode(_sdapin, output);
  digitalWrite(_sdapin, 0);
  return rtn == 0;
}

KSoftIIC softiic;

#endif  // asr_softiic.h
