#include "myiic.h"
#include "delay.h"

/*
 * IIC 软件模拟驱动（HAL 库版本）
 * 
 * 引脚定义：
 *   SCL: PB8
 *   SDA: PB9
 */

void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    /* PB8(SCL), PB9(SDA) 配置为开漏输出 + 上拉 */
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;   // 开漏输出
    GPIO_InitStruct.Pull = GPIO_PULLUP;            // 内部上拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    IIC_SDA = 1;
    IIC_SCL = 1;
}

/* 产生 IIC 起始信号 */
void IIC_Start(void)
{
    SDA_OUT();     // SDA 线输出
    IIC_SDA = 1;
    IIC_SCL = 1;
    delay_us(4);
    IIC_SDA = 0;   // START: when CLK is high, DATA change form high to low
    delay_us(4);
    IIC_SCL = 0;   // 钳住 I2C 总线，准备发送或接收数据
}

/* 产生 IIC 停止信号 */
void IIC_Stop(void)
{
    SDA_OUT();     // SDA 线输出
    IIC_SCL = 0;
    IIC_SDA = 0;   // STOP: when CLK is high DATA change form low to high
    delay_us(4);
    IIC_SCL = 1;
    delay_us(4);
    IIC_SDA = 1;   // 发送 I2C 总线结束信号
}

/* 等待应答信号到来 */
/* 返回值: 1 接收应答失败 */
/*         0 接收应答成功 */
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;
    
    SDA_IN();      // SDA 设置为输入
    IIC_SDA = 1;
    delay_us(1);
    IIC_SCL = 1;
    delay_us(1);
    
    while (PBin(9))  // 检测 PB9 (SDA) 电平
    {
        ucErrTime++;
        if (ucErrTime > 250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL = 0; // 时钟输出 0
    return 0;
}

/* 产生 ACK 应答 */
void IIC_Ack(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 0;
    delay_us(2);
    IIC_SCL = 1;
    delay_us(2);
    IIC_SCL = 0;
}

/* 不产生 ACK 应答 */
void IIC_Nack(void)   // 统一为小写 nack，兼容 24cxx.c 的调用
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 1;
    delay_us(2);
    IIC_SCL = 1;
    delay_us(2);
    IIC_SCL = 0;
}

/* IIC 发送一个字节 */
/* 返回从机有无应答 */
/* 1 有应答 */
/* 0 无应答 */
void IIC_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL = 0; // 拉低时钟开始数据传输
    
    for (t = 0; t < 8; t++)
    {
        IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1;
        delay_us(2);   // 对 TEA5767 这三个延时都是必须的
        IIC_SCL = 1;
        delay_us(2);
        IIC_SCL = 0;
        delay_us(2);
    }
}

/* 读 1 个字节，ack=1 时，发送 ACK，ack=0，发送 nACK */
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    SDA_IN();  // SDA 设置为输入
    
    for (i = 0; i < 8; i++)
    {
        IIC_SCL = 0;
        delay_us(2);
        IIC_SCL = 1;
        receive <<= 1;
        if (PBin(9))  // 读取 PB9 (SDA)
        {
            receive++;
        }
        delay_us(1);
    }
    if (!ack)
        IIC_Nack();  // 发送 nACK
    else
        IIC_Ack();   // 发送 ACK
   
    return receive;
}
