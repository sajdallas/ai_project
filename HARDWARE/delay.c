#include "delay.h"

/*
 * 基于DWT计数器的微秒延时函数
 * 
 * 注意：
 * - FreeRTOS已占用SysTick，所以不能用SysTick做延时
 * - DWT是Cortex-M4内置的32位循环计数器，独立于SysTick
 * - 精度：1个时钟周期（168MHz下约5.95ns）
 */

static uint8_t dwt_initialized = 0;

void delay_init(void)
{
    if (dwt_initialized) return;
    
    /* 使能DWT（数据观察点和跟踪单元） */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    
    /* 复位CYCCNT计数器 */
    DWT->CYCCNT = 0;
    
    /* 使能CPU周期计数 */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    
    dwt_initialized = 1;
}

/*
 * 微秒级延时
 * 参数: us - 延时的微秒数 (0 ~ 4294967, 约71分钟溢出)
 */
void delay_us(uint32_t us)
{
    uint32_t start, cycles;
    
    if (!dwt_initialized) {
        delay_init();
    }
    
    /* 计算需要的时钟周期数 */
    cycles = us * (SystemCoreClock / 1000000);
    
    /* 获取当前计数值 */
    start = DWT->CYCCNT;
    
    /* 等待直到达到目标计数值 */
    while ((DWT->CYCCNT - start) < cycles);
}

/*
 * 毫秒级延时
 * 参数: ms - 延时的毫秒数
 * 
 * 说明：这里用HAL_Delay替代，因为HAL_Delay在FreeRTOS环境下
 *       会自动调用osDelay，不会阻塞其他任务
 */
void delay_ms(uint16_t ms)
{
    HAL_Delay(ms);
}
