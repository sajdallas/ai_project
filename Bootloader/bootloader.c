#include "bootloader.h"

#include "main.h"
#include "ff.h"
#include "ota_config.h"
#include "usart.h"

#include <stdio.h>
#include <string.h>

#define BOOT_READ_CHUNK 1024U

extern void boot_log(const char *msg);
extern void boot_log_hex(const char *prefix, uint32_t value);

static FATFS s_fs;
static FIL s_file;
static uint8_t s_buf[BOOT_READ_CHUNK];
static uint8_t s_esp_link_id = 0;

static const char *const s_package_paths[] = {
    OTA_PACKAGE_PATH,
    "0:/Update/App.bin",
    "0:/UPDATE/APP.BIN",
    "0:/update/app.bin",
    "0:/app.bin",
    "0:/App.bin",
    "0:/APP.BIN"
};

static uint32_t crc32_update(uint32_t crc, const uint8_t *data, uint32_t len)
{
    uint32_t i;
    uint32_t bit;

    crc = ~crc;
    for (i = 0; i < len; i++) {
        crc ^= data[i];
        for (bit = 0; bit < 8; bit++) {
            crc = (crc & 1U) ? ((crc >> 1) ^ 0xEDB88320UL) : (crc >> 1);
        }
    }
    return ~crc;
}

static uint8_t uart_wait_iap_command(uint32_t wait_ms)
{
    uint8_t ch;
    uint8_t state = 0;
    uint32_t start = HAL_GetTick();

    boot_log("[IAP] wait command\r\n");
    while ((HAL_GetTick() - start) < wait_ms) {
        if (HAL_UART_Receive(&huart1, &ch, 1, 50) != HAL_OK) {
            continue;
        }

        if ((state == 0U && ch == 'I') ||
            (state == 1U && ch == 'A') ||
            (state == 2U && ch == 'P')) {
            state++;
            if (state == 3U) {
                return 1U;
            }
        } else {
            state = (ch == 'I') ? 1U : 0U;
        }
    }

    boot_log("[IAP] no command\r\n");
    return 0U;
}

static boot_status_t uart_receive_exact(uint8_t *buf, uint32_t len, uint32_t timeout_ms)
{
    uint32_t received = 0;

    while (received < len) {
        uint32_t chunk = len - received;
        if (chunk > BOOT_READ_CHUNK) {
            chunk = BOOT_READ_CHUNK;
        }

        if (HAL_UART_Receive(&huart1, &buf[received], (uint16_t)chunk, timeout_ms) != HAL_OK) {
            return BOOT_SD_ERROR;
        }

        received += chunk;
    }

    return BOOT_OK;
}

static void uart_drain_rx(uint32_t drain_ms)
{
    uint8_t ch;
    uint32_t start = HAL_GetTick();

    while ((HAL_GetTick() - start) < drain_ms) {
        (void)HAL_UART_Receive(&huart1, &ch, 1, 1);
    }
}

static void esp_drain_rx(uint32_t drain_ms)
{
    uint8_t ch;
    uint32_t start = HAL_GetTick();

    while ((HAL_GetTick() - start) < drain_ms) {
        (void)HAL_UART_Receive(&huart3, &ch, 1, 1);
    }
}

static uint8_t esp_wait_text(const char *expect, uint32_t timeout_ms)
{
    char window[24];
    uint32_t len = 0;
    uint32_t expect_len = strlen(expect);
    uint32_t start = HAL_GetTick();
    uint8_t ch;

    if (expect_len >= sizeof(window)) {
        return 0U;
    }

    memset(window, 0, sizeof(window));
    while ((HAL_GetTick() - start) < timeout_ms) {
        if (HAL_UART_Receive(&huart3, &ch, 1, 50) != HAL_OK) {
            continue;
        }

        if (len < expect_len) {
            window[len++] = (char)ch;
        } else {
            memmove(window, &window[1], expect_len - 1U);
            window[expect_len - 1U] = (char)ch;
        }

        if (len >= expect_len && memcmp(&window[len - expect_len], expect, expect_len) == 0) {
            return 1U;
        }
    }

    return 0U;
}

static uint8_t esp_send_cmd(const char *cmd, const char *expect, uint32_t timeout_ms)
{
    esp_drain_rx(50U);
    HAL_UART_Transmit(&huart3, (uint8_t *)cmd, strlen(cmd), 1000);
    return esp_wait_text(expect, timeout_ms);
}

static uint8_t esp_start_tcp_server(void)
{
    boot_log("[WIFI] esp init\r\n");

    if (!esp_send_cmd("AT\r\n", "OK", 1000U)) {
        boot_log("[WIFI] AT fail\r\n");
        return 0U;
    }
    (void)esp_send_cmd("ATE0\r\n", "OK", 1000U);

    if (!esp_send_cmd("AT+CWMODE=2\r\n", "OK", 2000U)) {
        boot_log("[WIFI] mode fail\r\n");
        return 0U;
    }
    if (!esp_send_cmd("AT+CWSAP=\"STM32-IAP\",\"12345678\",5,3\r\n", "OK", 5000U)) {
        boot_log("[WIFI] ap fail\r\n");
        return 0U;
    }
    if (!esp_send_cmd("AT+CIPMUX=1\r\n", "OK", 2000U)) {
        boot_log("[WIFI] mux fail\r\n");
        return 0U;
    }
    (void)esp_send_cmd("AT+CIPSERVER=0\r\n", "OK", 2000U);
    if (!esp_send_cmd("AT+CIPSERVER=1,8080\r\n", "OK", 3000U)) {
        boot_log("[WIFI] server fail\r\n");
        return 0U;
    }
    (void)esp_send_cmd("AT+CIPSTO=120\r\n", "OK", 2000U);

    boot_log("[WIFI] AP STM32-IAP pwd 12345678\r\n");
    boot_log("[WIFI] TCP 192.168.4.1:8080\r\n");
    return 1U;
}

static uint8_t esp_tcp_send_text(const char *text)
{
    char cmd[32];
    int len = (int)strlen(text);

    sprintf(cmd, "AT+CIPSEND=%d,%d\r\n", s_esp_link_id, len);
    HAL_UART_Transmit(&huart3, (uint8_t *)cmd, strlen(cmd), 1000);
    if (!esp_wait_text(">", 3000U)) {
        boot_log("[WIFI] cipsend prompt fail\r\n");
        return 0U;
    }

    HAL_UART_Transmit(&huart3, (uint8_t *)text, len, 1000);
    return 1U;
}

static boot_status_t esp_read_payload_byte(uint8_t *out, uint32_t timeout_ms)
{
    static uint32_t ipd_remaining = 0;
    static uint32_t ipd_len = 0;
    static uint32_t ipd_first = 0;
    static uint8_t scan_state = 0;
    static uint8_t ipd_has_link = 0;
    uint32_t start = HAL_GetTick();
    uint8_t ch;

    while ((HAL_GetTick() - start) < timeout_ms) {
        if (ipd_remaining > 0U) {
            if (HAL_UART_Receive(&huart3, out, 1, timeout_ms) == HAL_OK) {
                ipd_remaining--;
                return BOOT_OK;
            }
            return BOOT_SD_ERROR;
        }

        if (HAL_UART_Receive(&huart3, &ch, 1, 50) != HAL_OK) {
            continue;
        }

        switch (scan_state) {
        case 0: scan_state = (ch == '+') ? 1U : 0U; break;
        case 1: scan_state = (ch == 'I') ? 2U : 0U; break;
        case 2: scan_state = (ch == 'P') ? 3U : 0U; break;
        case 3: scan_state = (ch == 'D') ? 4U : 0U; break;
        case 4:
            if (ch == ',') {
                ipd_first = 0U;
                ipd_len = 0U;
                ipd_has_link = 0U;
                scan_state = 5U;
            } else {
                scan_state = 0U;
            }
            break;
        case 5:
            if (ch >= '0' && ch <= '9') {
                ipd_len = (ipd_len * 10U) + (uint32_t)(ch - '0');
            } else if (ch == ',') {
                ipd_first = ipd_len;
                ipd_len = 0U;
                ipd_has_link = 1U;
            } else if (ch == ':') {
                if (ipd_has_link) {
                    s_esp_link_id = (uint8_t)ipd_first;
                }
                ipd_remaining = ipd_len;
                scan_state = 0U;
            } else {
                scan_state = 0U;
            }
            break;
        default:
            scan_state = 0U;
            break;
        }
    }

    return BOOT_SD_ERROR;
}

static uint8_t esp_wait_iap_command(uint32_t wait_ms)
{
    uint8_t ch;
    uint8_t state = 0;
    uint32_t start = HAL_GetTick();

    boot_log("[WIFI] wait IAP\r\n");
    while ((HAL_GetTick() - start) < wait_ms) {
        if (esp_read_payload_byte(&ch, 500U) != BOOT_OK) {
            continue;
        }

        if ((state == 0U && ch == 'I') ||
            (state == 1U && ch == 'A') ||
            (state == 2U && ch == 'P')) {
            state++;
            if (state == 3U) {
                return 1U;
            }
        } else {
            state = (ch == 'I') ? 1U : 0U;
        }
    }

    boot_log("[WIFI] no IAP\r\n");
    return 0U;
}

static boot_status_t esp_receive_exact(uint8_t *buf, uint32_t len, uint32_t timeout_ms)
{
    uint32_t i;

    for (i = 0; i < len; i++) {
        if (esp_read_payload_byte(&buf[i], timeout_ms) != BOOT_OK) {
            return BOOT_SD_ERROR;
        }
    }

    return BOOT_OK;
}

static uint32_t flash_sector_from_address(uint32_t address)
{
    if (address < 0x08004000UL) return FLASH_SECTOR_0;
    if (address < 0x08008000UL) return FLASH_SECTOR_1;
    if (address < 0x0800C000UL) return FLASH_SECTOR_2;
    if (address < 0x08010000UL) return FLASH_SECTOR_3;
    if (address < 0x08020000UL) return FLASH_SECTOR_4;
    if (address < 0x08040000UL) return FLASH_SECTOR_5;
    if (address < 0x08060000UL) return FLASH_SECTOR_6;
    return FLASH_SECTOR_7;
}

static boot_status_t erase_app_region(uint32_t image_size)
{
    FLASH_EraseInitTypeDef erase = {0};
    uint32_t sector_error = 0;
    uint32_t first_sector = flash_sector_from_address(OTA_APP_BASE);
    uint32_t last_sector = flash_sector_from_address(OTA_APP_BASE + image_size - 1U);

    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.Sector = first_sector;
    erase.NbSectors = (last_sector - first_sector) + 1U;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    boot_log("[BOOT] flash erase start\r\n");
    if (HAL_FLASHEx_Erase(&erase, &sector_error) != HAL_OK) {
        boot_log("[BOOT] flash erase fail\r\n");
        return BOOT_FLASH_ERROR;
    }
    boot_log("[BOOT] flash erase ok\r\n");
    return BOOT_OK;
}

static boot_status_t program_app_image(uint32_t image_size, uint32_t *crc_out)
{
    UINT br = 0;
    uint32_t address = OTA_APP_BASE;
    uint32_t remaining = image_size;
    uint32_t crc = 0;

    boot_log("[BOOT] program start\r\n");
    while (remaining > 0U) {
        uint32_t to_read = remaining > BOOT_READ_CHUNK ? BOOT_READ_CHUNK : remaining;
        uint32_t i;

        if (f_read(&s_file, s_buf, to_read, &br) != FR_OK || br != to_read) {
            boot_log("[BOOT] program read fail\r\n");
            return BOOT_SD_ERROR;
        }

        crc = crc32_update(crc, s_buf, br);

        for (i = 0; i < br; i += 4U) {
            uint32_t word = 0xFFFFFFFFUL;
            uint32_t copy = (br - i) >= 4U ? 4U : (br - i);

            memcpy(&word, &s_buf[i], copy);
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, word) != HAL_OK) {
                boot_log("[BOOT] program flash fail\r\n");
                return BOOT_FLASH_ERROR;
            }
            address += 4U;
        }

        remaining -= br;
    }

    *crc_out = crc;
    boot_log("[BOOT] program ok\r\n");
    return BOOT_OK;
}

static boot_status_t program_uart_image(uint32_t image_size, uint32_t *crc_out)
{
    uint32_t address = OTA_APP_BASE;
    uint32_t remaining = image_size;
    uint32_t crc = 0;

    boot_log("[IAP] program start\r\n");
    while (remaining > 0U) {
        uint32_t to_read = remaining > BOOT_READ_CHUNK ? BOOT_READ_CHUNK : remaining;
        uint32_t i;

        boot_log("[IAP] next\r\n");
        if (uart_receive_exact(s_buf, to_read, 5000) != BOOT_OK) {
            boot_log("[IAP] receive fail\r\n");
            return BOOT_SD_ERROR;
        }

        crc = crc32_update(crc, s_buf, to_read);

        for (i = 0; i < to_read; i += 4U) {
            uint32_t word = 0xFFFFFFFFUL;
            uint32_t copy = (to_read - i) >= 4U ? 4U : (to_read - i);

            memcpy(&word, &s_buf[i], copy);
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, word) != HAL_OK) {
                boot_log("[IAP] flash program fail\r\n");
                return BOOT_FLASH_ERROR;
            }
            address += 4U;
        }

        remaining -= to_read;
    }

    *crc_out = crc;
    boot_log("[IAP] program ok\r\n");
    return BOOT_OK;
}

static boot_status_t program_esp_image(uint32_t image_size, uint32_t *crc_out)
{
    uint32_t address = OTA_APP_BASE;
    uint32_t remaining = image_size;
    uint32_t crc = 0;

    boot_log("[WIFI] program start\r\n");
    while (remaining > 0U) {
        uint32_t to_read = remaining > BOOT_READ_CHUNK ? BOOT_READ_CHUNK : remaining;
        uint32_t i;

        if (!esp_tcp_send_text("NEXT\n")) {
            boot_log("[WIFI] next send fail\r\n");
            return BOOT_SD_ERROR;
        }

        if (esp_receive_exact(s_buf, to_read, 10000U) != BOOT_OK) {
            boot_log("[WIFI] receive fail\r\n");
            return BOOT_SD_ERROR;
        }

        crc = crc32_update(crc, s_buf, to_read);

        for (i = 0; i < to_read; i += 4U) {
            uint32_t word = 0xFFFFFFFFUL;
            uint32_t copy = (to_read - i) >= 4U ? 4U : (to_read - i);

            memcpy(&word, &s_buf[i], copy);
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, word) != HAL_OK) {
                boot_log("[WIFI] flash program fail\r\n");
                return BOOT_FLASH_ERROR;
            }
            address += 4U;
        }

        remaining -= to_read;
    }

    *crc_out = crc;
    boot_log("[WIFI] program ok\r\n");
    return BOOT_OK;
}

static boot_status_t verify_package_image(uint32_t image_size, uint32_t expected_crc)
{
    UINT br = 0;
    uint32_t remaining = image_size;
    uint32_t crc = 0;

    boot_log("[BOOT] precheck start\r\n");
    while (remaining > 0U) {
        uint32_t to_read = remaining > BOOT_READ_CHUNK ? BOOT_READ_CHUNK : remaining;

        if (f_read(&s_file, s_buf, to_read, &br) != FR_OK || br != to_read) {
            boot_log("[BOOT] precheck read fail\r\n");
            return BOOT_SD_ERROR;
        }

        crc = crc32_update(crc, s_buf, br);
        remaining -= br;
    }

    boot_log_hex("[BOOT] precheck crc=", crc);
    if (crc != expected_crc) {
        boot_log("[BOOT] precheck crc fail\r\n");
        return BOOT_VERIFY_ERROR;
    }

    if (f_lseek(&s_file, sizeof(ota_package_header_t)) != FR_OK) {
        boot_log("[BOOT] precheck seek fail\r\n");
        return BOOT_SD_ERROR;
    }

    boot_log("[BOOT] precheck ok\r\n");
    return BOOT_OK;
}

static uint32_t crc32_flash(uint32_t address, uint32_t len)
{
    uint32_t crc = 0;
    uint32_t remaining = len;

    while (remaining > 0U) {
        uint32_t chunk = remaining > BOOT_READ_CHUNK ? BOOT_READ_CHUNK : remaining;
        memcpy(s_buf, (const void *)address, chunk);
        crc = crc32_update(crc, s_buf, chunk);
        address += chunk;
        remaining -= chunk;
    }

    return crc;
}

int boot_is_valid_app(uint32_t app_base)
{
    uint32_t stack = *(volatile uint32_t *)app_base;
    uint32_t reset = *(volatile uint32_t *)(app_base + 4U);

    if ((stack & 0x2FFE0000UL) != 0x20000000UL) {
        return 0;
    }

    if (reset < OTA_APP_BASE || reset >= OTA_FLASH_END) {
        return 0;
    }

    return 1;
}

void boot_jump_to_app(uint32_t app_base)
{
    typedef void (*app_entry_t)(void);
    uint32_t app_stack = *(volatile uint32_t *)app_base;
    uint32_t app_reset = *(volatile uint32_t *)(app_base + 4U);
    app_entry_t app_entry = (app_entry_t)app_reset;
    uint32_t i;

    __disable_irq();

    for (i = 0; i < 8U; i++) {
        NVIC->ICER[i] = 0xFFFFFFFFUL;
        NVIC->ICPR[i] = 0xFFFFFFFFUL;
    }

    HAL_DeInit();
    HAL_RCC_DeInit();

    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    __DSB();
    __ISB();

    SCB->VTOR = app_base;
    __set_MSP(app_stack);
    __enable_irq();
    app_entry();
}

boot_status_t boot_try_update_from_uart(uint32_t wait_ms)
{
    ota_package_header_t header;
    uint32_t programmed_crc = 0;
    uint32_t verify_crc;
    boot_status_t status;

    if (!uart_wait_iap_command(wait_ms)) {
        return BOOT_NO_PACKAGE;
    }

    uart_drain_rx(100U);
    boot_log("[IAP] ready\r\n");
    if (uart_receive_exact((uint8_t *)&header, sizeof(header), 10000) != BOOT_OK) {
        boot_log("[IAP] header receive fail\r\n");
        return BOOT_SD_ERROR;
    }

    boot_log("[IAP] check header\r\n");
    if (header.magic != OTA_PACKAGE_MAGIC ||
        header.header_version != OTA_PACKAGE_VERSION ||
        header.image_size == 0U ||
        header.image_size > OTA_APP_MAX_SIZE) {
        boot_log("[IAP] bad header\r\n");
        return BOOT_BAD_PACKAGE;
    }
    boot_log_hex("[IAP] image size=", header.image_size);
    boot_log_hex("[IAP] image crc=", header.image_crc32);

    boot_log("[IAP] flash unlock\r\n");
    HAL_FLASH_Unlock();
    status = erase_app_region(header.image_size);
    if (status == BOOT_OK) {
        status = program_uart_image(header.image_size, &programmed_crc);
    }
    HAL_FLASH_Lock();
    boot_log("[IAP] flash lock\r\n");

    if (status != BOOT_OK) {
        boot_log("[IAP] update write fail\r\n");
        return status;
    }

    boot_log("[IAP] check crc\r\n");
    boot_log_hex("[IAP] recv crc=", programmed_crc);
    if (programmed_crc != header.image_crc32) {
        boot_log("[IAP] crc fail\r\n");
        return BOOT_VERIFY_ERROR;
    }

    boot_log("[IAP] verify flash crc\r\n");
    verify_crc = crc32_flash(OTA_APP_BASE, header.image_size);
    if (verify_crc != header.image_crc32 || !boot_is_valid_app(OTA_APP_BASE)) {
        boot_log("[IAP] verify fail\r\n");
        return BOOT_VERIFY_ERROR;
    }

    boot_log("[IAP] update done\r\n");
    return BOOT_OK;
}

boot_status_t boot_try_update_from_esp(uint32_t wait_ms)
{
    ota_package_header_t header;
    uint32_t programmed_crc = 0;
    uint32_t verify_crc;
    boot_status_t status;

    if (!esp_start_tcp_server()) {
        return BOOT_SD_ERROR;
    }

    if (!esp_wait_iap_command(wait_ms)) {
        return BOOT_NO_PACKAGE;
    }

    boot_log("[WIFI] IAP command ok\r\n");
    if (!esp_tcp_send_text("READY\n")) {
        boot_log("[WIFI] ready send fail\r\n");
        return BOOT_SD_ERROR;
    }

    if (esp_receive_exact((uint8_t *)&header, sizeof(header), 10000U) != BOOT_OK) {
        boot_log("[WIFI] header receive fail\r\n");
        return BOOT_SD_ERROR;
    }

    boot_log("[WIFI] check header\r\n");
    if (header.magic != OTA_PACKAGE_MAGIC ||
        header.header_version != OTA_PACKAGE_VERSION ||
        header.image_size == 0U ||
        header.image_size > OTA_APP_MAX_SIZE) {
        boot_log("[WIFI] bad header\r\n");
        return BOOT_BAD_PACKAGE;
    }
    boot_log_hex("[WIFI] image size=", header.image_size);
    boot_log_hex("[WIFI] image crc=", header.image_crc32);

    boot_log("[WIFI] flash unlock\r\n");
    HAL_FLASH_Unlock();
    status = erase_app_region(header.image_size);
    if (status == BOOT_OK) {
        status = program_esp_image(header.image_size, &programmed_crc);
    }
    HAL_FLASH_Lock();
    boot_log("[WIFI] flash lock\r\n");

    if (status != BOOT_OK) {
        boot_log("[WIFI] update write fail\r\n");
        return status;
    }

    boot_log("[WIFI] check crc\r\n");
    boot_log_hex("[WIFI] recv crc=", programmed_crc);
    if (programmed_crc != header.image_crc32) {
        boot_log("[WIFI] crc fail\r\n");
        return BOOT_VERIFY_ERROR;
    }

    boot_log("[WIFI] verify flash crc\r\n");
    verify_crc = crc32_flash(OTA_APP_BASE, header.image_size);
    if (verify_crc != header.image_crc32 || !boot_is_valid_app(OTA_APP_BASE)) {
        boot_log("[WIFI] verify fail\r\n");
        return BOOT_VERIFY_ERROR;
    }

    boot_log("[WIFI] update done\r\n");
    return BOOT_OK;
}

boot_status_t boot_try_update_from_sd(void)
{
    ota_package_header_t header;
    UINT br = 0;
    FRESULT fres;
    uint32_t i;
    uint32_t programmed_crc = 0;
    uint32_t verify_crc;
    boot_status_t status;

    boot_log("[BOOT] mount start\r\n");
    if (f_mount(&s_fs, "0:", 1) != FR_OK) {
        boot_log("[BOOT] mount fail\r\n");
        return BOOT_SD_ERROR;
    }
    boot_log("[BOOT] mount ok\r\n");

    boot_log("[BOOT] open start\r\n");
    for (i = 0; i < (sizeof(s_package_paths) / sizeof(s_package_paths[0])); i++) {
        boot_log("[BOOT] try package\r\n");
        boot_log(s_package_paths[i]);
        boot_log("\r\n");
        fres = f_open(&s_file, s_package_paths[i], FA_READ);
        if (fres == FR_OK) {
            break;
        }
        boot_log_hex("[BOOT] f_open=", fres);
    }

    if (i == (sizeof(s_package_paths) / sizeof(s_package_paths[0]))) {
        boot_log("[BOOT] open fail all\r\n");
        return BOOT_NO_PACKAGE;
    }
    boot_log("[BOOT] open ok\r\n");

    boot_log("[BOOT] read header start\r\n");
    if (f_read(&s_file, &header, sizeof(header), &br) != FR_OK || br != sizeof(header)) {
        boot_log("[BOOT] read header fail\r\n");
        f_close(&s_file);
        return BOOT_BAD_PACKAGE;
    }
    boot_log("[BOOT] read header ok\r\n");

    boot_log("[BOOT] check header\r\n");
    if (header.magic != OTA_PACKAGE_MAGIC ||
        header.header_version != OTA_PACKAGE_VERSION ||
        header.image_size == 0U ||
        header.image_size > OTA_APP_MAX_SIZE ||
        f_size(&s_file) != (sizeof(header) + header.image_size)) {
        boot_log("[BOOT] bad header\r\n");
        f_close(&s_file);
        return BOOT_BAD_PACKAGE;
    }
    boot_log("[BOOT] header ok\r\n");

    status = verify_package_image(header.image_size, header.image_crc32);
    if (status != BOOT_OK) {
        boot_log("[BOOT] package precheck fail\r\n");
        f_close(&s_file);
        return status;
    }

    boot_log("[BOOT] flash unlock\r\n");
    HAL_FLASH_Unlock();
    status = erase_app_region(header.image_size);
    if (status == BOOT_OK) {
        status = program_app_image(header.image_size, &programmed_crc);
    }
    HAL_FLASH_Lock();
    boot_log("[BOOT] flash lock\r\n");

    f_close(&s_file);

    if (status != BOOT_OK) {
        boot_log("[BOOT] update write fail\r\n");
        return status;
    }

    boot_log("[BOOT] check crc\r\n");
    if (programmed_crc != header.image_crc32) {
        boot_log("[BOOT] program crc fail\r\n");
        return BOOT_VERIFY_ERROR;
    }

    boot_log("[BOOT] verify flash crc\r\n");
    verify_crc = crc32_flash(OTA_APP_BASE, header.image_size);
    if (verify_crc != header.image_crc32 || !boot_is_valid_app(OTA_APP_BASE)) {
        boot_log("[BOOT] verify fail\r\n");
        return BOOT_VERIFY_ERROR;
    }

    boot_log("[BOOT] skip rename\r\n");
    boot_log("[BOOT] update done\r\n");
    return BOOT_OK;
}
