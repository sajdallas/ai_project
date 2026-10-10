#ifndef OTA_CONFIG_H
#define OTA_CONFIG_H

#include <stdint.h>

#define OTA_BOOTLOADER_BASE      0x08000000UL
#define OTA_BOOTLOADER_SIZE      0x00008000UL
#define OTA_APP_BASE             0x08008000UL
#define OTA_FLASH_END            0x08080000UL
#define OTA_APP_MAX_SIZE         (OTA_FLASH_END - OTA_APP_BASE)

#define OTA_PACKAGE_MAGIC        0x41544F53UL
#define OTA_PACKAGE_VERSION      1UL
#define OTA_PACKAGE_PATH         "0:/Update/app.bin"

#ifndef OTA_APP_RELOCATE_VECTOR
#define OTA_APP_RELOCATE_VECTOR  0
#endif

#define OTA_APP_VECTOR_OFFSET    (OTA_APP_BASE - OTA_BOOTLOADER_BASE)

typedef struct {
    uint32_t magic;
    uint32_t header_version;
    uint32_t image_size;
    uint32_t image_crc32;
} ota_package_header_t;

#endif
