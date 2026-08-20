#ifndef __ASR_PRO_H
#define __ASR_PRO_H

#include "main.h"
#include "usart.h"
#include <string.h>

/* ASR PRO 接收缓冲区大小 */
#define ASR_RX_BUF_SIZE    512

/* 语音命令 ID 定义（在 ASR PRO 上位机软件中配置） */
#define ASR_CMD_MAIN_MENU    0x01   /* 打开菜单 */
#define ASR_CMD_STATUS       0x02   /* 系统状态 */
#define ASR_CMD_AI_CHAT      0x03   /* AI对话 */
#define ASR_CMD_BACK         0x04   /* 返回 */
#define ASR_CMD_SEND         0x05   /* 发送 */
#define ASR_CMD_IDLE         0x06   /* 待机画面 */
#define ASR_CMD_SETTINGS     0x07   /* 设置 */

/* 语音识别结果类型 */
typedef enum {
    ASR_TYPE_COMMAND = 0,    /* 命令 ID */
    ASR_TYPE_TEXT,           /* 文字内容（用于 AI 对话输入） */
} asr_msg_type_t;

/* 语音识别消息 */
#pragma anon_unions
typedef struct {
    asr_msg_type_t type;
    union {
        uint8_t cmd_id;             /* 命令 ID */
        char text[ASR_RX_BUF_SIZE]; /* 识别文字 */
    };
} asr_msg_t;

/* ASR PRO 驱动结构体 */
typedef struct {
    uint8_t rx_buf[ASR_RX_BUF_SIZE];
    volatile uint16_t rx_len;
    volatile uint8_t data_ready;   /* 1 = 收到一帧完整数据 */
} asr_t;

/* 函数声明 */
void ASR_Init(void);
void ASR_RxCallback(void);
void ASR_ProcessTask(void *argument);

#endif /* __ASR_PRO_H */