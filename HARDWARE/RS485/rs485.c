#include "sys.h"		    
#include "rs485.h"	 
#include "delay.h"
#include "stm32f4xx_hal.h"  // 新增：HAL 库头文件
#include "main.h"           // Error_Handler() 声明

//////////////////////////////////////////////////////////////////////////////////	 

//RS485 驱动（HAL 库版本）
//支持 ESP8266 WiFi 透传通信
//PA1: RS485 方向控制 (TX_EN)
//PA2: USART2_TX
//PA3: USART2_RX
////////////////////////////////////////////////////////////////////////////////// 	 

/* USART2 全局句柄 */
UART_HandleTypeDef huart2;

#if EN_USART2_RX   	
/* 接收缓冲区 */	
u8 RS485_RX_BUF[64];  	// 接收缓冲,最大64个字节.
u8 RS485_RX_CNT=0;     // 接收到的数据长度

/*
 * USART2 中断服务函数（HAL 库回调）
 * 注意：函数名必须是 USART2_IRQHandler，这是启动文件定义的
 */
void USART2_IRQHandler(void)
{
	u8 res;	    
	uint32_t isrflags   = READ_REG(huart2.Instance->SR);   // 读取状态寄存器
	uint32_t cr1its     = READ_REG(huart2.Instance->CR1);  // 读取控制寄存器
	
	/* 检查接收中断标志 */
	if (((isrflags & UART_FLAG_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
	{	 	
		res = (uint8_t)(huart2.Instance->DR & (uint8_t)0x00FF);  // 读取接收到的数据
		if(RS485_RX_CNT < 64)
		{
			RS485_RX_BUF[RS485_RX_CNT] = res;		// 记录接收到的值
			RS485_RX_CNT++;						// 接收数据增加1 
		} 
	}  											 
} 
#endif										 

/*
 * 初始化 IO 和 USART2
 * bound: 波特率
 */
void RS485_Init(u32 bound)
{  	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
    /* ====== HAL 库版本：时钟使能 ====== */
	__HAL_RCC_GPIOA_CLK_ENABLE();           // 使能 GPIOA 时钟
	__HAL_RCC_USART2_CLK_ENABLE();          // 使能 USART2 时钟

    /* ====== HAL 库版本：PA2(TX), PA3(RX) 配置为复用功能 ====== */
	GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;          // 复用推挽输出
	GPIO_InitStruct.Pull = GPIO_PULLUP;              // 上拉
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2;     // AF7: USART2 功能
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
    /* ====== HAL 库版本：PA1 (RS485 TX_EN) 配置为输出 ====== */
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;      // 推挽输出
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ====== HAL 库版本：USART2 参数配置 ====== */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = bound;                     // 波特率
	huart2.Init.WordLength = UART_WORDLENGTH_8B;      // 8位数据
	huart2.Init.StopBits = UART_STOPBITS_1;           // 1位停止位
	huart2.Init.Parity = UART_PARITY_NONE;            // 无校验
	huart2.Init.Mode = UART_MODE_TX_RX;               // 收发模式
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;           // 无硬件流控
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;  // 16倍过采样
	
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		/* 初始化错误处理 */
		Error_Handler();
	}
	
	/* 清除传输完成标志 */
	__HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_TC);
	
#if EN_USART2_RX
	/* 使能接收中断 */
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);

	/* ====== HAL 库版本：NVIC 中断配置 ====== */
	HAL_NVIC_SetPriority(USART2_IRQn, 3, 3);        // 抢占优先级3，子优先级3
	HAL_NVIC_EnableIRQ(USART2_IRQn);                 // 使能 USART2 中断
#endif	
	
	RS485_TX_EN = 0;			// 默认为接收模式
}

/*
 * RS485发送len个字节.
 * buf: 发送区首地址
 * len: 发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
 */
void RS485_Send_Data(u8 *buf, u8 len)
{
	RS485_TX_EN = 1;			// 设置为发送模式
	delay_ms(10);              // 等待模式切换稳定
	
    /* ====== HAL 库版本：阻塞式发送 ====== */
	HAL_UART_Transmit(&huart2, buf, len, 1000);       // 超时时间1000ms
	
	/* 等待发送完成 */
	while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) == RESET);
	
	RS485_RX_CNT = 0;	  
	RS485_TX_EN = 0;				// 设置为接收模式
}

/*
 * RS485查询接收到的数据
 * buf: 接收缓存首地址
 * len: 接收的数据长度
 */
void RS485_Receive_Data(u8 *buf, u8 *len)
{
	u8 rxlen = RS485_RX_CNT;
	u8 i = 0;
	*len = 0;				// 默认为0
	delay_ms(10);		// 等待10ms,如果连续10ms没有接收到一个数据,则认为接收结束
	if(rxlen == RS485_RX_CNT && rxlen)// 接收到了数据,且接收完成了
	{
		for(i = 0; i < rxlen; i++)
		{
			buf[i] = RS485_RX_BUF[i];	
		}		
		*len = RS485_RX_CNT;	// 记录本次数据长度
		RS485_RX_CNT = 0;		// 清零
	}
}
