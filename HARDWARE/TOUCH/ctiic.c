#include "ctiic.h"
#include "sys.h"       // 显式包含，确保使用最新版本
#include "delay.h"

/**
 * ģ�� I2C ������STM32F4 �汾��
 * Ӳ�����ţ�SCL = PB0, SDA = PF11
 * ����ʱ��ʹ��������ʱ���ƣ������ڱ�׼ 100kHz/400kHz ģʽ
 */

// ============================================================
// �ײ���ʱ�����ſ���
// ============================================================

/**
 * @brief I2C ʱ����ʱ��Լ 2us��
 * @note ������Ƶ���� delay_us �Ĳ�����ƥ������
 */
void CT_Delay(void)
{
    delay_us(2);
}

/**
 * @brief ��ʼ�� I2C ���ţ�PB0 �� PF11��
 * @note ����Ϊ��©������ڲ�����
 */
void CT_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();

    /* PB0(SCL), PF11(SDA) ����Ϊ��©��� + ���� */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    CT_IIC_SCL = 1;
    CT_IIC_SDA = 1;
}

// ============================================================
// I2C ��ʼ/ֹͣ����
// ============================================================

/**
 * @brief ���� I2C ��ʼ����
 * @note SCL �ߵ�ƽʱ��SDA �ɸ߱��
 */
void CT_IIC_Start(void)
{
    CT_SDA_OUT();          // SDA ��Ϊ���
    CT_IIC_SDA = 1;
    CT_IIC_SCL = 1;
    delay_us(30);
    CT_IIC_SDA = 0;        // ��ʼ������SCL �ߣ�SDA �½���
    CT_Delay();
    CT_IIC_SCL = 0;        // ǯס���ߣ�׼����������
}

/**
 * @brief ���� I2C ֹͣ����
 * @note SCL �ߵ�ƽʱ��SDA �ɵͱ��
 */
void CT_IIC_Stop(void)
{
    CT_SDA_OUT();          // SDA ��Ϊ���
    CT_IIC_SCL = 1;
    delay_us(30);
    CT_IIC_SDA = 0;        // SDA ������
    CT_Delay();
    CT_IIC_SDA = 1;        // ֹͣ������SCL �ߣ�SDA ������
}

// ============================================================
// Ӧ���źŴ���
// ============================================================

/**
 * @brief �ȴ����豸Ӧ���ź�
 * @return 0: �յ�Ӧ��SDA �����ͣ�; 1: Ӧ��ʱ
 */
uint8_t CT_IIC_Wait_Ack(void)
{
    uint8_t ucErrTime = 0;
    CT_SDA_IN();           // SDA ��Ϊ����
    CT_IIC_SDA = 1;
    CT_IIC_SCL = 1;
    CT_Delay();
    while (CT_READ_SDA) {  // ��� SDA ��ƽ
        ucErrTime++;
        if (ucErrTime > 250) {
            CT_IIC_Stop();
            return 1;
        }
        CT_Delay();
    }
    CT_IIC_SCL = 0;        // ����ʱ�ӣ����Ӧ��λ
    return 0;
}

/**
 * @brief ���� ACK Ӧ��SDA ���ͣ�
 */
void CT_IIC_Ack(void)
{
    CT_IIC_SCL = 0;
    CT_SDA_OUT();
    CT_Delay();
    CT_IIC_SDA = 0;
    CT_Delay();
    CT_IIC_SCL = 1;
    CT_Delay();
    CT_IIC_SCL = 0;
}

/**
 * @brief ���� NACK ��Ӧ��SDA ���ߣ�
 */
void CT_IIC_NAck(void)
{
    CT_IIC_SCL = 0;
    CT_SDA_OUT();
    CT_Delay();
    CT_IIC_SDA = 1;
    CT_Delay();
    CT_IIC_SCL = 1;
    CT_Delay();
    CT_IIC_SCL = 0;
}

// ============================================================
// ���ݷ��������
// ============================================================

/**
 * @brief ����һ���ֽ�����
 * @param txd Ҫ���͵��ֽ�
 * @note ������ɺ󣬴��豸�᷵��Ӧ������� CT_IIC_Wait_Ack ���
 */
void CT_IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
    CT_SDA_OUT();
    CT_IIC_SCL = 0;        // ����ʱ�ӣ�׼������
    CT_Delay();
    for (t = 0; t < 8; t++) {
        CT_IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1;
        CT_IIC_SCL = 1;
        CT_Delay();
        CT_IIC_SCL = 0;
        CT_Delay();
    }
}

/**
 * @brief ��ȡһ���ֽ�����
 * @param ack 1: ���� ACK ������ȡ; 0: ���� NACK ������ȡ
 * @return ��ȡ�����ֽ�
 */
uint8_t CT_IIC_Read_Byte(uint8_t ack)
{
    uint8_t i, receive = 0;
    CT_SDA_IN();           // SDA ��Ϊ����
    delay_us(30);
    for (i = 0; i < 8; i++) {
        CT_IIC_SCL = 0;
        CT_Delay();
        CT_IIC_SCL = 1;
        receive <<= 1;
        if (CT_READ_SDA)
            receive++;
    }
    if (!ack)
        CT_IIC_NAck();     // ���� NACK ������ȡ
    else
        CT_IIC_Ack();      // ���� ACK ������ȡ
    return receive;
}
