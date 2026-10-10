能用 1 分钟讲清背景和目标，用 3 分钟讲清架构和主链路
能明确自己负责的模块、接口、上下游
至少有 1 个完整问题定位案例：现象 → 假设 → 定位 → 方案 → 验证
至少有 1 个设计取舍：为什么这样选，不选另一种的原因
技术点能回到代码：初始化、中断、任务、共享数据、错误处理
结果尽量可验证：稳定性、时延、丢包、启动时间、功耗、故障恢复

========== 第一层：介绍 ==========

请用 1 分钟介绍你的主项目
--------------------------------
基于 STM32F407VET6 + FreeRTOS + LVGL 的多功能桌面AI交互机器人，用一块 3.5 寸彩屏
做交互。硬件集成了 AHT20 温湿度、BMP280 气压传感器，ESP8266 WiFi 模块，ASR PRO
离线语音识别，SD 卡存储。

软件上做了这几件事：
1）ESP8266 通过 HTTPS 直连阿里云 DashScope 大模型（Qwen），实现 AI 对话；
2）拼音输入法+虚拟键盘，让用户在触摸屏上打字和 AI 聊天；
3）一套 Web 上位机，浏览器访问设备 IP 就能看到实时传感器数据和设备状态；
4）语音控制，喊关键词触发机器人表情动画或功能跳转；
5）Bootloader + IAP 固件升级，支持 SD 卡 / UART / WiFi 三种升级通道。

代码参考：
- main.c: 系统启动入口，外设初始化，启动调度器
- freertos.c: 5个RTOS任务 + 4个消息队列 + 所有UI界面
- Bootloader/bootloader.c: 双分区引导 + 三种IAP升级通道

这个项目要解决的核心问题是什么
--------------------------------
在资源极度受限的 MCU（512KB Flash / 192KB SRAM）上，实现一个"有屏幕、能联网、
能AI对话、能语音控制、能浏览器远程监控、还能空中升级"的完整智能设备。

核心挑战不是单一功能难，而是"所有功能挤在一块小芯片上还不能崩"：
- 192KB SRAM 要装 FreeRTOS 内核 + LVGL 图形库 + 5个任务栈 + WiFi协议栈缓冲
- 512KB Flash 要装 Bootloader + App双分区 + 中文字体位图
- 屏幕、WiFi、语音、传感器四路数据并发，不能互相拖死

你在项目中的个人职责边界是什么
--------------------------------
全栈——从 Bootloader 到底层驱动到应用层 UI 到 Web 服务器全部一个人搞完。

具体包括：
- Bootloader: SD/UART/WiFi 三种 IAP 升级 + App 校验与跳转
- 驱动层: AHT20/BMP280(I2C)、LCD(FSMC)、触摸(I2C)、ESP8266(UART3)、ASR(UART1)、SDIO
- 中间件: FatFS文件系统、LVGL移植、CJK中文字体、拼音输入法引擎
- 应用层: 5个FreeRTOS任务、AI对话(HTTPS+JSON)、WebServer(HTTP路由)、机器人表情动画
- 稳定性: 内存碎片化根因分析、栈溢出排查、流式数据分片重组、CIPMUX模式冲突

系统整体如何分层
--------------------------------
上电 → Bootloader(0x08000000) → 校验App有效性 → 跳转App(0x08008000)
→ HAL_Init → SystemClock_Config(168MHz, HSE 8MHz → PLL) → 外设初始化
→ osKernelInitialize → MX_FREERTOS_Init(5任务+4队列) → osKernelStart

代码：main.c 第88-163行

分层架构（自底向上）：
┌────────────────────────────────────────────────────┐
│ 应用层: AI对话 / WiFi管理 / 时钟日历 / 环境显示    │
│         机器人表情动画 / 拼音输入法 / 语音控制      │
│         代码: freertos.c 中5个任务函数              │
├────────────────────────────────────────────────────┤
│ 中间件: LVGL 8.3 + CJK字体 + 拼音IME引擎           │
│         代码: LVGL/ + HARDWARE/LVGL/lv_port_*.c     │
├────────────────────────────────────────────────────┤
│ 协议栈: HTTP(AI对话) + WebServer + AT指令 + JSON    │
│         代码: web_server.c + esp8266.c              │
├────────────────────────────────────────────────────┤
│ 文件系统: FatFS + SDIO（字库/固件/日志存SD卡）      │
│         代码: FATFS/src/ + sdio_sdcard.c            │
├────────────────────────────────────────────────────┤
│ 驱动层: AHT20/BMP280(I2C) + LCD(FSMC) + 触摸(I2C)  │
│         ESP8266(UART3) + ASR(UART1) + SDIO(SD卡)    │
│         代码: HARDWARE/ 下各子目录                   │
├────────────────────────────────────────────────────┤
│ HAL: STM32 HAL库 + CMSIS                           │
│         代码: Drivers/ + Core/Src/                  │
├────────────────────────────────────────────────────┤
│ Bootloader: 独立工程，三种升级通道，双分区引导       │
│         代码: Bootloader/                           │
└────────────────────────────────────────────────────┘

如果重做一遍，你会先改哪一块
--------------------------------
内存布局。CCM RAM（0x10000000, 64KB）一开始完全闲置，白扔了。应该一开始就把
LVGL 内存池分配在 CCM（LV_MEM_ADR 0x10001000），这样主 SRAM 空出 20KB+，
很多后续的栈溢出和碎片化问题根本不会发生。

代码证据：lv_conf.h 中 LV_MEM_ADR 配置，以及 stm32_c内存优化_CCM与scatter.md 笔记。


========== 第二层：架构 ==========

模块之间的接口怎么定义
--------------------------------
任务间通信全部走 FreeRTOS 消息队列，不直接共享全局变量。队列声明在 freertos.c：

  队列名称            | 容量 | 消息大小 | 生产者          | 消费者
  -------------------|------|---------|----------------|----------
  esp_rx_queue       | 4    | 128B    | esp8266Task    | lvglTask
  esp_ai_queue       | 1    | 512B    | esp8266Task    | lvglTask
  esp_ai_req_queue   | 1    | 256B    | lvglTask       | esp8266Task
  asr_queue          | 4    | 8B      | asrTask        | lvglTask
  sensor_queue       | 4    | 16B     | sensorTask     | lvglTask

代码：freertos.c 第280-310行（MX_FREERTOS_Init 中的 osMessageQueueNew）

为什么这样设计：
- esp_ai_queue 容量=1：AI 对话是串行请求-响应模式，同一时间只需要一个回答
- sensor_queue 容量=4：传感器采集周期200ms，UI刷新周期30ms，4个缓冲足够平滑
- 所有数据最终汇入 lvglTask：因为 LVGL 不是线程安全的，所有 UI 操作必须在
  同一个任务（lvglTask）里执行，其他任务只负责"生产"数据扔队列。

共享资源怎么保护
--------------------------------
1. I2C 总线：AHT20(0x38) + BMP280(0x76) 共用 I2C1。目前都在 sensorTask 同一个
   任务里访问，暂不需要互斥锁。如果后续加 MPU6050 也在这条总线上且有独立任务
   访问，需要用互斥锁保护 I2C 访问。

2. ESP8266 串口：esp8266Task 独占 USART3 的发送和接收，其他任务通过消息队列
   间接通信，不会同时操作串口。

3. LVGL 内存池：所有 LVGL 对象操作都在 lvglTask 中执行，单任务访问 LVGL 池，
   天然线程安全。

4. WiFi 配置读写（wifi_config_load/save）：目前读写 SD 卡上的配置文件，没有
   显式的互斥保护。如果 sensorTask 也要写 SD 卡（比如数据记录），需要加互斥锁。

5. 关键全局标志（wifi_connected, chat_pending 等）：声明为 volatile int，
   单写多读场景，Cortex-M4 单核 + 对齐访问保证原子性，未用互斥锁。


========== 第三层：关键路径 ==========

数据从传感器进来后完整路径是什么
--------------------------------
AHT20 / BMP280 (I2C1, PB6-SCL / PB7-SDA, 100kHz)
  ↓ sensorTask (200ms周期, osPriorityNormal, 4KB栈)
  ATH20_Read_CTdata() → 温湿度 / BMP280_Read() → 气压海拔
  代码：ATH20.c 第30-92行, BMP280.c
  ↓ 填充 sensor_data_t 结构体
  osMessageQueuePut(sensor_queue)  — 非阻塞
  ↓
lvglTask (osPriorityAboveNormal, 16KB栈)
  轮询 osMessageQueueGet(sensor_queue, 0超时)
  → 更新 env_temp_label / env_humi_label / env_press_label / env_alti_label
  → WebServer 也可以通过 g_web_sensor 全局变量读取最新值

控制命令从上位机到执行器怎么走
--------------------------------
【语音控制路径】
ASR PRO 离线语音模块 (UART1, PA9/PA10, 115200)
  ↓ 每收到一个字节 → USART1_IRQHandler → HAL_UART_IRQHandler
  → ASR_RxCallback() (中断上下文，逐字节填充缓冲区)
  代码：stm32f4xx_it.c 第193行, asr_pro.c 第30-55行
  ↓ 收到 0x0A/0x0D（换行/回车）→ 置 data_ready=1
  ↓
asrTask (osPriorityNormal, 4KB栈)
  ASR_GetMessage() → 非阻塞取帧 → 超时判帧(ASR_RX_TIMEOUT_MS)
  → 解析 ASCII '1'~'9' → cmd_id
  osMessageQueuePut(asr_queue)
  ↓
lvglTask 取出 cmd_id → 执行对应动作（切换页面/触发表情/唤醒等）

【AI对话控制路径】（最复杂路径）
用户触摸输入 (拼音IME)
  ↓ lvglTask 中事件回调
  chat_send_to_ai(user_msg)
  osMessageQueuePut(esp_ai_req_queue)  — 通知esp8266Task
  ↓
esp8266Task (osPriorityNormal, 8KB栈)
  取出请求 → WebServer_Stop()（关闭CIPSERVER，释放CIPMUX=1）
  → ESP_SendCmd("AT+CIPMUX=0")（切换到单连接模式）
  → ESP_SendCmd("AT+CIPSSLCSNI=dashscope.aliyuncs.com")（设置 SNI）
  → ESP_SendCmd("AT+CIPSTART=SSL,...,443")（建立 SSL 连接）
  → 拼接 HTTP POST 请求（JSON body 含对话历史 chat_history[]）
  代码：freertos.c 中 chat_build_json() + esp8266StartTask()
  ↓ 收 +IPD 分片 → ESP_RecvResponse() 流式重组（scan 指针机制防丢包）
  extract_content() → 从 JSON 中抠出 assistant 的 content
  osMessageQueuePut(esp_ai_queue)
  ↓
lvglTask 取出 → chat_add_message("assistant", ...) → chat_ui_update()
  → WebServer_Start()（恢复CIPSERVER，浏览器恢复可用）

哪些逻辑在中断里，哪些在任务里
--------------------------------
中断里只做"最小动作"——收字节进缓冲区，不解析、不耗时：

  中断                | 只做什么                          | 代码位置
  -------------------|----------------------------------|---------------------------
  USART1_IRQHandler  | 逐字节收 ASR 数据到 rx_buf       | stm32f4xx_it.c:187
  USART3_IRQHandler  | 逐字节收 ESP8266 数据到 rx_buf   | stm32f4xx_it.c:200
  SDIO_IRQHandler    | HAL 库 SD 卡 DMA 传输完成回调     | stm32f4xx_it.c:222
  DMA2_Stream3/6     | SD 卡读写 DMA 完成通知            | stm32f4xx_it.c:231/243
  TIM1_UP_TIM10      | FreeRTOS 系统 tick                | stm32f4xx_it.c:174

任务里做：
- 解析协议帧（ASR_Parse, ESP_RecvResponse）
- HTTP/JSON 处理（chat_build_json, extract_content）
- UI 渲染和事件（lv_timer_handler, 所有 lv_event_cb）
- 传感器数据采集和计算（ATH20_Read_CTdata, BMP280_Read）
- 文件系统操作（FatFS 读写 SD 卡）

任务如何划分，优先级如何定
--------------------------------
lvglTask:    osPriorityAboveNormal (高)  — UI 是用户最直接的感受，30ms 刷新
defaultTask: osPriorityNormal                  — 呼吸灯，最低负载
esp8266Task: osPriorityNormal                  — 网络 I/O 重但可等待
asrTask:     osPriorityNormal                  — 语音数据量小
sensorTask:  osPriorityNormal                  — 200ms 周期，I2C 访问快

设计原则：
- lvglTask 优先级最高：用户交互不能被网络/传感器阻塞
- 网络任务优先级不最高：网络本身就有不可控延迟，优先级再高也没用
- 传感器和语音同级：数据量小、周期固定，不影响 UI

有没有单点故障，怎么处理
--------------------------------
有。ESP8266 是最大的单点——它挂了，AI 对话、WebServer、时钟同步、WiFi 升级
全挂。如果 App 运行中 ESP8266 无响应，目前的 ESP_SendCmd 有超时返回机制
（ESP_ERR_TIMEOUT），UI 会显示连接失败，但不会自动复位 ESP8266。

改进方向：在 esp8266Task 里检测连续多次超时后，通过 GPIO 拉低 ESP8266 RST 脚
做硬件复位，然后重新走初始化流程。代码目前未实现这个自愈机制。

另一个单点：SD 卡。Bootloader 固件包、字库文件都在 SD 卡上，SD 卡接触不良
会导致升级失败、字库无法加载。


========== RTOS / 并发 / 中断 ==========

为什么上 RTOS，不上行不行
--------------------------------
不行。理由有三：

1. 功能并发：屏幕要刷新(30ms)、传感器要采集(200ms)、WiFi 要收包(不确定延迟)、
   语音要解析、AI 回复要等 5~15 秒。裸机只用一个大 while(1) 轮询，任何一个
   环节阻塞（比如等 ESP8266 AT 指令返回 OK，可能等 15 秒），屏幕就卡死 15 秒。

2. 网络请求天然异步：ESP8266 的 AI 对话是"请求→等几秒→收回答"模式。裸机若
   在 while(1) 里阻塞等，整个系统停摆。RTOS 让 esp8266Task 阻塞等队列消息，
   lvglTask 继续渲染 UI，互不影响。

3. LVGL 强依赖 tick：LVGL 需要 1~5ms 的周期性 `lv_timer_handler()` 调用。
   FreeRTOS 的 `vTaskDelay(30)` 比裸机的 `delay_ms(30)` 精确得多，且不浪费 CPU。

队列、信号量、互斥锁在项目里分别解决什么问题
--------------------------------
队列（Message Queue）：最多用，解决"任务间异步传数据"。
- 传感器数据 → UI，AI 回复 → UI，语音命令 → UI，用户输入 → WiFi 任务
- 生产者只负责扔数据，消费者只负责取数据，互不阻塞

信号量（Semaphore）：目前未显式使用二元/计数信号量。
- 潜在的场景：WebServer_Start/Stop 切换时，用信号量通知 esp8266Task
  "模式已切换完毕"，避免竞态。

互斥锁（Mutex）：目前未显式使用。
- 潜在需求：I2C 总线保护（如果 MPU6050 独立任务访问同一条总线）
- SD 卡写保护（如果 sensorTask 也要写 SD）

中断里能做什么，不能做什么
--------------------------------
能做的：
- 逐字节填充环形/UART 接收缓冲区（ASR_RxCallback, ESP_RxCallback）
- 设置标志位（data_ready=1）
- 记录时间戳（last_rx_tick = HAL_GetTick()）
- HAL 库的中断处理回调（HAL_UART_IRQHandler, HAL_SD_IRQHandler）

不能做的：
- 调用 osDelay / vTaskDelay（中断没有任务上下文，不能阻塞）
- 调用 osMessageQueuePut 等可能阻塞的 API（除非用 FromISR 版本）
- 做字符串解析、JSON 提取、HTTP 解析（耗时长，会阻塞同优先级和低优先级中断）
- 操作 LVGL 对象（不是线程安全，且不能在中断上下文操作）

当前项目的中断设计是干净的：所有解析都在任务里做，中断只负责收字节。

是否遇到过栈溢出，怎么查
--------------------------------
遇到过。详见「内存与栈学习笔记.md」完整记录。

现象：拼音输入法连续打字到第 3 轮必死机，LED 呼吸灯也卡住不闪 → 整机崩溃。

定位三步走：
1）怀疑 snprintf 数组越界 → 修复了 chat_build_json 的累加问题（是真 bug 但不是元凶）
2）怀疑打字路径必然触发 → 指向内存问题
3）打开 MDK-ARM/ai_project/ai_project.map 文件 → 看执行区真实占用：
   RW_IRAM1 Size=0x19b80(≈105KB)/Max=0x1c000(112KB)
   RW_IRAM2 Size=0x01b68(≈7KB)/Max=0x4000(16KB)
   主 SRAM 128KB 用了 112KB，只剩 16KB！
   而 LVGL 内存池只有 20KB → 碎片化 → lv_mem_alloc 返回 NULL → HardFault

修复（四个改动，一套解决）：
- LVGL 池 20KB→48KB，搬到闲置的 64KB CCM RAM（0x10001000）
- FreeRTOS 堆 25KB→40KB
- lvglTask 栈 8KB→16KB
- esp8266Task 栈 4KB→8KB

验证方法：反复打字 20 轮不崩。还加了 uxTaskGetStackHighWaterMark 监控。

是否遇到过优先级反转或任务饿死
--------------------------------
目前没有遇到明确的优先级反转问题（因为没有使用互斥锁，不存在优先级继承的场景）。

潜在风险：如果后续对 I2C 加互斥锁，sensorTask(优先级Normal) 持有 I2C 锁期间被
lvglTask(优先级AboveNormal) 抢占，而 lvglTask 也在等同一把锁 → 可能触发。
但当前 lvglTask 不直接访问 I2C，所以暂时安全。

饿死风险：sensorTask/asrTask/esp8266Task 同优先级(Normal)，FreeRTOS 时间片
轮转调度，不会饿死。lvglTask 虽然是 AboveNormal，但它在 osDelay(30) 时会让出
CPU，不会把 Normal 任务饿死。

看门狗怎么配，喂狗策略是什么
--------------------------------
当前代码未显式配置独立看门狗（IWDG）。FreeRTOS 配置了
configCHECK_FOR_STACK_OVERFLOW=2（栈溢出检查），结合
vApplicationStackOverflowHook 在栈溢出时能立刻停下。

IWDG 推荐策略：
- 预分频 32 → LSI(≈40kHz) / 32 = 1250Hz
- 重装载值 1250 → 超时时间 1 秒
- 喂狗放在 idle hook 或一个专门的 monitorTask(最低优先级)
- 如果 idle 任务能跑说明所有高优先级任务没有死循环
- 配合 `configUSE_IDLE_HOOK` 和 `configUSE_TICK_HOOK`


========== 驱动 / 外设 / 协议 ==========

某个外设初始化时序你怎么确认是对的
--------------------------------
以 LCD 为例（FSMC 8080 并口）：
- 初始化：LCD_Init() 先读 LCD ID 寄存器 → 根据 ID 选择对应的初始化序列
- 验证：初始化完成后在 LCD 上画纯色 → 用逻辑分析仪抓 FSMC 写时序
- 关键参数：FSMC 地址建立时间、保持时间要和 LCD 数据手册匹配

以 AHT20 为例（I2C）：
- 初始化：ATH20_Init() 发软复位命令(0xBA) → 等待 → 发初始化命令(0xBE+0x08,0x00)
- 验证：读状态寄存器 Bit[3]（校准使能位），为 1 表示就绪
- 关键时序：初始化后需等待 500ms（手册要求 300ms 以上）
- 代码：ATH20.c 第62-92行

以 ESP8266 为例（UART）：
- 验证：上电发 "AT\r\n"，看是否回 "OK"
- 如果模块无响应：检查波特率是否匹配（默认 115200），TX/RX 是否反接

SPI/I2C/UART 出过哪些典型问题
--------------------------------
I2C：
- AHT20 初始化后读状态一直 Bit[3]!=1 → 是因为等待时间不够或芯片还在忙
  → 加了 while 重试循环（最多 10 次）+ 每次等 500ms
- 两条 I2C 设备地址冲突：好在 AHT20(0x38) 和 BMP280(0x76) 地址不冲突

UART（ESP8266）：
- 不定长数据分片到达 → +IPD 帧头到齐了但 1195 字节 body 还没到齐
  → 旧代码直接"跳过"（i=len;break），数据永久丢失
  → 改为 scan 指针机制：没到齐就 break，下次从老位置重扫
- SNI 问题 → TLS 握手失败，连接秒断
  → 加 AT+CIPSSLCSNI 一行解决（DEBUG.md 坑3）
- 中文乱码 → UTF-8 vs GBK 编码混淆
  → 最终 prompt 改英文，绕过中文字库问题

SPI（W25QXX Flash）：
- 未遇到严重问题。BaudRatePrescaler=DIV2，42MHz，在 168MHz APB2 下是 84MHz/2=42MHz
  → W25QXX 最大支持 80MHz，安全范围内

DMA 缓冲为什么放在那段内存
--------------------------------
SD 卡读写用了 DMA（hdma_sdio_rx / hdma_sdio_tx），DMA 缓冲区在 SRAM（0x20000000
主 SRAM 区域），不能放在 CCM（0x10000000），因为 DMA 控制器访问不到 CCM。

LCD 不走 DMA——它用 FSMC 8080 并口，CPU 直接往 0x68000000 发地址写数据，
硬件自动产生读写时序，不占用 DMA 通道。颜色缓冲区也因此在 SRAM 而非 CCM。

这一点在「内存与栈学习笔记.md」中有详细说明：CCM 只能 CPU 访问，不能 DMA，
所以 LVGL 控件对象池可以放 CCM，但 DMA 缓冲绝对不能。

不定长数据怎么判断一帧结束
--------------------------------
【ASR PRO 语音模块】—— 定界符判帧
- 以 0x0A(\n) 或 0x0D(\r) 为帧结束标志
- 同时加超时保护：如果收到数据后超过 ASR_RX_TIMEOUT_MS 毫秒没有新字节，
  即便没收到换行符，也视为一帧结束
- 代码：asr_pro.c 第30-78行

【ESP8266 +IPD 帧】—— 长度字段判帧
- 帧格式：+IPD,<len>:<data>
- 先找到 "+IPD," 帧头 → 解析出 data_len → 判断缓冲区里是否收够了 data_len 字节
- 没到齐就 break，等下次轮询重扫（scan 指针不前进）
- 代码：esp8266.c 中 ESP_RecvResponse 的 scan 指针机制

协议如何做超时、重传、校验
--------------------------------
【AT 指令——超时+重试】
- ESP_SendCmd 有 timeout 参数（如 2000ms），超时返回 ESP_ERR_TIMEOUT
- ESP_ConnectAP 有 ESP_MAX_RETRY 次重试，每次重试前 osDelay(1000)
- 代码：esp8266.c 第65-85行

【AI 对话 HTTP——超时】
- 接收超时设为 60 秒（因为大模型冷启动可能超过 20 秒）
- 这是 DEBUG.md 坑4 的教训：不能拿本地 API 经验去套云大模型

【IAP 固件升级——CRC32】
- 升级包格式：header(magic+version+size+crc32) + payload
- Bootloader 接收时边收边算 CRC → 写完 Flash 后再读一遍校验 CRC
- 三端 CRC 一致才认为升级成功
- 代码：bootloader.c 中 crc32_update() 函数

【HTTP 响应——Connection: close】
- 请求头带 Connection: close，服务器回完就主动关连接
- STM32 收到 CLOSED 就知道"收完了"，不用猜长度

如何区分是硬件问题还是软件问题
--------------------------------
经典判断方法：
1. 换一块相同的硬件模块 → 问题消失则源模块硬件坏
2. 同一块板烧回旧版本固件 → 问题消失则是新代码引入的软件 bug
3. 用逻辑分析仪/示波器抓信号 → 波形不对则硬件/时序问题

实际案例（DEBUG.md）：
- 串口助手显示"浣犲ソ"，以为是数据传错了或 AT 指令发错了格式
  → 直接用十六进制看原始字节 → 发现字节是对的（UTF-8 编码的中文）
  → 是显示端（串口助手）用 GBK 解码的问题 → 软件问题（编码理解错误）
- ESP8266 AT+CIPSEND 成功但没有 +IPD 响应
  → 开始怀疑被动模式、缓冲大小、超时（都是软件层）
  → 最终发现是 SNI(TLS 层) → 属于协议层问题，硬件没问题


========== 稳定性 / 升级 / 调试 ==========

项目里最难的 bug 是什么
--------------------------------
LVGL 内存碎片化导致的第 3 次打字必死机。

为什么难：
- 崩溃点"随机"：不是固定的某行代码崩，是 lv_mem_alloc 返回 NULL 后被其他代码
  解引用时崩
- 症状间接：崩在完全不相干的地方（因为空指针解引用导致 HardFault）
- 编译器优化：加一行 printf 后栈布局变了，可能就不崩了——经典的堆栈踩踏症状

你当时最先看到的异常现象是什么
--------------------------------
- LED 呼吸灯卡住不闪了（说明不是 UI 线程卡顿，是整机 HardFault 了）
- 连续打字到第 3 轮才崩（不是第 1 次就崩，说明是累积性问题）
- 不是每次都崩在同一个输入（碎片化程度和布局有关，有随机性）

你如何缩小问题范围
--------------------------------
1. 先修了一个已知的 snprintf 越界 → 修了但没根治，说明还有更深的问题
2. 注意到"第 3 次必崩"且和"打字"强相关 → 指向内存
3. 打开 .map 文件看真实内存占用 → RW_IRAM1 112KB/128KB！只剩 16KB
4. 结合 LVGL 内存池只有 20KB → 碎片化是唯一合理的解释
5. 假设：加大 LVGL 池 → 如果第 10 次才崩，证实碎片化假设

最终怎么证明修掉了，而不是碰巧消失
--------------------------------
- 反复打字 20 轮不崩（修复前第 3 轮必崩）
- 加了 uxTaskGetStackHighWaterMark 监控，各任务剩余栈 > 20%
- LVGL 池从 20KB 搬到 48KB CCM，主 SRAM 空出约 40KB
- 修复是可解释的（碎片化 → 搬到独立 CCM 池 → 碎片化影响隔离），不是碰运气

有没有做固件升级，掉电怎么办
--------------------------------
有。三种通道：SD 卡、UART(IAP)、ESP8266 WiFi(TCP 服务器)。

升级包格式：
  header: magic(4B) + version(4B) + image_size(4B) + image_crc32(4B)
  payload: 实际固件 .bin

掉电保护策略：
- Bootloader 先完整接收 payload 并写入 Flash
- 写入完成后 CRC 三端校验（接收中计算 / 写入后回读 / header 中记录）
- 校验通过才标记升级完成并跳转
- 如果掉电发生在写入过程中：Flash 中残留数据不完整，下次上电 Bootloader
  校验失败 → 重新进入 IAP 等待升级
- 当前没有 A/B 分区双备份回滚机制（Flash 空间不够 512KB/2=256KB+APP）

代码：bootloader.c 中 boot_try_update_from_sd/uart/esp 三个函数

Bootloader 和 APP 如何跳转与校验
--------------------------------
分区布局：
  0x08000000  Bootloader (32KB)
  0x08008000  App

上电流程：CPU 从 0x08000000 取 MSP → 取 Reset_Handler → 执行 Bootloader
  ↓
读取 App 开头两个 32 位数据：
  app_stack  = *(uint32_t*)0x08008000  → 初始 MSP 栈顶地址
  app_reset  = *(uint32_t*)0x08008004  → Reset_Handler 入口地址
  ↓
如果 app_stack 和 app_reset 都是合法地址（不是 0xFFFFFFFF 且在线性 Flash 范围内）
  ↓
SCB->VTOR = 0x08008000;    // 中断向量表偏移到 App 的向量表
__set_MSP(app_stack);      // 切换到 App 的主栈
app_entry();               // 跳到 App 的 Reset_Handler（永不返回）

关键：VTOR 决定中断向量表从哪取，MSP 决定主栈起始位置。任一步漏掉，App 表面
启动了但进中断就崩。

代码：bootloader.c 中 boot_jump_to_app() + boot_is_valid_app()

日志和断言在定位里起了什么作用
--------------------------------
串口日志（app_log/boot_log）是定位启动问题的唯一手段——它不依赖 LCD、LVGL、
文件系统，只要 USART 初始化就能用：

  [BOOT] start                    ← 证明芯片上电正常
  [BOOT] before sdio init         ← 证明代码跑到这里了
  [BOOT] after sdio init          ← 证明 SDIO 初始化没卡死
  [APP] uart ok / spi ok / ...    ← 逐个外设验证

定位方法：如果只看到 "before foo" 看不到 "after foo"，说明 foo() 里卡死或
HardFault 了。这样能把"屏幕白屏"这种毫无线索的问题，缩小到具体某一行代码。

断言：代码中使用了 Error_Handler()（HAL 库默认），会卡在 while(1) 里。
改进方向：在 Error_Handler 中加串口打印文件名和行号（__FILE__, __LINE__），
方便快速定位。

有哪些故障注入或边界测试
--------------------------------
当前没有系统化的故障注入。可以补充的测试：
1. 拔掉 SD 卡 → Bootloader 应进入 IAP 等待，App 应优雅降级（字库不可用）
2. 拔掉 ESP8266 电源 → UI 应显示 WiFi 断开，不崩
3. 连续快速点击（事件风暴）→ LVGL 事件队列不能溢出
4. WiFi 密码错误 → 应超时退出不卡死
5. 堆接近耗尽 → 用 malloc 失败注入（hook lv_mem_alloc 随机返回 NULL）


========== 取舍与迁移 ==========

为什么选这个方案而不是另一个
--------------------------------
【ESP8266 AT 固件 vs 自己写 ESP SDK】
选 AT 固件：
- 优点：ESP8266 端不用改一行代码，STM32 端用熟悉的 AT 指令控制
- 缺点：AT+CIPSEND 先发长度再发数据，HTTP 请求必须完整拼好一次发；
        模式切换（CIPMUX=0/1）麻烦；效率不如透传
- 不选自写 SDK：时间成本太高，ESP8266 SDK 是另一套开发工具链

【FSMC 8080 并口 vs SPI 驱动 LCD】
选 FSMC 8080 并口：
- 优点：16 位数据总线，写一个像素只需一次 16 位写入，刷新快
- 缺点：占用 16 根数据线 + 4 根控制线 = 20 个 GPIO
- 不选 SPI：3.5 寸 480x320 分辨率，SPI 最高 42MHz，刷满一帧要几百毫秒，
  LVGL 界面延迟不可接受

【LVGL 8.3 vs 自己画 UI】
选 LVGL：
- 优点：成熟的开源 GUI 框架，自带 30+ 控件、动画系统、多国语言支持
- 缺点：ROM/RAM 占用量大，需要仔细裁剪 lv_conf.h
- 不选自绘：机器人表情动画（眨眼/眼球游走/说话嘴巴）用 LVGL 的动画系统
  几行代码搞定，自己画要写几百行

【CIPMUX 动态切换 vs 只用一种模式】
选动态切换：
- WebServer 必须 CIPMUX=1（多连接）
- AI HTTP 请求用 CIPMUX=0（单连接，更稳定）
- 两者互斥 → 在 HTTP 请求前后 Stop/Start WebServer
- 代价：切换期间浏览器 2~3 秒不可用
- 替代方案（选了但没用）：全用 CIPMUX=1 发 HTTPS 请求 → AT 指令交互更复杂

性能、功耗、稳定性之间你怎么权衡
--------------------------------
这个项目偏重「功能完整性」和「稳定性」，功耗不做优先考量（桌面设备插电用）。

权衡点：
1. 系统时钟 168MHz（最高）→ 性能最好，放弃低功耗
2. LVGL 池 48KB → 稳定性优先，宁可占 RAM 也不冒碎片化风险
3. lvglTask 栈 16KB → 稳定优先，给渲染路径留够余量
4. 传感器 200ms 周期 → 不追求实时（环境变化慢），省 CPU 给 UI

如果芯片 ROM/RAM 再砍一半，设计怎么改
--------------------------------
假设：256KB Flash / 96KB SRAM（砍一半）

1. 放弃 Bootloader 双分区 → 只保留 App，去掉 IAP 升级（省 32KB Flash）
2. 中文字体 138KB 不能放内部 Flash → 必须全放 SD 卡，lv_font_load 动态加载
3. LVGL 裁剪更激进：去掉 lv_roller, lv_table, lv_msgbox 等不用的控件
4. 对话历史从 20 轮砍到 5 轮（省约 4KB SRAM）
5. WebServer 页面精简到纯 JSON API，去掉完整 HTML 页面
6. 传感器采集周期从 200ms 拉长到 1s

如果要加 OTA/屏显/双核，现有架构哪里会爆
--------------------------------
加 OTA：
- 已经在用！三种通道。但当前没有 A/B 分区回滚。如果要加：
  → Flash 分区重新规划：0x08000000 Bootloader + 0x08008000 App_A + 0x08040000 App_B
  → F407 512KB Flash 不够两个 App(各约 200KB) → 需要换 H743(2MB Flash)

加更大屏幕（800x480）：
- LVGL 显示缓冲翻倍（全屏 buf 800*480*2=768KB → SRAM 直接爆）
  → 必须用 Partial Buffer（1/10 屏）或外挂 SDRAM(FSMC 外扩)

加双核（M7+M4）：
- FreeRTOS 单核模型不适用 → 需要 AMP(非对称多处理) 架构
- 核间通信：OpenAMP/rpmsg 或共享内存+中断通知
- 任务重新分配：UI 跑 M7(高性能)，传感器+WiFi 跑 M4
- 当前所有任务都在同一个核的概念要彻底改

加中文字库显示：
- 当前用英文 prompt 绕过了。如果要显示中文：
  → 方案：SD 卡存放字库 .bin → FatFS 读取 → lv_font_load 动态加载字形
  → 内存：动态加载不常驻内存，每次显示时加载当前需要的字形到临时缓冲区
  → RAM 影响很小（只需几 KB 字形缓存），不爆

加电池供电（功耗要求）：
- 当前功耗不在考量范围。要加电池需改：
  → 168MHz 降到 84MHz 或更低
  → ESP8266 用休眠模式，定时唤醒
  → LCD 背光 PWM 调低
  → 传感器采集周期拉长
  → 复杂度增加很大，架构需重新评估