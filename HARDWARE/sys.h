#ifndef __SYS_H
#define __SYS_H

#include "stdint.h"
#include "stm32f4xx_hal.h"

/* 类型兼容定义（让旧代码不用改） */
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

/* 有符号类型*/
typedef int8_t   s8;    // ← 新增
typedef int16_t  s16;   // ← 新增
typedef int32_t  s32;   // ← 新增 ← 这就是 touch.c 需要的！

/* Volatile 类型（新增！用于硬件寄存器访问）*/
typedef volatile uint8_t  vu8;    // ← 新增
typedef volatile uint16_t vu16;   // ← 新增（lcd.c 需要这个！）
typedef volatile uint32_t vu32;   // ← 新增

typedef volatile int8_t   vs8;    // ← 新增
typedef volatile int16_t  vs16;   // ← 新增
typedef volatile int32_t  vs32;   // ← 新增


/* Cortex-M4 位带操作宏 */
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr & 0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))

/* GPIO 输出位带地址 */
#define GPIOA_ODR_Addr    (GPIOA_BASE+20)
#define GPIOB_ODR_Addr    (GPIOB_BASE+20)
#define GPIOC_ODR_Addr    (GPIOC_BASE+20)
#define GPIOD_ODR_Addr    (GPIOD_BASE+20)
#define GPIOE_ODR_Addr    (GPIOE_BASE+20)
#define GPIOF_ODR_Addr    (GPIOF_BASE+20)   /* ← 新增 */

/* GPIO 输入位带地址 */
#define GPIOA_IDR_Addr    (GPIOA_BASE+16)
#define GPIOB_IDR_Addr    (GPIOB_BASE+16)
#define GPIOC_IDR_Addr    (GPIOC_BASE+16)
#define GPIOD_IDR_Addr    (GPIOD_BASE+16)
#define GPIOE_IDR_Addr    (GPIOE_BASE+16)
#define GPIOF_IDR_Addr    (GPIOF_BASE+16)   /* ← 新增 */

/* IO 口操作宏（和正点原子完全一致） */
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)
#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)
#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)
#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)
#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)   /* ← 新增（ctiic.c 需要！）*/

#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)

#endif /* __SYS_H */



