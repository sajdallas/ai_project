#include "web_server.h"
#include "esp8266.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HTTP_PORT        80
#define HTTP_RESP_BUF    512
#define HTTP_URL_BUF     128

web_sensor_data_t g_web_sensor;

static int server_running = 0;
static char rx_buf[1024];
static int  rx_len = 0;

static void parse_http(const char *req, char *method, char *path, int cap);
static void build_json_sensors(char *buf, int cap);
static void build_json_status(char *buf, int cap);
static void send_response(int conn_id, int code, const char *ctype, const char *body);
static int  extract_conn_id(const char *data, int len);

int WebServer_Start(void)
{
    if (server_running) return 0;

    ESP_SendCmd("AT+CIPMUX=1\r\n", "OK", 2000);
    ESP_SendCmd("AT+CIPSERVER=0\r\n", "OK", 1000);

    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+CIPSERVER=1,%d\r\n", HTTP_PORT);
    if (ESP_SendCmd(cmd, "OK", 3000) != ESP_OK) {
        return -1;
    }

    server_running = 1;
    return 0;
}

void WebServer_Stop(void)
{
    if (!server_running) return;
    ESP_SendCmd("AT+CIPSERVER=0\r\n", "OK", 1000);
    server_running = 0;
}

void WebServer_Poll(void)
{
    if (!server_running) return;

    /* 检查 ESP8266 接收缓冲区是否有数据 */
    int len = (int)ESP_GetRxLen();
    if (len == 0) return;

    /* 拷贝到本地缓冲区，防止处理过程中被覆盖 */
    int cap = sizeof(rx_buf) - 1;
    if (len > cap) len = cap;
    memcpy(rx_buf, ESP_GetRxBuf(), len);
    rx_buf[len] = '\0';
    rx_len = len;

    /* 查找 +IPD 标记，ESP8266 服务器格式：+IPD,<id>,<len>:<HTTP数据> */
    char *ipd = strstr(rx_buf, "+IPD,");
    if (!ipd) return;

    /* 提取连接 ID，后续 AT+CIPSEND 需要指定发给哪个客户端 */
    int conn_id = extract_conn_id(rx_buf, rx_len);
    if (conn_id < 0) return;

    /* 跳过 +IPD,<id>,<len>: 前缀，定位到 HTTP 请求正文 */
    char *colon = strchr(ipd, ':');
    if (!colon) return;
    colon++;

    /* 解析 HTTP 请求行，提取 method 和 path */
    char method[8] = {0};
    char path[HTTP_URL_BUF] = {0};
    parse_http(colon, method, path, sizeof(path));

    /* 路由分发：根据请求路径返回不同 JSON 数据 */
    char resp[HTTP_RESP_BUF];

    if (strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0) {
        send_response(conn_id, 200, "text/html; charset=utf-8",
            "<!DOCTYPE html><html lang=\"zh-CN\"><head>"
            "<meta charset=\"UTF-8\">"
            "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\">"
            "<title>智能设备</title>"
            "<style>"
            "body{font-family:sans-serif;background:#1a1a2e;color:#eee;padding:16px;text-align:center}"
            "h1{font-weight:300;font-size:20px}"
            ".card{background:#16213e;border-radius:16px;padding:16px;margin:10px 0}"
            ".val{font-size:28px;font-weight:bold}"
            ".unit{font-size:12px;color:#aaa}"
            ".row{display:flex;justify-content:space-around;text-align:center}"
            ".off{color:#e94560}.on{color:#00c853}"
            "</style></head><body>"
            "<h1>🤖 智能设备</h1>"
            "<div id=\"wifi\" class=\"off\">📶 离线</div>"
            "<div class=\"card\"><div class=\"row\">"
            "<div><div class=\"val\" id=\"temp\">--</div><div class=\"unit\">°C</div><div>温度</div></div>"
            "<div><div class=\"val\" id=\"humi\">--</div><div class=\"unit\">%</div><div>湿度</div></div>"
            "<div><div class=\"val\" id=\"pres\">--</div><div class=\"unit\">hPa</div><div>气压</div></div>"
            "</div></div>"
            "<div class=\"card\"><div class=\"row\">"
            "<div><div class=\"val\" id=\"alti\">--</div><div class=\"unit\">m</div><div>海拔</div></div>"
            "<div><div class=\"val\" id=\"runtime\">--</div><div class=\"unit\"></div><div>运行时间</div></div>"
            "</div></div>"
            "<script>"
            "function u(){"
            "fetch('/api/sensors').then(r=>r.json()).then(d=>{"
            "document.getElementById('temp').textContent=d.temp.toFixed(1);"
            "document.getElementById('humi').textContent=d.humi.toFixed(1);"
            "document.getElementById('pres').textContent=d.pres.toFixed(1);"
            "document.getElementById('alti').textContent=d.alti.toFixed(1);"
            "document.getElementById('wifi').textContent='📶 在线';"
            "document.getElementById('wifi').className='on';"
            "}).catch(()=>{"
            "document.getElementById('wifi').textContent='📶 离线';"
            "document.getElementById('wifi').className='off';"
            "});"
            "fetch('/api/status').then(r=>r.json()).then(d=>{"
            "document.getElementById('runtime').textContent=d.uptime_str||(d.uptime_sec+'s');"
            "}).catch(()=>{});"
            "}"
            "setInterval(u,3000);u();"
            "</script></body></html>"
        );
    }
    else if (strcmp(path, "/api/sensors") == 0) {
        /* 传感器数据：温度、湿度、气压、海拔 */
        build_json_sensors(resp, sizeof(resp));
        send_response(conn_id, 200, "application/json", resp);
    }
    else if (strcmp(path, "/api/status") == 0) {
        /* 设备状态：芯片型号、屏幕、传感器、运行时间 */
        build_json_status(resp, sizeof(resp));
        send_response(conn_id, 200, "application/json", resp);
    }
    else if (strcmp(path, "/api/ping") == 0) {
        /* 连通性测试：返回 {"ok":true} */
        send_response(conn_id, 200, "application/json", "{\"ok\":true}");
    }
    else {
        /* 未知路径，返回 404 */
        send_response(conn_id, 404, "application/json", "{\"error\":\"not found\"}");
    }

    /* 处理完毕：清空接收缓冲，关闭该连接 */
    ESP_FlushRx();
    {
        char cmd[16];
        snprintf(cmd, sizeof(cmd), "AT+CIPCLOSE=%d\r\n", conn_id);
        ESP_SendCmd(cmd, NULL, 500);
    }
}

/*
 * 从 +IPD,<id>,<len>:<data> 中提取连接 ID
 * 例："+IPD,0,256:GET /" → 跳过 "+IPD," → 取 "0" → 返回 0
 * 返回 -1 表示解析失败
 */
static int extract_conn_id(const char *data, int len)
{
    (void)len;                              /* len 暂未使用，保留以备后续校验 */
    char *ipd = strstr(data, "+IPD,");      /* 定位 +IPD, 标记 */
    if (!ipd) return -1;
    ipd += 5;                               /* 跳过 "+IPD," 5 个字符 */
    char *comma = strchr(ipd, ',');         /* 找下一个逗号，即 ID 的结束位置 */
    if (!comma) return -1;
    int n = (int)(comma - ipd);             /* ID 字符串长度 */
    if (n <= 0 || n > 4) return -1;         /* ID 长度异常（0~9999 合理） */
    char tmp[8];
    memcpy(tmp, ipd, n);                    /* 拷贝 ID 数字串 */
    tmp[n] = '\0';
    return atoi(tmp);                       /* 转整数返回 */
}

/*
 * 解析 HTTP 请求行，提取方法和路径
 * 例："GET /api/sensors HTTP/1.1" → method="GET", path="/api/sensors"
 *      "GET /api/chat?msg=hi HTTP/1.1" → method="GET", path="/api/chat"
 * 忽略 ? 后的查询参数，只取路径部分
 */
static void parse_http(const char *req, char *method, char *path, int cap)
{
    const char *p = req;
    int i = 0;

    /* 提取 method：读到第一个空格为止，最多 7 字符（GET/POST/DELETE...） */
    while (*p && *p != ' ' && i < 7) {
        method[i++] = *p++;
    }
    method[i] = '\0';

    /* 跳过 method 和 path 之间的空格 */
    while (*p == ' ') p++;

    /* 提取 path：读到空格或 ? 为止 */
    i = 0;
    while (*p && *p != ' ' && *p != '?' && i < cap - 1) {
        path[i++] = *p++;
    }
    path[i] = '\0';
}

/* 拼接传感器 JSON，例：{"temp":25.3,"humi":60.0,"pres":1013.2,"alti":120.5} */
static void build_json_sensors(char *buf, int cap)
{
    snprintf(buf, cap,
        "{"
        "\"temp\":%.1f,"     /* 温度，保留 1 位小数 */
        "\"humi\":%.1f,"     /* 湿度 */
        "\"pres\":%.1f,"     /* 气压 */
        "\"alti\":%.1f"      /* 海拔 */
        "}",
        g_web_sensor.temperature,
        g_web_sensor.humidity,
        g_web_sensor.pressure,
        g_web_sensor.altitude);
}

/* 拼接设备状态 JSON，例：{"chip":"STM32F407","lcd":"240x320","sensors":"AHT20+BMP280","uptime_sec":3600,"uptime_str":"1h 0m 0s"} */
static void build_json_status(char *buf, int cap)
{
    /* 计算系统运行时间（自上次复位起） */
    uint32_t uptime = HAL_GetTick() / 1000;     /* 总秒数 */
    uint32_t h = uptime / 3600;                 /* 小时 */
    uint32_t m = (uptime % 3600) / 60;          /* 分钟 */
    uint32_t s = uptime % 60;                   /* 秒   */

    snprintf(buf, cap,
        "{"
        "\"chip\":\"STM32F407\","               /* 主控芯片 */
        "\"lcd\":\"240x320\","                  /* 屏幕分辨率 */
        "\"sensors\":\"AHT20+BMP280\","         /* 传感器型号 */
        "\"uptime_sec\":%lu,"                   /* 运行秒数（数字，方便前端计算） */
        "\"uptime_str\":\"%luh %lum %lus\""     /* 运行时间（可读字符串） */
        "}",
        uptime, h, m, s);
}

static void send_response(int conn_id, int code, const char *ctype, const char *body)
{
    char header[256];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 %d OK\r\n"
        "Content-Type: %s\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Connection: close\r\n"
        "Content-Length: %d\r\n"
        "\r\n",
        code, ctype, (int)strlen(body));

    int total = header_len + (int)strlen(body);

    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d,%d\r\n", conn_id, total);
    ESP_SendCmd(cmd, ">", 2000);

    ESP_FlushRx();

    HAL_UART_Transmit(&huart3, (uint8_t *)header, header_len, 1000);
    HAL_UART_Transmit(&huart3, (uint8_t *)body, strlen(body), 1000);

    osDelay(50);
}