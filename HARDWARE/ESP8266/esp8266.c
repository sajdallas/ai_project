#include "esp8266.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static esp_t esp;

/* Snapshot of the raw RX bytes (with +IPD framing) taken inside
 * ESP_RecvResponse, so it survives the later CIPCLOSE flush and can be
 * dumped from freertos.c for diagnostics. */
static char esp_raw[ESP_RX_BUF_SIZE];
static int esp_raw_len = 0;

/* Flush the receive buffer and re-arm the 1-byte interrupt from rx_buf[0].
 * This fixes the stale-data bug where rx_len was cleared but the UART
 * receive pointer was left at the old position. */
static void ESP_FlushRx(void)
{
    __HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
    __HAL_UART_CLEAR_OREFLAG(&huart3);
    huart3.RxState = HAL_UART_STATE_READY;
    esp.rx_len = 0;
    memset(esp.rx_buf, 0, ESP_RX_BUF_SIZE);
    HAL_UART_Receive_IT(&huart3, &esp.rx_buf[0], 1);
}

/* Bounded substring search (strstr without NUL-terminator dependency). */
static char *ESP_MemStr(const char *hay, int haylen, const char *needle)
{
    int nlen = (int)strlen(needle);
    int i;
    if (nlen == 0 || haylen < nlen) return NULL;
    for (i = 0; i <= haylen - nlen; i++) {
        if (memcmp(&hay[i], needle, nlen) == 0) return (char *)&hay[i];
    }
    return NULL;
}

void ESP_Init(void)
{
    memset(&esp, 0, sizeof(esp));
    HAL_UART_Receive_IT(&huart3, &esp.rx_buf[0], 1);
}

void ESP_RxCallback(void)
{
    if (esp.rx_len < ESP_RX_BUF_SIZE - 1) {
        HAL_UART_Receive_IT(&huart3, &esp.rx_buf[esp.rx_len + 1], 1);
        esp.rx_len++;
    }
}

esp_status_t ESP_SendCmd(const char *cmd, const char *expect, uint32_t timeout)
{
    uint32_t tick_start = HAL_GetTick();

    esp.is_busy = 1;
    ESP_FlushRx();

    HAL_UART_Transmit(&huart3, (uint8_t*)cmd, strlen(cmd), 1000);

    while ((HAL_GetTick() - tick_start) < timeout) {
        if (expect && strstr((char*)esp.rx_buf, expect)) {
            esp.is_busy = 0;
            return ESP_OK;
        }
        if (!expect && strstr((char*)esp.rx_buf, "OK")) {
            esp.is_busy = 0;
            return ESP_OK;
        }
        if (strstr((char*)esp.rx_buf, "ERROR")) {
            esp.is_busy = 0;
            return ESP_ERR_UNKNOWN;
        }
        osDelay(10);
    }

    esp.is_busy = 0;
    return ESP_ERR_TIMEOUT;
}

esp_status_t ESP_ConnectAP(const char *ssid, const char *pwd)
{
    char cmd[128];
    esp_status_t status;

    for (int i = 0; i < ESP_MAX_RETRY; i++) {
        status = ESP_SendCmd("AT\r\n", "OK", 2000);
        if (status != ESP_OK) continue;

        status = ESP_SendCmd("ATE0\r\n", "OK", 1000);

        sprintf(cmd, "AT+CWQAP\r\n");
        ESP_SendCmd(cmd, NULL, 1000);

        if (strlen(pwd) > 0) {
            sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
        } else {
            sprintf(cmd, "AT+CWJAP=\"%s\"\r\n", ssid);
        }

        status = ESP_SendCmd(cmd, "OK", 15000);
        if (status == ESP_OK) {
            ESP_SendCmd("AT+CIPSSLSIZE=4096\r\n", "OK", 2000);
            break;
        }

        osDelay(1000);
    }

    return status;
}

/* Snapshot the raw RX bytes for diagnostics (survives the later CIPCLOSE flush). */
static void ESP_SnapshotRaw(void)
{
    int m = (int)esp.rx_len;
    if (m > ESP_RX_BUF_SIZE - 1) m = ESP_RX_BUF_SIZE - 1;
    memcpy(esp_raw, esp.rx_buf, m);
    esp_raw_len = m;
}

/* Receive a raw HTTP response over an open connection. The firmware frames
 * data as "+IPD,<len>:<data>" (CIPMUX=0) or "+IPD,<link_id>,<len>:<data>"
 * (CIPMUX=1). Stops on "CLOSED" or timeout. Returns payload length, or -1 on
 * timeout.
 *
 * NOTE: "Recv N bytes" is the AT+CIPSEND acknowledgement (the module received
 * OUR request bytes), NOT a response notification — so it is ignored here. */
int ESP_RecvResponse(char *resp, int cap, uint32_t timeout_ms)
{
    int resp_len = 0;
    int scan = 0;
    int closed = 0;
    uint32_t tick = HAL_GetTick();

    while ((HAL_GetTick() - tick) < timeout_ms) {
        int len = (int)esp.rx_len;
        int i = scan;

        while (i < len) {
            char *p = ESP_MemStr((const char *)&esp.rx_buf[i], len - i, "+IPD,");
            char *c = ESP_MemStr((const char *)&esp.rx_buf[i], len - i, "CLOSED");

            if (c && (!p || c < p)) {          /* CLOSED before next +IPD */
                closed = 1;
                i = len;
                break;
            }
            if (!p) { i = len; break; }        /* nothing more to parse yet */

            /* +IPD,<len>:<data>  or  +IPD,<link_id>,<len>:<data> */
            {
                int j = (int)(p - (const char *)esp.rx_buf) + 5;
                int n = 0;
                while (j < len && esp.rx_buf[j] >= '0' && esp.rx_buf[j] <= '9') {
                    n = n * 10 + (esp.rx_buf[j] - '0');
                    j++;
                }
                if (j < len && esp.rx_buf[j] == ',') {   /* link_id form (CIPMUX=1) */
                    j++;
                    n = 0;
                    while (j < len && esp.rx_buf[j] >= '0' && esp.rx_buf[j] <= '9') {
                        n = n * 10 + (esp.rx_buf[j] - '0');
                        j++;
                    }
                }
                if (j >= len || esp.rx_buf[j] != ':') { break; }   /* header incomplete: re-scan next poll */
                j++;
                if (j + n > len) { break; }                        /* payload incomplete: re-scan next poll */

                if (resp && resp_len < cap - 1) {
                    int copy = n;
                    if (resp_len + copy > cap - 1) copy = cap - 1 - resp_len;
                    memcpy(&resp[resp_len], &esp.rx_buf[j], copy);
                    resp_len += copy;
                }
                i = j + n;
            }
        }

        scan = i;
        if (closed) break;
        osDelay(5);
    }

    ESP_SnapshotRaw();
    if (resp) resp[resp_len] = '\0';
    return (resp_len > 0 || closed) ? resp_len : -1;
}

esp_status_t ESP_HTTPPost(const char *url, const char *json, char *resp, uint16_t resp_size)
{
    return ESP_HTTPPostAuth(url, json, NULL, resp, resp_size);
}

esp_status_t ESP_HTTPPostAuth(const char *url, const char *json, const char *auth_token, char *resp, uint16_t resp_size)
{
    char header[512];
    char cmd[128];
    esp_status_t status;
    const char *host_start;
    const char *path_start;
    char host[64];
    char path[128];
    int port = 80;
    int is_ssl = 0;
    char *colon;
    int json_len, header_len, total_len;
    int n;

    host_start = strstr(url, "://");
    if (!host_start) {
        host_start = url;
    } else {
        if (strncmp(url, "https", 5) == 0) {
            is_ssl = 1;
            port = 443;
        }
        host_start += 3;
    }

    path_start = strchr(host_start, '/');
    if (path_start) {
        int hlen = (int)(path_start - host_start);
        if (hlen >= (int)sizeof(host)) hlen = (int)sizeof(host) - 1;
        strncpy(host, host_start, hlen);
        host[hlen] = '\0';
        strcpy(path, path_start);
    } else {
        strcpy(host, host_start);
        strcpy(path, "/");
    }

    colon = strchr(host, ':');
    if (colon) {
        port = atoi(colon + 1);
        *colon = '\0';
    }

    /* connect: SSL first, fall back to legacy AT+CIPSSL=1 for old firmware */
    if (is_ssl) {
        /* dashscope/aliyun requires SNI — set it before the TLS handshake */
        sprintf(cmd, "AT+CIPSSLCSNI=\"%s\"\r\n", host);
        ESP_SendCmd(cmd, "OK", 2000);
    }
    if (is_ssl) {
        sprintf(cmd, "AT+CIPSTART=\"SSL\",\"%s\",%d\r\n", host, port);
        status = ESP_SendCmd(cmd, "OK", 15000);
        if (status != ESP_OK) {
            ESP_SendCmd("AT+CIPCLOSE\r\n", NULL, 1000);
            ESP_SendCmd("AT+CIPSSL=1\r\n", "OK", 2000);
            sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", host, port);
            status = ESP_SendCmd(cmd, "OK", 15000);
        }
    } else {
        sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", host, port);
        status = ESP_SendCmd(cmd, "OK", 15000);
    }
    if (status != ESP_OK) return status;

    /* build HTTP request header */
    json_len = (int)strlen(json);
    if (auth_token) {
        header_len = snprintf(header, sizeof(header),
            "POST %s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Authorization: Bearer %s\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n",
            path, host, auth_token, json_len);
    } else {
        header_len = snprintf(header, sizeof(header),
            "POST %s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n",
            path, host, json_len);
    }
    total_len = header_len + json_len;

    /* ask ESP8266 to expect the request bytes */
    sprintf(cmd, "AT+CIPSEND=%d\r\n", total_len);
    status = ESP_SendCmd(cmd, ">", 2000);
    if (status != ESP_OK) {
        ESP_SendCmd("AT+CIPCLOSE\r\n", NULL, 1000);
        return status;
    }

    /* Clear the CIPSEND prompt/ack before sending the request. Do this before
     * transmitting so a fast HTTP response is not flushed accidentally. */
    ESP_FlushRx();

    /* send header + body */
    HAL_UART_Transmit(&huart3, (uint8_t *)header, header_len, 1000);
    HAL_UART_Transmit(&huart3, (uint8_t *)json, json_len, 1000);

    /* receive response, stripping +IPD framing */
    n = ESP_RecvResponse(resp, resp_size, 60000);

    ESP_SendCmd("AT+CIPCLOSE\r\n", NULL, 1000);

    if (n < 0) return ESP_ERR_TIMEOUT;
    return ESP_OK;
}

uint16_t ESP_GetRxLen(void)
{
    return esp.rx_len;
}

const char* ESP_GetRxBuf(void)
{
    return (const char*)esp.rx_buf;
}

const char* ESP_GetRawBuf(void)
{
    return esp_raw;
}

int ESP_GetRawLen(void)
{
    return esp_raw_len;
}

void ESP_ClearRx(void)
{
    esp.rx_len = 0;
    memset(esp.rx_buf, 0, ESP_RX_BUF_SIZE);
}