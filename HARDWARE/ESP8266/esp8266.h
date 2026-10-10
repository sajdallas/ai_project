#ifndef __ESP8266_H
#define __ESP8266_H

#include "main.h"
#include "usart.h"
#include <string.h>

#define ESP_RX_BUF_SIZE    4096
#define ESP_TX_BUF_SIZE    256
#define ESP_TIMEOUT_MS     5000
#define ESP_MAX_RETRY      3

typedef enum {
    ESP_OK = 0,
    ESP_ERR_TIMEOUT,
    ESP_ERR_AT,
    ESP_ERR_WIFI,
    ESP_ERR_CONNECT,
    ESP_ERR_SEND,
    ESP_ERR_UNKNOWN
} esp_status_t;

typedef struct {
    uint8_t rx_buf[ESP_RX_BUF_SIZE];
    volatile uint16_t rx_len;
    uint8_t is_busy;
} esp_t;

void ESP_FlushRx(void);
void ESP_Init(void);
void ESP_RxCallback(void);
esp_status_t ESP_SendCmd(const char *cmd, const char *expect, uint32_t timeout);
esp_status_t ESP_SendData(const char *data, uint16_t len);
esp_status_t ESP_ConnectAP(const char *ssid, const char *pwd);
esp_status_t ESP_AutoConnectSavedAP(void);
int ESP_ScanAP(char ssids[][32], int max_count);
esp_status_t ESP_HTTPPost(const char *url, const char *json, char *resp, uint16_t resp_size);
esp_status_t ESP_HTTPPostAuth(const char *url, const char *json, const char *auth_token, char *resp, uint16_t resp_size);
uint16_t ESP_GetRxLen(void);
const char* ESP_GetRxBuf(void);
const char* ESP_GetRawBuf(void);
int ESP_GetRawLen(void);
int ESP_RecvResponse(char *resp, int cap, uint32_t timeout_ms);
void ESP_ClearRx(void);

#endif