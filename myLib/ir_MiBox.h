/**
 * @file ir_MiBox.h
 * @brief 红外发送MiBox码的头文件
 * @details 支持MiBox协议发送
 * @version 0.1
 * @date 2022-07-02
 * @author hhdd
 * @copyright Copyright (c) 2021 TWen51 Technology Co., Ltd.
 * 
 */
#ifndef _IRSENDREV_H_
#define _IRSENDREV_H_

#include "asr.h"
#ifndef TW_ASR_PRO
#include "asr_delay.h"
#endif

/******************** 红外发射采样时间宏定义, 用户不要随意修改  *******************/
//asr的时钟无法取整，所以延时函数误差会比较大
#if defined(TW_ASR_PRO)
#define     MiBOX_HDR_MARK	        1085    //引导码高电平
#define     MiBOX_HDR_SPACE	        595     //引导码低电平
#define     MiBOX_BIT_MARK	        650
#define     MiBOX_ZERO_SPACE	    642     //"0"码    |-600±200-|-570±150-|
#define     MiBOX_ONE_SPACE	        976     //"1"码    |-600±200-|-870±150-|
#define     MiBOX_TWO_SPACE	        1302    //"2"码    |-600±200-|-1170±150-|
#define     MiBOX_THREE_SPACE	    1657    //"3"码    |-600±200-|-1470±150-|
#else
#define     MiBOX_HDR_MARK	        1080    //引导码高电平
#define     MiBOX_HDR_SPACE	        580     //引导码低电平
#define     MiBOX_BIT_MARK	        600
#define     MiBOX_ZERO_SPACE	    570     //"0"码    |-600±200-|-570±150-|
#define     MiBOX_ONE_SPACE	        870     //"1"码    |-600±200-|-870±150-|
#define     MiBOX_TWO_SPACE	        1170    //"2"码    |-600±200-|-1170±150-|
#define     MiBOX_THREE_SPACE	    1470    //"3"码    |-600±200-|-1470±150-|
#endif

#define     MiBOX_POWER_KEY             0x3CCC  //电源键
#define     MiBOX_POWER_KEY_PARITY      0x0F
#define     MiBOX_UP_KEY                0x8605  //上键
#define     MiBOX_UP_KEY_PARITY         0x0B
#define     MiBOX_DOWN_KEY              0x8606  //下键
#define     MiBOX_DOWN_KEY_PARITY       0x08
#define     MiBOX_LEFT_KEY              0x860B  //左键
#define     MiBOX_LEFT_KEY_PARITY       0x05
#define     MiBOX_RIGHT_KEY             0x860C  //右键
#define     MiBOX_RIGHT_KEY_PARITY      0x02
#define     MiBOX_CONFIRM_KEY           0x860D  //确认键
#define     MiBOX_CONFIRM_KEY_PARITY    0x03
#define     MiBOX_HOMEPAGE_KEY          0x8608  //主页键
#define     MiBOX_HOMEPAGE_KEY_PARITY   0x06
#define     MiBOX_GOBACK_KEY            0x8607  //返回键
#define     MiBOX_GOBACK_KEY_PARITY     0x09
#define     MiBOX_MENU_KEY              0x8604  //菜单键
#define     MiBOX_MENU_KEY_PARITY       0x0A
#define     MiBOX_VOL_UP_KEY           0x860E  //音量加键
#define     MiBOX_VOL_UP_KEY_PARITY    0x00
#define     MiBOX_VOL_DOWN_KEY          0x860F  //音量减键
#define     MiBOX_VOL_IDOWN_KEY_PARITY  0x01

class IRMiBox
{
private: 
    pwm_base_t ir_send_channel;
    void mark(uint32_t usec);
    void space(uint32_t usec);
	void enableIROut(uint32_t hz);
public:
    void begin(pwm_base_t channel);  //红外发送初始化
    void sendRaw(uint32_t buf[], uint32_t len, uint32_t hz);  
    void send_MiBox(uint16_t data_code);    //红外单次发送
    void send_MiBox_repeat(uint16_t data_code, uint8_t t); //红外循环多次发送
};

/*************************************************   红外发射  *********************************************************/
void IRMiBox::begin(pwm_base_t channel)
{
    ir_send_channel = channel;
}

//========================================================================
// 描述: 红外发射使能.
// 参数: none
// 返回: none.
//========================================================================
void IRMiBox::enableIROut(uint32_t hz) 
{
   PWM_enble(ir_send_channel, hz, 1000, 0); 
}

//========================================================================
// 描述: 红外发射载波信号.
// 参数: time:发射的时间.
// 返回: none.
//========================================================================
void IRMiBox::mark(uint32_t time) 
{
    pwm_set_duty(ir_send_channel, 330, 1000);
    delayMicroseconds(time);
}

//========================================================================
// 描述: 红外发射空闲信号.
// 参数: time:发射的时间.
// 返回: none.
//========================================================================
void IRMiBox::space(uint32_t time) 
{
    pwm_set_duty(ir_send_channel, 1000, 1000);
    delayMicroseconds(time);
}

//========================================================================
// 描述: 红外发射
// 参数: buf: 红外发送数组(buf[0]为发送空闲信号时间，buf[1]为发射载波信号时间，依次类推); 
//        len: 发送的长度;
//         hz:  载波频率.
// 返回: none.
//========================================================================
void IRMiBox::sendRaw(uint32_t buf[], uint32_t len, uint32_t hz)
{
    enableIROut(hz);
    for(uint32_t i = 0; i < len; i++)
    {
        if(i & 1){
            space(buf[i]);
        }else{
            mark(buf[i]);
        }
    }
    space(0);
}

//========================================================================
// 描述: 红外发射
// 参数: data_code:数据码.
// 返回: none.
//========================================================================
void IRMiBox::send_MiBox(uint16_t data_code)
{
    uint8_t parity_code = 0;

    //获取校验码
    parity_code =  ((data_code&0xf000)>>12)^((data_code&0x0f00)>>8)^   \
                        ((data_code&0x00f0)>>4)^(data_code&0x000f);       

    enableIROut(38000);

    taskENTER_CRITICAL();
    // 1. Lead code
    mark(MiBOX_HDR_MARK);
    space(MiBOX_HDR_SPACE);

    // 2. Data code
    for (uint8_t i=0;i<8;i++){
        switch (data_code & 0xc000)
        {
            case 0x0000:
                mark(MiBOX_BIT_MARK);
                space(MiBOX_ZERO_SPACE);
                break;
            case 0x4000:
                mark(MiBOX_BIT_MARK);
                space(MiBOX_ONE_SPACE);
                break;
            case 0x8000:
                mark(MiBOX_BIT_MARK);
                space(MiBOX_TWO_SPACE);
                break;
            case 0xc000:
                mark(MiBOX_BIT_MARK);
                space(MiBOX_THREE_SPACE);
                break;
        }
        data_code <<= 2;
    }

    // 3. Parity code
    for (uint8_t i=0;i<2;i++)
    {
        switch (parity_code & 0x0c)
        {
            case 0x00:
                mark(MiBOX_BIT_MARK);
                space(MiBOX_ZERO_SPACE);
                break;
            case 0x04:
                mark(MiBOX_BIT_MARK);
                space(MiBOX_ONE_SPACE);
                break;
            case 0x08:
                mark(MiBOX_BIT_MARK);
                space(MiBOX_TWO_SPACE);
                break;
            case 0x0c:
                mark(MiBOX_BIT_MARK);
                space(MiBOX_THREE_SPACE);
                break;
        }
        parity_code <<= 2;
    }
    mark(MiBOX_BIT_MARK);
    space(1);
    taskEXIT_CRITICAL();
}

//========================================================================
// 描述: 多次红外发射
// 参数: data_code:数据码.
// 返回: none.
//========================================================================
void IRMiBox::send_MiBox_repeat(uint16_t data_code, uint8_t t)
{
    for(int i=0;i < t;i++){
        send_MiBox(data_code);
        delay(11);
    }
}


#endif


