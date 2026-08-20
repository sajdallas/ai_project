/**
 * @file asr_blueRF.h
 * @brief 常用无线协议发送解析模块驱动的头文件
 * @details 支持433M、315M等常用频段的2262、1527、Dooya等协议发送和解析
 * @version 0.1
 * @date 2022-06-22
 * @author Blue
 * @copyright Copyright (c) 2021 TWen51 Technology Co., Ltd.
 * 
 */
 
#ifndef _ASR_BLUE_RF_H_
#define _ASR_BLUE_RF_H_

#include "asr.h"
#include "asr_hwtimer.h"

//----------------------------------------------------------------------------------------
//-----------------------------------数据链路层--------------------------------------------
//----------------------------------------------------------------------------------------

//根据需要设置缓存长度
#define _ASR_BLUERF_BUF_MAX 82 //40bit*2+syn*2

typedef struct {
    uint8_t  tx_pin; //发送引脚   
    uint8_t  rx_pin; //接收引脚      
    uint16_t buf[_ASR_BLUERF_BUF_MAX]; //发送缓存
    uint64_t idx; //数据索引
    uint64_t len; //缓存数据长度
    uint8_t  cnt; //发送次数计数
    uint8_t  repeat; //重发次数
    timer_base_t timer;
    bool invertedSignal;
} rfparams_t;

volatile rfparams_t rfparams; //无线参数

class BlueRFLink
{
  public:
    BlueRFLink(uint8_t tx_pin, timer_base_t timer); 

    struct HighLow {
        uint8_t high_pulse;
        uint8_t low_pulse;
    };

    struct Protocol {
        /** base pulse length in microseconds, e.g. 350 */
        uint16_t pulseLength;

        HighLow sync;
        HighLow zero;
        HighLow one;

        /**
         * If true, interchange high and low logic levels in all transmissions.
         *
         * By default, BlueRFLink assumes that any signals it sends or receives
         * can be broken down into pulses which start with a high signal level,
         * followed by a a low signal level. This is e.g. the case for the
         * popular PT 2260 encoder chip, and thus many switches out there.
         *
         * But some devices do it the other way around, and start with a low
         * signal level, followed by a high signal level, e.g. the HT6P20B. To
         * accommodate this, one can set invertedSignal to true, which causes
         * BlueRFLink to change how it interprets any HighLow struct FOO: It will
         * then assume transmissions start with a low signal lasting
         * FOO.high*pulseLength microseconds, followed by a high signal lasting
         * FOO.low*pulseLength microseconds.
         */
        bool invertedSignal;
    };
    Protocol protocol;
    void setProtocol(Protocol protocol);
    void setProtocol(int nProtocol);
    void setProtocol(int nProtocol, int nPulseLength);

    void setPulseLength(int nPulseLength);
    void setRepeatTransmit(int nRepeatTransmit);

    void sendRaw(unsigned long long code, unsigned int length);

  private:
    uint8_t _timerx_irq;
};

static const BlueRFLink::Protocol proto[]=
{
  { 350, {  1, 31 }, {  1,  3 }, {  3,  1 }, false },    // protocol 1
  { 650, {  1, 10 }, {  1,  2 }, {  2,  1 }, false },    // protocol 2
  { 100, { 30, 71 }, {  4, 11 }, {  9,  6 }, false },    // protocol 3
  { 380, {  1,  6 }, {  1,  3 }, {  3,  1 }, false },    // protocol 4
  { 500, {  6, 14 }, {  1,  2 }, {  2,  1 }, false },    // protocol 5
  { 450, { 23,  1 }, {  1,  2 }, {  2,  1 }, true },     // protocol 6 (HT6P20B)
  { 150, {  2, 62 }, {  1,  6 }, {  6,  1 }, false },    // protocol 7 (HS2303-PT, i. e. used in AUKEY Remote)
  { 200, {  3, 130}, {  7, 16 }, {  3,  16}, false},     // protocol 8 Conrad RS-200 RX
  { 200, { 130, 7 }, {  16, 7 }, { 16,  3 }, true},      // protocol 9 Conrad RS-200 TX
  { 365, { 18,  1 }, {  3,  1 }, {  1,  3 }, true },     // protocol 10 (1ByOne Doorbell)
  { 270, { 36,  1 }, {  1,  2 }, {  2,  1 }, true },     // protocol 11 (HT12E)
  { 320, { 36,  1 }, {  1,  2 }, {  2,  1 }, true },     // protocol 12 (SM5212)
  { 400, {  1, 31 }, {  1,  3 }, {  3,  1 }, false },    // protocol 13 公牛无线开关
  {  20, { 239, 78}, { 20, 35 }, { 35, 20 }, false}      // protocol 14 Dooya DC1600
};

enum {
   numProto = sizeof(proto) / sizeof(proto[0])
};

/**
  * @brief  发送初始化
  * @param  tx_pin 引脚
  * @param  timer 调用定时器（TIMER0/1/2/3）
  */
BlueRFLink::BlueRFLink(uint8_t tx_pin, timer_base_t timer)
{
    rfparams.tx_pin = tx_pin;
    rfparams.len = 0;
    this->setRepeatTransmit(10);//默认重复发送10次
    this->setProtocol(1);//默认使用协议1
    rfparams.timer = timer;
    rfparams.invertedSignal = protocol.invertedSignal;

    if(rfparams.timer == TIMER0)
    {
      _timerx_irq = TIMER0_IRQn;
    }
    else if(rfparams.timer == TIMER1)
    {
      _timerx_irq = TIMER1_IRQn;
    }
    else if(rfparams.timer == TIMER2)
    {
      _timerx_irq = TIMER2_IRQn;
    }
    else if(rfparams.timer == TIMER3)
    {
      _timerx_irq = TIMER3_IRQn;
    }
}

/**
  * Sets the protocol to send.
  */
void BlueRFLink::setProtocol(Protocol protocol) {
	this->protocol = protocol;
}

/**
  * Sets the protocol to send, from a list of predefined protocols
  */
void BlueRFLink::setProtocol(int nProtocol) {
	if (nProtocol < 1 || nProtocol > numProto) {
		nProtocol = 1;  // TODO: trigger an error, e.g. "bad protocol" ???
	}

	memcpy(&this->protocol, &proto[nProtocol-1], sizeof(Protocol));
}

/**
  * Sets the protocol to send with pulse length in microseconds.
  */
void BlueRFLink::setProtocol(int nProtocol, int nPulseLength) {
	setProtocol(nProtocol);
	this->setPulseLength(nPulseLength);
}


/**
  * Sets pulse length in microseconds
  */
void BlueRFLink::setPulseLength(int nPulseLength) {
	this->protocol.pulseLength = nPulseLength;
}

/**
 * Sets Repeat Transmits
 */
void BlueRFLink::setRepeatTransmit(int nRepeatTransmit) {
	rfparams.repeat = nRepeatTransmit;
}

/**
 * @brief timer中断处理函数
 *
 */
#ifdef __cplusplus
extern "C"{
#endif

static void rf_timer_timeout_deal(void)
{
	timer_clear_irq(rfparams.timer);

	if(rfparams.cnt < rfparams.repeat)
	{
		if(rfparams.idx < rfparams.len)
		{
		digitalWrite(rfparams.tx_pin,!digitalRead(rfparams.tx_pin));//反转IO
		timer_set_count(rfparams.timer,get_apb_clk()/1000000*rfparams.buf[rfparams.idx++]);//更新脉宽时间
		timer_start(rfparams.timer);
		}
		else
		{
		rfparams.cnt++;

		if(rfparams.invertedSignal)
		{
			digitalWrite(rfparams.tx_pin,1);//高电平
		}
		else
		{
			digitalWrite(rfparams.tx_pin,0);//低电平
		}

		rfparams.idx = 0;
		timer_set_count(rfparams.timer,get_apb_clk()/100000);//10us继续下一次
		timer_start(rfparams.timer);
		}
	}
	else
	{
		timer_stop(rfparams.timer);
	}
}

#ifdef __cplusplus
} // extern "C"
#endif

// /**
//  * @param sCodeWord   a binary code word consisting of the letter 0, 1
//  */
// void BlueRFLink::sendRaw(const char* sCodeWord) {
//   // turn the tristate code word into the corresponding bit pattern, then send it
//   unsigned long code = 0;
//   unsigned int length = strlen(sCodeWord);
//   for (uint8_t i=0; i<length; i++) {
//     code <<= 1L;
//     if (*sCodeWord != '0')
//       code |= 1L;
//     sCodeWord++;
//   }
//   this->sendRaw(code, length);
// }

/**
  * @brief  数据发送
  * @param code 原始数据 
  * @param length 长度
  */
void BlueRFLink::sendRaw(unsigned long long code, unsigned int length)
{  
	pinMode(rfparams.tx_pin,output);//输出模式
#if defined(TW_ASR_PRO)
	set_pin_to_gpio_mode(rfparams.tx_pin);
#else
	if((rfparams.tx_pin<=8) && (rfparams.tx_pin >=5))
	{
		setPinFun(rfparams.tx_pin,SECOND_FUNCTION);
	}
	else
	{
		setPinFun(rfparams.tx_pin,FIRST_FUNCTION);
	}
#endif
	if(protocol.invertedSignal)
	{
		digitalWrite(rfparams.tx_pin,1);//高电平
	}
	else
	{
		digitalWrite(rfparams.tx_pin,0);//低电平
	}
	//转码
	//1.同步头
	rfparams.buf[0]=protocol.sync.high_pulse*protocol.pulseLength;
	rfparams.buf[1]=protocol.sync.low_pulse*protocol.pulseLength;
	
	//2.数据
	for (int i = length-1; i >= 0; i--) 
	{
		if (code & (1ULL << (length - i - 1)))
		{
		rfparams.buf[2+i*2]=protocol.one.high_pulse*protocol.pulseLength;
		rfparams.buf[2+i*2+1]=protocol.one.low_pulse*protocol.pulseLength;
		}
		else
		{
		rfparams.buf[2+i*2]=protocol.zero.high_pulse*protocol.pulseLength;
		rfparams.buf[2+i*2+1]=protocol.zero.low_pulse*protocol.pulseLength;
		}
	}

	rfparams.len = length*2+2;
	rfparams.idx = 0;
	rfparams.cnt = 0;

  //调用定时器中断去完成发送
  //配置TIMER的中断
#if defined(TW_ASR_PRO)
	scu_set_device_gate(rfparams.timer,ENABLE);
#else
	Scu_SetDeviceGate(rfparams.timer,ENABLE);
#endif
	__eclic_irq_set_vector(_timerx_irq,(int)rf_timer_timeout_deal);
	eclic_irq_enable(_timerx_irq);

	timer_init_t rftimer_init;
	rftimer_init.mode  = timer_count_mode_single;
	rftimer_init.div   = timer_clk_div_0;
	rftimer_init.width = timer_iqr_width_f;
	rftimer_init.count = get_apb_clk()/1000;//1ms后开始启动
	timer_init(rfparams.timer,rftimer_init);

	timer_start(rfparams.timer);
}

//----------------------------------------------------------------------------------------
//-------------------------------------应用层----------------------------------------------
//----------------------------------------------------------------------------------------
 /**
 * @brief 1527协议应用层数据收发
 * 1527编码
 * 同步+地址（C0-C19）+开关（D0 D1 D2 D3）
 * 同步   _|——|____________________|
 * 时间   _|1T|________31T_________|
 * DATA(H)_|————————|__|
 *        _|———3T———|1T|
 * DATA(L)_|——|________|
 *        _|1T|———3T———|
 * 16LCK=4T
 */
class rf1527:public BlueRFLink
{
  public:
    rf1527(uint8_t tx_pin, timer_base_t timer):BlueRFLink(tx_pin,timer)
    {
      setProtocol(13);//默认使用公牛的协议，这样公牛面板和其他厂家的面板都能兼容，公牛面板只支持这个。
    };
    void send(uint32_t address, uint8_t data);
};

void rf1527::send(uint32_t address, uint8_t data)
{
  sendRaw(((address<<4) | (data&0x0f)), 24);
}

/**
 * @brief 2262协议应用层数据收发
 * 脉冲宽度300-500us,默认350
 * 同步   _|——|____________________|
 * 时间   _|1T|________31T_________|
 * DATA(0)_|——|________|——|________| 数据0
 * DATA(1)_|————————|__|————————|__| 数据1
 * DATA(F)_|——|________|————————|__| 浮空
 * A0 A1 A2 A3 A4 A5 A6/D5 A7/D4 A8/D3 A9/D2 A10/D1 A11/D0
 * 最大12位地址，包含了6位数据地址共用位。
 * 底层数据发送还是和1527协议一样转换为24位长度发送
 * 不同厂商对地址的分配不同，需要根据实际情况做应用层数据解析
 */
class rf2262:public BlueRFLink
{
  public:
    rf2262(uint8_t tx_pin, timer_base_t timer):BlueRFLink(tx_pin,timer){setProtocol(1);};
    void sendTriState(const char* sCodeWord);
};

/**
  * @brief  2262三态数据发送
  * @param sCodeWord 三态字符串（"0000011111FF"）
  */
void rf2262::sendTriState(const char* sCodeWord) {
  // turn the tristate code word into the corresponding bit pattern, then send it
  unsigned long code = 0;
  unsigned int length = 0;
  for (const char* p = sCodeWord; *p; p++) {
    code <<= 2L;
    switch (*p) {
      case '0':
        // bit pattern 00
        break;
      case 'F':
        // bit pattern 01
        code |= 1L;
        break;
      case '1':
        // bit pattern 11
        code |= 3L;
        break;
    }
    length += 2;
  }
  sendRaw(code, length);
}

 /**
 * @brief DOOYA电机协议应用层数据收发
 * 1527编码
 * 同步+28 Bit ID + 4 Bit CH + 8 Bit CMD
 * 同步   _|——————————————|________|
 * 时间   _|————11.95T————|__3.9T__|
 * DATA(H)_|————————|__|
 *        _|——1.75T—|1T|
 * DATA(L)_|——|________|
 *        _|1T|——1.75T—|
 * CH=0 群控，最大15
 * CMD=0x11 开；CMD=0x55 停；CCMD=0x33 关；MD=0xCC 对码；CMD=0x1E 固定点动开；CMD=0x3C 固定点动关；
 */
class dooya:public BlueRFLink
{
  public:
    dooya(uint8_t tx_pin, timer_base_t timer):BlueRFLink(tx_pin,timer)
    {
      setProtocol(14);
      setChannel(1);
      setAddress(0xfffffff);
    };
    void motorCW();
    void motorCCW();
    void motorStop();
    void motorInchingCW();
    void motorInchingCCW();
    void matchCode();
    void setAddress(uint32_t addr);
    void setChannel(uint8_t ch);
    void sendCmd(uint8_t cmd);
  private:
    uint32_t _address;
    uint8_t _ch;
};

/**
  * @brief  设置出厂地址
  * @param addr 地址
  */
void dooya::setAddress(uint32_t addr)
{
  _address = addr;
}

/**
  * @brief  设置通道
  * @param ch 通道，0为群控，最大15
  */
void dooya::setChannel(uint8_t ch)
{
  _ch = ch;
}

/**
  * @brief  电机顺时针方向转动
  */
void dooya::motorCW()
{
  sendRaw(((((uint64_t)_address)<<12) | (_ch<<8) | 0x11), 40);
}

/**
  * @brief  电机逆时针方向转动
  */
void dooya::motorCCW()
{
  sendRaw(((((uint64_t)_address)<<12) | (_ch<<8) | 0x33), 40);
}

/**
  * @brief  停止电机
  */
void dooya::motorStop()
{
  sendRaw(((((uint64_t)_address)<<12) | (_ch<<8) | 0x55), 40);
}

/**
  * @brief  电机顺时针方向点动
  */
void dooya::motorInchingCW()
{
  sendRaw(((((uint64_t)_address)<<12) | (_ch<<8) | 0x1E), 40);
}

/**
  * @brief  电机逆时针方向点动
  */
void dooya::motorInchingCCW()
{
  sendRaw(((((uint64_t)_address)<<12) | (_ch<<8) | 0x3C), 40);
}

/**
  * @brief  发送对码
  */
void dooya::matchCode()
{
  sendRaw(((((uint64_t)_address)<<12) | (_ch<<8) | 0xCC), 40);
}

/**
  * @brief  发送自定义命令
  * 还有开关灯等其他命令，根据实际情况自己设置
  */
void dooya::sendCmd(uint8_t cmd)
{
  sendRaw(((((uint64_t)_address)<<12) | (_ch<<8) | cmd), 40);
}

#endif //_ASR_BLUE_RF_H_