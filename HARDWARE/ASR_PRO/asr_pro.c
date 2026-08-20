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
    asr.last_rx_tick = HAL_GetTick();  /* 记录最后收字节的时刻 */

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

/* 解析收到的数据帧，填充 asr_msg_t
 * ASR PRO 仅支持离线命令词，串口发送单个 ASCII 数字 '1'~'7' */
static int ASR_Parse(asr_msg_t *msg)
{
    if (!asr.data_ready || asr.rx_len == 0) return 0;

    uint8_t first = asr.rx_buf[0];

    if (first >= '1' && first <= '9') {
        msg->cmd_id = (uint8_t)(first - '0');   /* ASCII '1' → 0x01 */
    } else {
        msg->cmd_id = 0;   /* 无效数据 */
    }

    /* 解析完毕，清空缓冲区，从头开始接收下一帧 */
    ASR_FlushRx();
    return 1;
}

/* 获取一帧数据（非阻塞），返回 1 表示有数据
 * 判帧策略：优先等换行符；若收不到换行符，超时 ASR_RX_TIMEOUT_MS 后也视为一帧结束 */
int ASR_GetMessage(asr_msg_t *msg)
{
    /* 超时判帧：有数据但没收到换行符，且超过 N ms 无新字节 */
    if (!asr.data_ready && asr.rx_len > 0) {
        if (HAL_GetTick() - asr.last_rx_tick >= ASR_RX_TIMEOUT_MS) {
            asr.rx_buf[asr.rx_len] = '\0';
            asr.data_ready = 1;
        }
    }
    return ASR_Parse(msg);
}