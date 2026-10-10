#ifndef __WEB_SERVER_H
#define __WEB_SERVER_H

#include <stdint.h>

typedef struct {
    float temperature;
    float humidity;
    float pressure;
    float altitude;
    uint32_t update_tick;
} web_sensor_data_t;

extern web_sensor_data_t g_web_sensor;

int  WebServer_Start(void);
void WebServer_Stop(void);
void WebServer_Poll(void);

#endif
