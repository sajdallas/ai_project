# Web 上位机调试笔记

> 目标：通过浏览器网页实时查看 STM32 传感器数据、设备状态，无需额外安装任何软件。
> 结论：已跑通。这份笔记记录的是**架构设计、踩过的坑**，以及背后的原理。

---

## 1. 整体架构

```
浏览器(PC/手机)  ──HTTP──>  ESP8266(AT固件)  ──USART3──>  STM32F407
     │                         │                              │
     │  GET /api/sensors       │  +IPD,0,256:GET /api/sensors │  build_json_sensors()
     │  <────────────────────  │  <────────────────────────── │
     │  200 OK {"temp":25.3}   │  AT+CIPSEND=0,128            │
```

关键认知：**ESP8266 只是一个透传通道**。浏览器发 HTTP 请求 → ESP8266 以 `+IPD` 帧格式推给 STM32 → STM32 解析 HTTP 请求路径 → 构造 JSON 响应 → 通过 AT+CIPSEND 发回 ESP8266 → ESP8266 转发给浏览器。

---

## 2. 核心难题：CIPMUX 模式冲突

这是整个上位机开发中最深的一个坑，也是大部分 bug 的根源。

### 2.1 两种模式

| 模式 | 指令 | 特点 | 用途 |
|------|------|------|------|
| CIPMUX=0 | `AT+CIPMUX=0` | 单连接，`+IPD` 无连接 ID | WiFi 扫描、HTTP 请求、AI 对话、时钟同步 |
| CIPMUX=1 | `AT+CIPMUX=1` | 多连接，`+IPD` 带连接 ID | WebServer（CIPSERVER 必须是多连接模式） |

### 2.2 为什么不能同时用

- **WebServer 需要 CIPMUX=1**：`AT+CIPSERVER=1,80` 启动 TCP 服务器，同时允许多个浏览器连接。
- **HTTP 请求需要 CIPMUX=0**：`AT+CIPSTART` 建立 SSL 连接去访问阿里云 DashScope 或时钟同步服务器，单连接模式更稳定。

两者互斥，**同一时刻只能选一种**。

### 2.3 解决方案：动态切换

```
正常状态：CIPMUX=1，WebServer 运行，等待浏览器请求
     │
     │ 需要 AI 对话 / 时钟同步 / WiFi 扫描
     ├──> WebServer_Stop()  →  AT+CIPSERVER=0  →  AT+CIPMUX=0
     │    执行 HTTP 请求
     └──> WebServer_Start() →  AT+CIPMUX=1    →  AT+CIPSERVER=1,80
     │
     │ 回到正常状态，浏览器继续可以访问
```

代码位置：
- `WebServer_Stop()` / `WebServer_Start()`：在 `web_server.c` 中
- 切换逻辑：在 `freertos.c` 的每次 HTTP 请求（时钟同步、AI 对话）前后调用

### 2.4 切换的副作用

- **切换期间浏览器会短暂离线**：CIPSERVER 关闭时，浏览器请求会超时。一般 2-3 秒内恢复。
- **AI 对话期间网页不可用**：AI 对话可能持续 5-15 秒，期间 WebServer 关闭。
- **WiFi 扫描时网页不可用**：扫描耗时约 5 秒，同理。

---

## 3. WebServer API 接口

### 3.1 路由表

| 路径 | 方法 | 返回 | 说明 |
|------|------|------|------|
| `/` | GET | HTML | 精简版监控页面（方式1） |
| `/index.html` | GET | HTML | 同上 |
| `/api/sensors` | GET | JSON | 温度、湿度、气压、海拔 |
| `/api/status` | GET | JSON | 芯片型号、屏幕、传感器状态、运行时间 |
| `/api/ping` | GET | JSON | 连通性测试 `{"ok":true}` |
| 其他 | GET | 404 JSON | `{"error":"not found"}` |

### 3.2 响应格式示例

**GET /api/sensors**：
```json
{
    "temp": 25.3,
    "humi": 62.1,
    "pres": 1013.2,
    "alti": 152.8
}
```

**GET /api/status**：
```json
{
    "chip": "STM32F407",
    "lcd": "3.5inch",
    "sensor": "AHT20+BMP280",
    "uptime_sec": 1234,
    "uptime_str": "20m 34s"
}
```

### 3.3 CORS 跨域

所有响应都带 `Access-Control-Allow-Origin: *`，所以浏览器不管从哪个域名（包括 `file://` 本地文件）访问都不会被拦截。

---

## 4. 两种访问方式

### 方式1：浏览器直接访问 ESP8266

```
浏览器输入: http://<ESP8266的IP>/
```

- 优点：不需要任何文件，任何设备打开浏览器就能用
- 缺点：页面是嵌入在固件里的精简版 HTML，功能有限

### 方式2：本地打开 HTML 文件（推荐）

```
1. 用浏览器打开项目根目录的 index.html
2. 页面顶部会提示输入设备 IP
3. 从 STM32 屏幕上获取 IP（时钟同步成功后显示）
4. 输入 IP 后点击连接
```

- 优点：页面功能完整，可以定制，不受固件大小限制
- 缺点：需要知道设备 IP，首次配置需要手动输入

### 方式2 的跨域原理

```javascript
// index.html 中的关键代码
function getApiUrl(path) {
    if (window.location.protocol === 'file:') {
        // 本地文件通过 file:// 打开，直接请求 http://IP
        return `http://${DEVICE_IP}${path}`;
    }
    // 线上部署时直接用相对路径
    return path;
}
```

浏览器从 `file://` 协议加载页面时，API 请求会自动指向 `http://<设备IP>/api/...`，配合 WebServer 的 CORS 头，跨域无问题。

---

## 5. 如何获取设备 IP

设备 IP 会在时钟同步成功后打印到屏幕上，显示在"时钟同步成功"的下一行。

代码位置：`freertos.c`，时钟同步成功后：

```c
if (esp_try_sync_time_once()) {
    esp_log_update_last("时钟同步成功");
    // 通过 AT+CIFSR 查询 STAIP
    // 解析 "STAIP,\"xxx.xxx.xxx.xxx\"" 格式
    // 打印到屏幕
}
```

---

## 6. 踩过的坑

### 坑 1：WebServer 导致 HTTP 请求失败

**现象**：加上 WebServer 后，时钟同步一直失败，AI 对话返回错误 1。

**根因**：WebServer 启动时设置了 `CIPMUX=1`，而后续的 HTTP 请求（`AT+CIPSTART`）在 CIPMUX=1 模式下行为不同——立即返回 OK 但 SSL 连接未就绪，导致后续 `AT+CIPSEND` 超时。

**修复**：在每次 HTTP 请求前后做 Stop/Start 切换（见第 2.3 节）。

### 坑 2：AI 对话返回错误 1（ESP_ERR_TIMEOUT）

**现象**：EspStatus = 1，AI 无任何回复内容。

**根因**：CIPMUX=1 模式下 `AT+CIPSTART` 立即返回 OK，但 SSL 握手实际需要几秒。代码在 OK 后立即发 `AT+CIPSEND`，连接还没建立就超时了。

**修复**：在 `ESP_HTTPPostAuth` 中添加等待逻辑，直到收到 `CONNECT` 通知才继续。

### 坑 3：`+IPD` 帧格式因 CIPMUX 不同而变化

**现象**：加 WebServer 后，原来的 HTTP 响应解析失败。

**根因**：
- CIPMUX=0 时：`+IPD,<长度>:<数据>`
- CIPMUX=1 时：`+IPD,<连接ID>,<长度>:<数据>`

两个模式的 `+IPD` 格式不同，多了一个连接 ID 字段。如果在 CIPMUX=1 时解析 CIPMUX=0 格式的帧，会误把连接 ID 当成长度，导致数据错乱。

**修复**：确保 HTTP 请求时切换到 CIPMUX=0，WebServer 时用 CIPMUX=1，各自解析各自的格式。

### 坑 4：网页显示离线

**可能原因**（按排查顺序）：

1. **IP 地址不对**：ESP8266 重启后路由器可能分配了新 IP。查看屏幕上的 IP 显示。
2. **WebServer 没启动**：检查 WebServer_Start 是否成功调用（观察串口日志）。
3. **防火墙拦截**：PC 防火墙可能阻止了到 80 端口的连接。
4. **不在同一网络**：确保 PC/手机和 ESP8266 连的是同一个 WiFi。
5. **CIPMUX 切换时机**：如果在 AI 对话期间访问网页，WebServer 是关闭的，会显示离线。

**快速诊断**：浏览器直接访问 `http://<IP>/api/ping`，如果返回 `{"ok":true}` 说明网络通、WebServer 正常。

---

## 7. 文件清单

| 文件 | 作用 |
|------|------|
| `HARDWARE/WEB_SERVER/web_server.c` | HTTP 服务器：解析请求、路由分发、构造响应 |
| `HARDWARE/WEB_SERVER/web_server.h` | WebServer 接口：Start/Stop/Poll |
| `HARDWARE/ESP8266/esp8266.c` | ESP8266 驱动：AT 命令、HTTP/HTTPS 通信 |
| `Core/Src/freertos.c` | 任务调度：时钟同步、AI 对话、WebServer 生命周期管理 |
| `index.html` | Web 前端：传感器面板、设置页面、IP 配置 |

---

## 8. 串口调试命令

```bash
# 确认 WebServer 是否运行
AT+CIPSERVER?          # 返回 +CIPSERVER:1,80 表示正在监听

# 查看当前 CIPMUX 模式
AT+CIPMUX?             # 返回 +CIPMUX:0 或 +CIPMUX:1

# 查看 ESP8266 的 IP
AT+CIFSR               # 返回 STAIP,"xxx.xxx.xxx.xxx"

# 查看当前 WiFi 连接状态
AT+CWJAP?              # 返回 +CWJAP:"ssid","bssid",channel,rssi
```