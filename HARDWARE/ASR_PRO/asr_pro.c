#include "asr_pro.h"
#include "cmsis_os.h"

static asr_t asr;

/* 将 rx 缓冲区指针复位到起始位置，重新开始接收 */
static void ASR_FlushRx(void)
{
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
    __HAL_UART_CLEAR_OREFLAG(&huart1);
    huart1.RxState = HAL_UART_STATE_READY;
    asr.rx_len = 0;
    asr.data_ready = 0;
    memset(asr.rx_buf, 0, ASR_RX_BUF_SIZE);
    HAL_UART_Receive_IT(&huart1, &asr.rx_buf[0], 1);
}

/* 初始化 ASR PRO 驱动 */
void ASR_Init(void)
{
    memset(&asr, 0, sizeof(asr));
    HAL_UART_Receive_IT(&huart1, &asr.rx_buf[0], 1);
}

/* UART 中断回调：每收到一个字节调用一次 */
void ASR_RxCallback(void)
{
    uint8_t ch = asr.rx_buf[asr.rx_len];

    /* 收到换行符 0x0A 或回车 0x0D 表示一帧结束 */
    if (ch == 0x0A || ch == 0x0D) {
        if (asr.rx_len > 0) {
            asr.rx_buf[asr.rx_len] = '\0';  /* 用 NUL 结尾 */
            asr.data_ready = 1;
        }
        asr.rx_len++;
        if (asr.rx_len < ASR_RX_BUF_SIZE - 1) {
            HAL_UART_Receive_IT(&huart1, &asr.rx_buf[asr.rx_len], 1);
        }
        return;
    }

    if (asr.rx_len < ASR_RX_BUF_SIZE - 1) {
        HAL_UART_Receive_IT(&huart1, &asr.rx_buf[asr.rx_len + 1], 1);
        asr.rx_len++;
    }
}

/* 解析收到的数据帧，填充 asr_msg_t */
static int ASR_Parse(asr_msg_t *msg)
{
    if (!asr.data_ready || asr.rx_len == 0) return 0;

    /* 按命令 ID 模式解析：第一字节为命令 ID */
    uint8_t cmd = asr.rx_buf[0];

    if (cmd >= 0x01 && cmd <= 0x0F) {
        msg->type = ASR_TYPE_COMMAND;
        msg->cmd_id = cmd;
    } else {
        /* 其余情况当作文字内容 */
        msg->type = ASR_TYPE_TEXT;
        snprintf(msg->text, sizeof(msg->text), "%s", (char*)asr.rx_buf);
    }

    asr.data_ready = 0;
    return 1;
}

/* 获取一帧数据（非阻塞），返回 1 表示有数据 */
int ASR_GetMessage(asr_msg_t *msg)
{
    return ASR_Parse(msg);
}