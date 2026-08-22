#ifndef __ASR_PRO_H
#define __ASR_PRO_H

#include "main.h"
#include "usart.h"
#include <string.h>

/* ASR PRO 接收缓冲区大小 */
#define ASR_RX_BUF_SIZE    512

/* 语音命令 ID 定义 — 与 ASR PRO 上位机配置的 ID 严格对应
 * 上位机 ID 1 → 打印 "1" → STM32 解析为 cmd_id=0x01 → 对应宏
 * 以此类推，ID 1~7 直通 */
#define ASR_CMD_MAIN_MENU    0x01   /* 打开菜单 */
#define ASR_CMD_IDLE         0x02   /* 待机画面（表情） */
#define ASR_CMD_STATUS       0x03   /* 系统状态 */
#define ASR_CMD_AI_CHAT      0x04   /* AI对话 */
#define ASR_CMD_BACK         0x05   /* 返回 */
#define ASR_CMD_SETTINGS     0x06   /* 设置 */
#define ASR_CMD_CLOCK        0x07   /* 时钟 */
#define ASR_CMD_CALENDAR     0x08   /* 日历 */

/* 语音识别消息（仅离线命令 ID，无语音转文字功能） */
typedef struct {
    uint8_t cmd_id;             /* 命令 ID 1~7 */
} asr_msg_t;

/* 超时判帧：如果收到数据后 N ms 内没有新字节且没有换行符，也视为一帧结束 */
#define ASR_RX_TIMEOUT_MS   20

/* ASR PRO 驱动结构体 */
typedef struct {
    uint8_t rx_buf[ASR_RX_BUF_SIZE];
    volatile uint16_t rx_len;
    volatile uint8_t data_ready;   /* 1 = 收到一帧完整数据 */
    volatile uint32_t last_rx_tick; /* 最后一次收到字节的时刻 */
} asr_t;

/* 函数声明 */
void ASR_Init(void);
void ASR_RxCallback(void);
int ASR_GetMessage(asr_msg_t *msg);
void ASR_ProcessTask(void *argument);

#endif /* __ASR_PRO_H */
