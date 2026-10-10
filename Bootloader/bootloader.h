#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <stdint.h>

typedef enum {
    BOOT_OK = 0,
    BOOT_NO_PACKAGE,
    BOOT_BAD_PACKAGE,
    BOOT_SD_ERROR,
    BOOT_FLASH_ERROR,
    BOOT_VERIFY_ERROR
} boot_status_t;

boot_status_t boot_try_update_from_sd(void);
boot_status_t boot_try_update_from_uart(uint32_t wait_ms);
boot_status_t boot_try_update_from_esp(uint32_t wait_ms);
void boot_jump_to_app(uint32_t app_base);
int boot_is_valid_app(uint32_t app_base);

#endif
