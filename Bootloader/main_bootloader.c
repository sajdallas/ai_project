#include "main.h"
#include "gpio.h"
#include "dma.h"
#include "sdio.h"
#include "bootloader.h"
#include "ota_config.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

void SystemClock_Config(void);
static void boot_report_status(boot_status_t st);
static uint8_t boot_update_key_pressed(void);
static void boot_iap_loop(const char *reason);

void boot_log(const char *msg)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);
}

void boot_log_hex(const char *prefix, uint32_t value)
{
    char buf[48];
    static const char hex[] = "0123456789ABCDEF";
    size_t len = strlen(prefix);
    size_t i;

    if (len > sizeof(buf) - 14U) {
        len = sizeof(buf) - 14U;
    }

    memcpy(buf, prefix, len);
    buf[len++] = '0';
    buf[len++] = 'x';
    for (i = 0; i < 8U; i++) {
        buf[len++] = hex[(value >> (28U - (i * 4U))) & 0x0FU];
    }
    buf[len++] = '\r';
    buf[len++] = '\n';
    buf[len] = '\0';
    boot_log(buf);
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_DMA_Init();
	
		MX_USART1_UART_Init();
		boot_log("\r\n[BOOT] start\r\n");
    MX_USART3_UART_Init();
    boot_log("[BOOT] usart3 ok\r\n");
	
    boot_log("[BOOT] before sdio init\r\n");
		//MX_SDIO_SD_Init();
		boot_log("[BOOT] after sdio init\r\n");

    boot_log_hex("[BOOT] app stack=", *(volatile uint32_t *)OTA_APP_BASE);
    boot_log_hex("[BOOT] app reset=", *(volatile uint32_t *)(OTA_APP_BASE + 4U));
    if (boot_is_valid_app(OTA_APP_BASE)) {
        boot_log("[BOOT] app valid before update\r\n");
        if (!boot_update_key_pressed()) {
            boot_log("[BOOT] key released, jump app\r\n");
            HAL_Delay(50);
            boot_jump_to_app(OTA_APP_BASE);
        }
        boot_log("[BOOT] key pressed, enter iap\r\n");
        boot_iap_loop("[BOOT] key iap mode\r\n");
    } else {
        boot_log("[BOOT] app invalid before update\r\n");
    }

    boot_iap_loop("[BOOT] no valid app, recovery mode\r\n");
}

static void boot_iap_loop(const char *reason)
{
    boot_status_t st;

    boot_log(reason);

    while (1) {
        boot_log("[BOOT] iap uart\r\n");
        st = boot_try_update_from_uart(5000U);
        if (st == BOOT_NO_PACKAGE) {
            boot_log("[BOOT] iap wifi\r\n");
            st = boot_try_update_from_esp(60000U);
        }
        boot_report_status(st);

        if (boot_is_valid_app(OTA_APP_BASE)) {
            boot_log("[BOOT] iap jump app\r\n");
            HAL_Delay(50);
            boot_jump_to_app(OTA_APP_BASE);
        }

        boot_log("[BOOT] iap wait retry\r\n");
        HAL_Delay(1000);
    }
}

static uint8_t boot_update_key_pressed(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    gpio.Pin = GPIO_PIN_0;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLDOWN;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpio);

    HAL_Delay(20);
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) {
        HAL_Delay(20);
        return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET;
    }

    return 0U;
}

static void boot_report_status(boot_status_t st)
{
    switch (st) {
    case BOOT_OK:
        boot_log("[BOOT] update ok\r\n");
        break;
    case BOOT_NO_PACKAGE:
        boot_log("[BOOT] no package\r\n");
        break;
    case BOOT_BAD_PACKAGE:
        boot_log("[BOOT] bad package\r\n");
        break;
    case BOOT_SD_ERROR:
        boot_log("[BOOT] sd error\r\n");
        break;
    case BOOT_FLASH_ERROR:
        boot_log("[BOOT] flash error\r\n");
        break;
    case BOOT_VERIFY_ERROR:
        boot_log("[BOOT] verify error\r\n");
        break;
    default:
        boot_log("[BOOT] unknown error\r\n");
        break;
    }
}
