#include "led_breath.h"
#include "cmsis_os.h"

#define BREATH_TABLE_SIZE  1000U   /* 500 渐亮 + 500 渐暗 */
static uint16_t breath_table[BREATH_TABLE_SIZE];

/* 生成三角波查找表 */
static void Breath_Table_Generate(void)
{
    uint32_t i;
    for (i = 0; i < 500; i++)
    {
        breath_table[i] = (uint16_t)i;                      /* 渐亮 */
        breath_table[i + 500] = (uint16_t)(499 - i);       /* 渐暗 */
    }
}

static void Breath_DMA_Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    /* DMA1_Stream4（或 Stream5，看具体芯片）配置：内存到外设 */
    DMA1_Stream5->CR = 0;
    DMA1_Stream5->PAR = (uint32_t)&TIM3->CCR2;           /* 外设地址 */
    DMA1_Stream5->M0AR = (uint32_t)breath_table;          /* 内存地址 */
    DMA1_Stream5->NDTR = BREATH_TABLE_SIZE;               /* 传输次数 */
    DMA1_Stream5->CR |= DMA_SxCR_CHSEL_2 | DMA_SxCR_CHSEL_0;  // 置 bit27 + bit25                 /* 通道选择（查手册） */
    DMA1_Stream5->CR |= DMA_SxCR_MSIZE_0;                 /* 内存数据宽度：16bit */
    DMA1_Stream5->CR |= DMA_SxCR_PSIZE_0;                 /* 外设数据宽度：16bit */
    DMA1_Stream5->CR |= DMA_SxCR_MINC;                    /* 内存地址递增 */
    DMA1_Stream5->CR |= DMA_SxCR_CIRC;                    /* 循环模式 */
    DMA1_Stream5->CR |= DMA_SxCR_DIR_0;                   /* 方向：内存到外设 */
    DMA1_Stream5->CR |= DMA_SxCR_EN;                      /* 使能 DMA */

    /* TIM3 使能 DMA 更新请求 */
    TIM3->DIER |= TIM_DIER_CC2DE;
}

 void LED_Breath_Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	GPIOA->MODER &= ~(3U << (7U * 2U));
	GPIOA->MODER |=  (2U << (7U * 2U));   // ?? 10,??????
	
	GPIOA->OTYPER &= ~(1U << 7U);
	
	GPIOA->OSPEEDR &= ~(3U << (7U * 2U));
	GPIOA->OSPEEDR |= (2U << (7U * 2U));
	
	GPIOA->PUPDR &= ~(3U << (7U * 2U));
	
	GPIOA->AFR[0] &= ~(0xFU << (4U*7U));
	GPIOA->AFR[0] |= (0x2U << (4U*7U));
	
	  TIM3->CR1  = 0;
    TIM3->CCMR1 = 0;
    TIM3->CCER = 0;
	
    TIM3->PSC = 84U - 1U;
	  TIM3->ARR = 1000U - 1U;
    TIM3->CCR2 = 0U;
		
    TIM3->CCMR1 &= ~((3U << 8U) | (7U << 12U));
    TIM3->CCMR1 |=  (6U << 12U);
    TIM3->CCMR1 |=  TIM_CCMR1_OC2PE;
		
    /* The onboard LED is active-low: invert CH2 so a larger duty is brighter. */
    TIM3->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2P;
		
		TIM3->CR1 |= TIM_CR1_ARPE;
		
		TIM3->EGR = TIM_EGR_UG;
		
    Breath_Table_Generate();
    Breath_DMA_Init();

	TIM3->CR1 |= TIM_CR1_CEN;
}



void LED_PWM_SetDuty(uint16_t duty)
{
	if(duty > LED_PWM_MAX_DUTY)
		duty = LED_PWM_MAX_DUTY;
	if(duty < LED_PWM_MIN_DUTY)
		duty = LED_PWM_MIN_DUTY;
	TIM3->CCR2 = duty;
}


void LED_Breath_Update(uint32_t period_ms)
{
	uint32_t phase;
	uint32_t half_period;
	uint32_t duty;

	if (period_ms < 2U)
	{
		LED_PWM_SetDuty(LED_PWM_MIN_DUTY);
		return;
	}

	half_period = period_ms / 2U;
	phase = HAL_GetTick() % period_ms;

	if (phase < half_period)
	{
		/* First half: fade from off to full brightness. */
		duty = (uint32_t)LED_PWM_MAX_DUTY * phase / half_period;
	}
	else
	{
		/* Second half: fade from full brightness back to off. */
		duty = (uint32_t)LED_PWM_MAX_DUTY * (period_ms - phase)
			 / (period_ms - half_period);
	}

	LED_PWM_SetDuty((uint16_t)duty);
}
