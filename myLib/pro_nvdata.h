
#ifndef _PRO_NVDATA_
#define _PRO_NVDATA_
#include "ci_nvdata_manage.h"

uint16_t nvdata_temp_len;
void asrpro_nvdata_init(void* buf,uint16_t len,uint32_t id)
{
	cinv_register_hotid(id);
	cinv_item_init(id, len, buf);	
}
void asrpro_nvdata_write(void* buf,uint16_t len,uint32_t id)
{
	cinv_item_write(id,len, buf);	
}

void asrpro_nvdata_read(uint32_t id,uint16_t len,void* buf)
{
	cinv_item_read(id,len, buf, &nvdata_temp_len);
		
}


#endif