/****************************************************************************
 * @file asr_blueRF.h
 * @brief 常用无线协议接收以及解析库
 * @details 支持433M、315M等常用频段的1527协议的接收以及解析
 * @version 0.1
 * @date 2022-06-27
 * @author Blue
 * @copyright Copyright (c) 2021 TWen51 Technology Co., Ltd.
 ****************************************************************************/
#ifndef _ASR_RCSWITCH_H
#define _ASR_RCSWITCH_H

#include "asr.h"
#ifndef TW_ASR_PRO
#include "asr_delay.h"
#endif

// Number of maximum _high/Low changes per packet.
// We can handle up to (unsigned long) => 32 bit * 2 H/L changes per bit + 2 for sync
#define RCSWITCH_MAX_CHANGES    67

static uint8_t nReceiverInterrupt;

class RCSwitch 
{
public:
    // RCSwitch(uint8_t tx_pin, timer_base_t timer);
    RCSwitch();

    void enableReceive(uint8_t interrupt);
    void disableReceive();
    bool available();
    void resetAvailable();

    uint32_t getReceivedValue();
    unsigned int getReceivedBitlength();
    unsigned int getReceivedDelay();
    unsigned int getReceivedProtocol();
    unsigned int* getReceivedRawdata();

    void setReceiveTolerance(int nPercent);

    /**
     * Description of a single pule, which consists of a _high signal
     * whose duration is "_high" times the base pulse length, followed
     * by a _low signal lasting "_low" times the base pulse length.
     * Thus, the pulse overall lasts (_high+_low)*pulseLength
     */
    struct HighLow {
        uint8_t _high;
        uint8_t _low;
    };

    struct Protocol {
        uint16_t pulseLength;  //同步头高电平脉宽(us)
        HighLow syncFactor; /* 同步头的高电平和低电平脉宽比 */
        HighLow zero;  /* 数据0的高电平和低电平脉宽比 */
        HighLow one;   /* 数据1的高电平和低电平脉宽比 */

        /**
         * If true, interchange _high and _low logic levels in all transmissions.
         *
         * By default, RCSwitch assumes that any signals it sends or receives
         * can be broken down into pulses which start with a _high signal level,
         * followed by a a _low signal level. This is e.g. the case for the
         * popular PT 2260 encoder chip, and thus many switches out there.
         *
         * But some devices do it the other way around, and start with a _low
         * signal level, followed by a _high signal level, e.g. the HT6P20B. To
         * accommodate this, one can set invertedSignal to true, which causes
         * RCSwitch to change how it interprets any HighLow struct FOO: It will
         * then assume transmissions start with a _low signal lasting
         * FOO._high*pulseLength microseconds, followed by a _high signal lasting
         * FOO._low*pulseLength microseconds.
         */
        bool invertedSignal;  //如果为真，则交换所有传输中_high和_low的逻辑。
    };

private:
    static void handleInterrupt();
    static bool receiveProtocol(const int p, unsigned int changeCount);

    int nTransmitterPin;
    int nRepeatTransmit;
    
    Protocol protocol;

    static int nReceiveTolerance;
    volatile static uint32_t nReceivedValue;
    volatile static unsigned int nReceivedBitlength;
    volatile static unsigned int nReceivedDelay;
    volatile static unsigned int nReceivedProtocol;
    const static unsigned int nSeparationLimit;
    /* 
    * timings[0] contains sync timing, followed by a number of bits
    */
    static unsigned int timings[RCSWITCH_MAX_CHANGES];
};

static const RCSwitch::Protocol recv_proto[] = {
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
  {  20, { 239, 78}, { 20, 35 }, { 35, 20 }, false },    // protocol 14 Dooya DC1600
};

enum {
   numRecvProto = sizeof(recv_proto) / sizeof(recv_proto[0])
};

volatile uint32_t RCSwitch::nReceivedValue = 0;
volatile unsigned int RCSwitch::nReceivedBitlength = 0;
volatile unsigned int RCSwitch::nReceivedDelay = 0;
volatile unsigned int RCSwitch::nReceivedProtocol = 0;
int RCSwitch::nReceiveTolerance = 60;
const unsigned int RCSwitch::nSeparationLimit = 4300;
// separationLimit: minimum microseconds between received codes, closer codes are ignored.
// according to discussion on issue #14 it might be more suitable to set the separation
// limit to the same time as the '_low' part of the sync signal for the current protocol.
unsigned int RCSwitch::timings[RCSWITCH_MAX_CHANGES];


RCSwitch::RCSwitch() 
{
  nReceiverInterrupt = -1;
  this->setReceiveTolerance(100);
  RCSwitch::nReceivedValue = 0;
}

/*************************************************************************
 * @fn      setReceiveTolerance
 * @brief   设置接收的容许公差.
 * @param   interrupt - 中断号        
 * @return  none
 *************************************************************************/
void RCSwitch::setReceiveTolerance(int nPercent) {
  RCSwitch::nReceiveTolerance = nPercent;
}

/*************************************************************************
 * @fn      enableReceive
 * @brief   使能接收.
 * @param   interrupt - 中断号        
 * @return  none
 *************************************************************************/
void RCSwitch::enableReceive(uint8_t interrupt) {
    nReceiverInterrupt = interrupt;
    if (nReceiverInterrupt != -1) 
    {
        RCSwitch::nReceivedValue = 0;
        RCSwitch::nReceivedBitlength = 0;
        set_pin_to_gpio_mode(interrupt);
        pinMode(interrupt,input);
        Set_GPIO_irq(nReceiverInterrupt,both_edges_trigger,(int32_t)(handleInterrupt));
    }
}

/*************************************************************************
 * @fn      disableReceive
 * @brief   失能接收.
 * @param   none       
 * @return  none
 *************************************************************************/
void RCSwitch::disableReceive() 
{
    nReceiverInterrupt = -1;
}

/*************************************************************************
 * @fn      available
 * @brief   是否接收到数据.
 * @param   none       
 * @return  true:接收到数据；false：没有接收到数据
 *************************************************************************/
bool RCSwitch::available() 
{
  return RCSwitch::nReceivedValue != 0;
}

/*************************************************************************
 * @fn      resetAvailable
 * @brief   接收到数据标志清零.
 * @param   none       
 * @return  none
 *************************************************************************/
void RCSwitch::resetAvailable() 
{
  RCSwitch::nReceivedValue = 0;
}

/*************************************************************************
 * @fn      getReceivedValue
 * @brief   获取接收到的数据.
 * @param   none       
 * @return  接收到的数据
 *************************************************************************/
uint32_t RCSwitch::getReceivedValue() 
{
  return RCSwitch::nReceivedValue;
}

/*************************************************************************
 * @fn      getReceivedBitlength
 * @brief   获取接收到的数据位数长度.
 * @param   none       
 * @return  数据长度
 *************************************************************************/
unsigned int RCSwitch::getReceivedBitlength() 
{
  return RCSwitch::nReceivedBitlength;
}

/*************************************************************************
 * @fn      getReceivedDelay
 * @brief   获取接收
 * @param   none       
 * @return  none
 *************************************************************************/
unsigned int RCSwitch::getReceivedDelay() 
{
  return RCSwitch::nReceivedDelay;
}

/*************************************************************************
 * @fn      getReceivedProtocol
 * @brief   获取接收
 * @param   none       
 * @return  none
 *************************************************************************/
unsigned int RCSwitch::getReceivedProtocol() 
{
  return RCSwitch::nReceivedProtocol;
}

/*************************************************************************
 * @fn      getReceivedRawdata
 * @brief   获取接收
 * @param   none       
 * @return  none
 *************************************************************************/
unsigned int* RCSwitch::getReceivedRawdata() 
{
  return RCSwitch::timings;
}


/*************************************************************************
 * @fn      diff
 * @brief   获取两个数相减的绝对值
 * @param   none       
 * @return  none
 *************************************************************************/
static inline unsigned int diff(int A, int B) 
{
  return abs(A - B);
}

/*************************************************************************
 * @fn      receiveProtocol
 * @brief   接收处理函数
 * @param   none       
 * @return  none
 *************************************************************************/
bool RCSwitch::receiveProtocol(const int p, unsigned int changeCount) 
{
	const Protocol &pro = recv_proto[p-1];
	uint32_t code = 0;

	//获取同步头中较长的脉宽
	const unsigned int syncLengthInPulses =  ((pro.syncFactor._low) > (pro.syncFactor._high)) ? (pro.syncFactor._low) : (pro.syncFactor._high);
	//得到同步头比值中一份比值的脉宽(比如对于DOOYA为：timings[0]/31)
	volatile unsigned int delay = RCSwitch::timings[0] / syncLengthInPulses;
  //设置容许误差，此处为delay*1
	volatile unsigned int delayTolerance = delay * ((float)RCSwitch::nReceiveTolerance / 100.0);
	//如果pro.invertedSignal为真，则交换所有传输中_high和_low的逻辑。
  const unsigned int firstDataTiming = (pro.invertedSignal) ? (2) : (1);

	for (unsigned int i = firstDataTiming; i < changeCount - 1; i += 2) {
		code <<= 1;
		if (diff(RCSwitch::timings[i], delay * pro.zero._high) < delayTolerance &&
			diff(RCSwitch::timings[i + 1], delay * pro.zero._low) < delayTolerance) {
			// zero

		} else if (diff(RCSwitch::timings[i], delay * pro.one._high) < delayTolerance &&
					diff(RCSwitch::timings[i + 1], delay * pro.one._low) < delayTolerance) {
			// one
			code |= 1;
		} else {
			// Failed
			return false;
		}
	}

	if (changeCount > 7) {    // ignore very short transmissions: no device sends them, so this must be noise
		RCSwitch::nReceivedValue = code;
		RCSwitch::nReceivedBitlength = (changeCount - 1) / 2;
		RCSwitch::nReceivedDelay = delay;
		RCSwitch::nReceivedProtocol = p;
		return true;
	}
	return false;
}


/*************************************************************************
 * @fn      handleInterrupt
 * @brief   接收中断函数
 * @param   none       
 * @return  none
 *************************************************************************/
void RCSwitch::handleInterrupt() 
{
	if(gpio_get_irq_status(nReceiverInterrupt))
	{
		Clear_GPIO_irq(nReceiverInterrupt);
		static unsigned int changeCount = 0;
		static unsigned long lastTime = 0;
		static unsigned int repeatCount = 0;
		const uint64_t tim = micros();
		const unsigned int duration = tim - lastTime;
		

		//长时间没有电平变化
		if (duration > RCSwitch::nSeparationLimit)
		{
			//如果当前这个长信号的长度和上次记录到的长信号值大致接近
			//这代表这个确实可能是同步头或者2个传输之间的间隙
			//发送方需要多次发送该信号，且两者之间间距大致相同
			if ((repeatCount==0) || (diff(duration, RCSwitch::timings[0]) < 200))
			{
				repeatCount++;
				if (repeatCount == 2) {
					for(unsigned int i = 1; i <= numRecvProto; i++) {
					  if (receiveProtocol(i, changeCount)) {
					    break;
					  }
					}
					repeatCount = 0;
				}
			}
      changeCount = 0;  
		}
	
		//数据溢出
		if (changeCount >= RCSWITCH_MAX_CHANGES) 
		{
			changeCount = 0;
			repeatCount = 0;
		}
		RCSwitch::timings[changeCount++] = duration;
		lastTime = tim;  
	}
}

#endif










