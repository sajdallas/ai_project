#include "diskio.h"
#include "sdio_sdcard.h"
#include <stdlib.h>
#include <stdint.h>

#define BOOT_SD_CARD 0

extern void boot_log(const char *msg);
extern void boot_log_hex(const char *prefix, uint32_t value);

static uint8_t s_ff_lfn_pool[1024];
static uint8_t s_ff_lfn_used;

DSTATUS disk_initialize(BYTE pdrv)
{
    if (pdrv != BOOT_SD_CARD) {
        boot_log("[BOOT] disk bad drive\r\n");
        return STA_NOINIT;
    }

    boot_log("[BOOT] disk init start\r\n");
    if (SD_Init() != SD_OK) {
        boot_log("[BOOT] disk init fail\r\n");
        return STA_NOINIT;
    }
    boot_log("[BOOT] disk init ok\r\n");
    return 0;
}

DSTATUS disk_status(BYTE pdrv)
{
    return pdrv == BOOT_SD_CARD ? 0 : STA_NOINIT;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    uint32_t retry;

    if (pdrv != BOOT_SD_CARD || count == 0U) {
        return RES_PARERR;
    }

    boot_log("[BOOT] disk read\r\n");
    boot_log_hex("[BOOT] read sector=", sector);
    boot_log_hex("[BOOT] read count=", count);

    for (retry = 0; retry < 3U; retry++) {
        if (SD_ReadDisk(buff, sector, count) == SD_OK) {
            boot_log("[BOOT] disk read ok\r\n");
            return RES_OK;
        }
        boot_log("[BOOT] disk read retry\r\n");
        boot_log_hex("[BOOT] retry=", retry + 1U);
    }

    boot_log("[BOOT] disk read fail\r\n");
    return RES_ERROR;
}

#if _USE_WRITE
DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    if (pdrv != BOOT_SD_CARD || count == 0U) {
        return RES_PARERR;
    }

    return SD_WriteDisk((u8 *)buff, sector, count) == SD_OK ? RES_OK : RES_ERROR;
}
#endif

#if _USE_IOCTL
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    if (pdrv != BOOT_SD_CARD) {
        return RES_PARERR;
    }

    switch (cmd) {
    case CTRL_SYNC:
        return RES_OK;
    case GET_SECTOR_SIZE:
        *(WORD *)buff = 512;
        return RES_OK;
    case GET_BLOCK_SIZE:
        *(DWORD *)buff = SDCardInfo.CardBlockSize;
        return RES_OK;
    case GET_SECTOR_COUNT:
        *(DWORD *)buff = SDCardInfo.CardCapacity / 512U;
        return RES_OK;
    default:
        return RES_PARERR;
    }
}
#endif

DWORD get_fattime(void)
{
    return 0;
}

void *ff_memalloc(UINT size)
{
    if (s_ff_lfn_used || size > sizeof(s_ff_lfn_pool)) {
        boot_log("[BOOT] ff mem fail\r\n");
        boot_log_hex("[BOOT] ff mem size=", size);
        return NULL;
    }

    s_ff_lfn_used = 1U;
    return s_ff_lfn_pool;
}

void ff_memfree(void *mem)
{
    if (mem == s_ff_lfn_pool) {
        s_ff_lfn_used = 0U;
    }
}

WCHAR ff_convert(WCHAR chr, UINT dir)
{
    (void)dir;
    return chr < 0x80U ? chr : 0;
}

WCHAR ff_wtoupper(WCHAR chr)
{
    if (chr >= 'a' && chr <= 'z') {
        return chr - ('a' - 'A');
    }

    return chr;
}
