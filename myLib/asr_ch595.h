#ifndef _ASR_CH595_H
#define _ASR_CH595_H

#include "asr.h"

#ifndef         HC595_DS
#define 		HC595_DS            PB_0        //SI
#endif

#ifndef         _HC595_DS_MODE
#define         _HC595_DS_MODE    {set_pin_to_gpio_mode(HC595_DS);pinMode(HC595_DS,output);}	//推挽输出
#endif

#ifndef         HC595_STCP
#define 		HC595_STCP           PB_1       //RCK
#endif

#ifndef         _HC595_STCP_MODE 
#define         _HC595_STCP_MODE   {set_pin_to_gpio_mode(HC595_STCP);pinMode(HC595_STCP,output);}//推挽输出
#endif

#ifndef         HC595_SHCP
#define 		HC595_SHCP           PB_2        //SCK
#endif  

#ifndef         _HC595_SHCP_MODE
#define         _HC595_SHCP_MODE   {set_pin_to_gpio_mode(HC595_SHCP);pinMode(HC595_SHCP,output);}//推挽输出
#endif

#define         HC595_DS_HL(X)       digitalWrite(HC595_DS,(X))
#define         HC595_STCP_HL(X)     digitalWrite(HC595_STCP,(X))
#define         HC595_SHCP_HL(X)     digitalWrite(HC595_SHCP,(X))

#ifndef         hc595_num
#define         hc595_num       10
#endif

static uint8_t _595_hdata[hc595_num];

// 描述: 595使能输出.V
void hc595_out_enable()
{
    HC595_STCP_HL(0);
    delay5us();
    HC595_STCP_HL(1);
    delay5us();
    HC595_STCP_HL(0);
}

// 描述: 595发送8位数据.
void hc595_send_byte(uint8_t outdata)
{
    uint8_t i;

    for(i=0;i<8;i++)  //将8位数据按位发送，先发高字节再发低字节
	{      
        if(outdata&0x80)
	    {
            HC595_DS_HL(1);
        }
	    else
	    {
            HC595_DS_HL(0);
        }
        delay5us();
        HC595_SHCP_HL(0); //时钟线低电平
        delay5us();
        HC595_SHCP_HL(1); //时钟线高电平
        outdata = outdata<<1;
    }
}

// 描述: 595初始化引脚.
void _HC595_Init(void)
{
 	_HC595_DS_MODE;        //引脚初始化
    _HC595_STCP_MODE;
    _HC595_SHCP_MODE;

    HC595_DS_HL(0);
	HC595_STCP_HL(0);
	HC595_SHCP_HL(0);
}

// 描述: 595更新缓存数组.
void HC595_send_gx_byte()
{
    uint8_t i;
    for(i=hc595_num;i>0;i--)
	{
        hc595_send_byte(_595_hdata[i-1]); //依次更新缓存数据
    }
    hc595_out_enable();
}

//所有端口清空
void _HC595_disable()
{
    uint8_t i;
    for(i=hc595_num;i>0;i--)
	{
        _595_hdata[i-1] = (0x00);
    }
    HC595_send_gx_byte();
}

//595发送8位数据到缓存并更新     pois(1-hc595_num) 
void _HC595_Send_Updata(uint8_t pois,uint8_t _data)
{
    if(pois <= hc595_num)
    {
         _595_hdata[pois] = _data;
    }
    HC595_send_gx_byte();
}

//595发送数组.
void _HC595_Send_arr(uint8_t *ddata,uint8_t len)
{
    uint8_t i;
    if(len > hc595_num) len = hc595_num;
    for(i=0;i<len;i++)
    {
        _595_hdata[i] = *(ddata++);
    }
    HC595_send_gx_byte();
}

// 595发送指定端口指定1位数据.
void _HC595_Send_Bit(uint8_t Port,uint8_t pois,uint8_t TF)
{
    if(Port <= hc595_num)
    {
        if(TF == 0)
        {
            _595_hdata[Port] &=~ (1<<(pois)); //将Port的第poisi位清0
            HC595_send_gx_byte();
        }
        else
        {
            _595_hdata[Port] |= (1<<(pois)); //将Port的第pois位清0
            HC595_send_gx_byte();     
        }
    }
}
void _HC595_Send_x_y(uint8_t x,uint8_t y,uint8_t TF)
{
    // 计算目标位所在的字节位置及其在字节内的偏移
    uint8_t byteIndex = x / 8;
    uint8_t bitOffsetInByte = x % 8;

    // 计算要影响的最后一个字节位置及其在字节内的偏移
    uint8_t endByteIndex = y  / 8;
    uint8_t endBitOffsetInByte = y % 8;

    uint8_t i;
    // 确保位索引在有效范围内
    if (x >= 8 * hc595_num || y >= 8 * hc595_num || x > y) 
    {
        return;
    }




    if(byteIndex == endByteIndex)
    {

        for (i = bitOffsetInByte; i <= endBitOffsetInByte; i++)
        {
            if(TF)
            {
                _595_hdata[byteIndex] |= (1<<(i)); //
            }
            else
            {
                _595_hdata[byteIndex] &=~ (1<<(i)); //
            }
            
        }
    }
    else
    {
        for (i = bitOffsetInByte; i < 8; i++) 
        {
            if (TF) {
                _595_hdata[byteIndex] |= (1 << i);
            } else {
                _595_hdata[byteIndex] &= ~(1 << i);
            }
        }

        for (i = byteIndex + 1; i < endByteIndex; i++) 
        {
            _595_hdata[i] = TF ? 0xFF : 0x00;
        }

        for (i = 0; i <= endBitOffsetInByte; i++) 
        {
            if (TF) {
                _595_hdata[endByteIndex] |= (1 << i);
            } else {
                _595_hdata[endByteIndex] &= ~(1 << i);
            }
        } 
    }
    HC595_send_gx_byte();   
}


#endif