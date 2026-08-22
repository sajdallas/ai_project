/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "lcd.h"
#include "touch.h"
#include "lvgl.h"

LV_FONT_DECLARE(lv_font_cjk16);
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "esp8266.h"
#include "pinyin_data.h"
#include "../HARDWARE/ASR_PRO/asr_pro.h"
#include "../HARDWARE/ROBOT_FACE/robot_face_anim.h"
#include "../HARDWARE/aht20_bmp280/ATH20.h"
#include "../HARDWARE/aht20_bmp280/BMP280.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum {
    ESP_MSG_INIT,
    ESP_MSG_AT_OK,
    ESP_MSG_AT_FAIL,
    ESP_MSG_WIFI_OK,
    ESP_MSG_WIFI_FAIL,
    ESP_MSG_RESP,
    ESP_MSG_UPDATE_LAST,
} esp_msg_type_t;

typedef struct {
    esp_msg_type_t type;
    char text[128];
} esp_msg_t;


/* ---- AI 对话 ---- */
#define CHAT_HISTORY_MAX 20          /* 最多保留 20 轮对话 */
#define CHAT_MSG_LEN     256         /* 每条消息最大长度 */

typedef struct {
    char role[16];                   /* "user"/"assistant"（"assistant" 含 \0 需 10 字节，8 会截断成 "assista"） */
    char content[CHAT_MSG_LEN];
} chat_msg_t;

static chat_msg_t chat_history[CHAT_HISTORY_MAX];
static int chat_count = 0;           /* 当前对话轮数 */
static lv_obj_t *chat_list;          /* 对话显示列表 */
static lv_obj_t *chat_input;         /* 用户输入框 */
static lv_obj_t *chat_send_btn;      /* 发送按钮 */
static lv_obj_t *kb;                 /* 虚拟键盘 */
static volatile int chat_pending = 0;/* 1=等待 AI 回复 */
static volatile int wifi_connected = 0;  /* 1=WiFi 已连接 */

/* ---- 拼音输入法候选栏 ---- */
#define IME_MAX_CAND 6                  /* 候选栏每页最多显示的候选字数 */
static lv_obj_t *ime_bar;               /* 候选栏容器 */
static lv_obj_t *ime_btns[IME_MAX_CAND];  /* 候选字按钮 */
static lv_obj_t *ime_lbls[IME_MAX_CAND];  /* 候选字标签 */
static char ime_cand[IME_MAX_CAND][4];    /* 候选字 UTF-8（3 字节 + \0） */
static int  ime_cand_plen[IME_MAX_CAND];  /* 候选字来源拼音组的拼音长度 */
static int  ime_page;                     /* 当前候选页 */
static int  ime_total;                    /* 总候选字数 */
static char ime_all_cand[512][4];         /* 全部候选字缓存 */
static int  ime_all_plen[512];            /* 全部候选字拼音长度 */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define AI_URL "https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions"
#define AI_KEY "sk-ws-H.EPLHEIP.hWXu.MEYCIQDytiueAUS-gKpkG1X7QQ9DxoDTPCFth471SALe1gZnjgIhAJFrKg4RZ9edeHxWVbypeJmRSSaAmhASte47U3XpsJ0R"
#define AI_MODEL "qwen3.8-max"
#define AI_SYSTEM_PROMPT "Reply in concise Chinese. Do not use emoji, emoticons, markdown symbols, or special symbols."
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
static char http_resp[4096];
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for lvglTask */
osThreadId_t lvglTaskHandle;
const osThreadAttr_t lvglTask_attributes = {
  .name = "lvglTask",
  .stack_size = 4096 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for esp8266Task */
osThreadId_t esp8266TaskHandle;
const osThreadAttr_t esp8266Task_attributes = {
  .name = "esp8266Task",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for esp_rx_queue */
osMessageQueueId_t esp_rx_queueHandle;
const osMessageQueueAttr_t esp_rx_queue_attributes = {
  .name = "esp_rx_queue"
};
/* Definitions for esp_ai_queue */
osMessageQueueId_t esp_ai_queueHandle;
const osMessageQueueAttr_t esp_ai_queue_attributes = {
  .name = "esp_ai_queue"
};
/* Definitions for esp_ai_req_queue */
osMessageQueueId_t esp_ai_req_queueHandle;
const osMessageQueueAttr_t esp_ai_req_queue_attributes = {
  .name = "esp_ai_req_queue"
};
/* Definitions for asrTask */
osThreadId_t asrTaskHandle;
const osThreadAttr_t asrTask_attributes = {
  .name = "asrTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for asr_queue */
osMessageQueueId_t asr_queueHandle;
const osMessageQueueAttr_t asr_queue_attributes = {
  .name = "asr_queue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static void create_main_ui(void);
static void create_robot_face(void);
static void create_system_ui(void);
static void create_clock_ui(void);
static void create_calendar_ui(void);
static void create_environment_ui(void);
static void main_to_clock_handler(lv_event_t *e);
static void main_to_calendar_handler(lv_event_t *e);
static void main_to_environment_handler(lv_event_t *e);
static void create_ai_ui(void);

static lv_obj_t *idle_scr;
static lv_obj_t *main_scr;
static lv_obj_t *system_scr;
static lv_obj_t *clock_scr;
static lv_obj_t *calendar_scr;
static lv_obj_t *env_scr;
static lv_obj_t *env_temp_label;
static lv_obj_t *env_humi_label;
static lv_obj_t *env_press_label;
static lv_obj_t *env_alti_label;
static lv_obj_t *ai_scr;
static lv_obj_t *status_info_label;   /* 系统状态页面的信息标签 */
static lv_obj_t *robot_eye_l, *robot_eye_r, *robot_pupil_l, *robot_pupil_r, *robot_mouth; /* 机器人表情 */
static lv_timer_t *main_idle_timer;  /* 菜单10秒无操作自动返回待机 */
static void esp_log(const char *text);
static void esp_log_update_last(const char *text);
static void esp_log_ai(const char *text);
static void extract_content(const char *resp, char *out, int out_sz);
static void sanitize_ai_text(char *dst, int dst_sz, const char *src);

static void chat_add_message(const char *role, const char *content);
static void chat_send_to_ai(const char *user_msg);
static void chat_build_json(char *json, int json_sz);
static void chat_ui_update(void);
static void chat_send_handler(lv_event_t *e);
static void ai_input_click_handler(lv_event_t *e);
static void ai_scr_click_handler(lv_event_t *e);
static void cal_goto_rtc_month(void);
static void cal_refresh(void);
static void ime_update_candidates(void);
static void ime_commit(const char *cand, int pinyin_len);
static void ime_cand_handler(lv_event_t *e);
static void ime_value_changed_handler(lv_event_t *e);
static void ime_prev_handler(lv_event_t *e);
static void ime_next_handler(lv_event_t *e);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void lvglStartTask(void *argument);
void esp8266StartTask(void *argument);
void asrStartTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of esp_rx_queue */
  esp_rx_queueHandle = osMessageQueueNew (4, sizeof(esp_msg_t), &esp_rx_queue_attributes);

  /* creation of esp_ai_queue */
  esp_ai_queueHandle = osMessageQueueNew (1, 512, &esp_ai_queue_attributes);

  /* creation of esp_ai_req_queue */
  esp_ai_req_queueHandle = osMessageQueueNew (1, 256, &esp_ai_req_queue_attributes);

  /* creation of asr_queue */
  asr_queueHandle = osMessageQueueNew (4, sizeof(asr_msg_t), &asr_queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of lvglTask */
  lvglTaskHandle = osThreadNew(lvglStartTask, NULL, &lvglTask_attributes);

  /* creation of esp8266Task */
  esp8266TaskHandle = osThreadNew(esp8266StartTask, NULL, &esp8266Task_attributes);

  /* creation of asrTask */
  asrTaskHandle = osThreadNew(asrStartTask, NULL, &asrTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);

    osDelay(500);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_lvglStartTask */
/**
* @brief Function implementing the lvglTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_lvglStartTask */
void lvglStartTask(void *argument)
{
  /* USER CODE BEGIN lvglStartTask */
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    create_robot_face();
    create_system_ui();
    create_clock_ui();
    create_calendar_ui();
    create_ai_ui();
    create_main_ui();
    create_environment_ui();

    lv_scr_load(idle_scr);
    lv_timer_handler();  /* 立即渲染首帧，屏幕先亮 */

    /* 屏幕加载后初始化表情动画（此时布局已确定，坐标正确） */
    robot_face_anim_init(robot_eye_l, robot_eye_r, robot_pupil_l, robot_pupil_r, robot_mouth, idle_scr);

    /* 后台初始化传感器（屏幕已显示，不阻塞 UI） */
    I2C_Bus_Init();
    ATH20_Init();
    BMP280_Init();

    esp_msg_t m;
    char status_buf[512] = "";
    int status_line = 0;
    char ai_buf[512];

    /* 表情状态计时器：用于自动恢复到 IDLE */
    uint32_t face_state_timer = 0;
    robot_state_t face_auto_state = ROBOT_IDLE;

    for (;;) {
        /* 读取 ESP8266 状态消息并更新状态页面 */
        while (osMessageQueueGet(esp_rx_queueHandle, &m, NULL, 0) == osOK) {
            if (m.type == ESP_MSG_UPDATE_LAST && status_line > 0) {
                char *last = strrchr(status_buf, '\n');
                size_t prefix_len = last ? (size_t)(last + 1 - status_buf) : 0;
                status_buf[prefix_len] = '\0';
                strncat(status_buf, m.text, sizeof(status_buf) - strlen(status_buf) - 1);
            } else {
                if (status_line > 0) {
                    strncat(status_buf, "\n", sizeof(status_buf) - strlen(status_buf) - 1);
                }
                strncat(status_buf, m.text, sizeof(status_buf) - strlen(status_buf) - 1);
                status_line++;
            }
            if (status_info_label != NULL) {
                lv_label_set_text(status_info_label, status_buf);
            }
        }

        /* 读取 AI 回复并追加到对话 */
        if (osMessageQueueGet(esp_ai_queueHandle, ai_buf, NULL, 0) == osOK) {
            chat_add_message("assistant", ai_buf);
            chat_pending = 0;
            /* AI 回复到达 → 机器人说话表情，2 秒后自动恢复 */
            robot_face_set_state(ROBOT_SPEAKING);
            face_state_timer = lv_tick_get();
            face_auto_state = ROBOT_IDLE;
        }

        /* 读取语音命令并处理（仅离线命令词，无语音转文字） */
        asr_msg_t asr_msg;
        if (osMessageQueueGet(asr_queueHandle, &asr_msg, NULL, 0) == osOK) {
            /* 收到语音命令 → 机器人聆听表情 */
            robot_face_set_state(ROBOT_LISTENING);
            face_state_timer = lv_tick_get();
            face_auto_state = ROBOT_IDLE;

            switch (asr_msg.cmd_id) {
            case ASR_CMD_IDLE:
                lv_scr_load(idle_scr);
                break;
            case ASR_CMD_MAIN_MENU:
                lv_scr_load(main_scr);
                break;
            case ASR_CMD_STATUS:
            case ASR_CMD_SETTINGS:
                lv_scr_load(system_scr);
                break;
            case ASR_CMD_AI_CHAT:
                lv_scr_load(ai_scr);
                break;
            case ASR_CMD_CLOCK:
                lv_scr_load(clock_scr);
                break;
            case ASR_CMD_CALENDAR:
                lv_scr_load(calendar_scr);
                break;
            case ASR_CMD_BACK:
                lv_scr_load(main_scr);
                break;
            default:
                break;
            }
        }

        /* 表情自动恢复：超过 1.8 秒后切回目标状态 */
        if (face_state_timer > 0 && (lv_tick_get() - face_state_timer) > 1800) {
            robot_face_set_state(face_auto_state);
            face_state_timer = 0;
        }

        robot_face_tick();
        lv_timer_handler();
        osDelay(5);
    }
  /* USER CODE END lvglStartTask */
}

/* 时间同步：从 HTTP 响应头 "Date:" 字段解析并设置 RTC，仅执行一次 */
static int rtc_synced = 0;
#define RTC_SYNC_RETRY_MS 30000U

static int sync_days_in_month(int year, int month)
{
    static const int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (month == 2 && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) {
        return 29;
    }
    return days[month - 1];
}

static const char *find_http_date_value(const char *resp)
{
    const char *p = resp;
    while (p && *p) {
        if ((p == resp || p[-1] == '\n') &&
            strncmp(p, "Date:", 5) == 0) {
            return p + 5;
        }
        if ((p == resp || p[-1] == '\n') &&
            strncmp(p, "date:", 5) == 0) {
            return p + 5;
        }
        p = strchr(p, '\n');
        if (p) p++;
    }
    return NULL;
}

static void esp_sync_time(const char *resp)
{
    if (rtc_synced || !resp) return;

    const char *p = find_http_date_value(resp);
    if (!p) return;

    /* 跳过星期几 */
    while (*p == ' ') p++;
    while (*p && *p != ' ') p++;
    while (*p == ' ') p++;

    /* 解析 "20 Aug 2026 12:34:56 GMT" */
    int day = 0, year = 0, hour = 0, min = 0, sec = 0;
    char mon_str[4] = {0};
    if (sscanf(p, "%d %3s %d %d:%d:%d", &day, mon_str, &year, &hour, &min, &sec) < 6) return;

    static const char *months[] = {"Jan","Feb","Mar","Apr","May","Jun",
                                    "Jul","Aug","Sep","Oct","Nov","Dec"};
    int mon = 0;
    for (int i = 0; i < 12; i++) {
        if (strncmp(mon_str, months[i], 3) == 0) { mon = i + 1; break; }
    }
    if (mon == 0) return;

    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    /* GMT 转北京时间 (UTC+8)，处理跨日 */
    int total_h = hour + 8;
    int day_carry = total_h / 24;
    total_h = total_h % 24;
    day += day_carry;
    if (day > sync_days_in_month(year, mon)) {
        day = 1;
        mon++;
        if (mon > 12) {
            mon = 1;
            year++;
        }
    }

    sTime.Hours = (uint8_t)total_h;
    sTime.Minutes = min;
    sTime.Seconds = sec;
    sTime.TimeFormat = RTC_HOURFORMAT_24;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    sDate.WeekDay = 1;
    sDate.Month = mon;
    sDate.Date = day;
    sDate.Year = year - 2000;
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) == HAL_OK &&
        HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) == HAL_OK) {
        rtc_synced = 1;
    }
}

static int esp_try_sync_time_once(void)
{
    esp_status_t status = ESP_HTTPPost("http://httpbin.org/post", "{}", http_resp, (uint16_t)sizeof(http_resp));
    if (status == ESP_OK) {
        esp_sync_time(http_resp);
    }
    return rtc_synced;
}

/* USER CODE BEGIN Header_esp8266StartTask */
/**
* @brief Function implementing the esp8266Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_esp8266StartTask */
void esp8266StartTask(void *argument)
{
  /* USER CODE BEGIN esp8266StartTask */
    esp_status_t status;
    char req[256];
    static char json[8192];
    uint32_t next_sync_retry_tick = 0;

    /* 初始化 + 连接 WiFi，失败则重试 */
    while (!wifi_connected) {
        esp_log("\xE6\xAD\xA3\xE5\x9C\xA8\xE5\x88\x9D\xE5\xA7\x8B\xE5\x8C\x96" "ESP8266..."); /* 正在初始化ESP8266... */
        ESP_Init();
        osDelay(2000);

        status = ESP_SendCmd("AT\r\n", "OK", 2000);
        if (status != ESP_OK) {
            esp_log("AT\xE9\x80\x9A\xE8\xAE\xAF\xE5\xA4\xB1\xE8\xB4\xA5\xEF\xBC\x8C" "5\xE7\xA7\x92\xE5\x90\x8E\xE9\x87\x8D\xE8\xAF\x95..."); /* AT通讯失败，5秒后重试... */
            osDelay(5000);
            continue;
        }
        esp_log("AT\xE9\x80\x9A\xE8\xAE\xAF\xE6\xAD\xA3\xE5\xB8\xB8"); /* AT通讯正常 */

        esp_log("\xE6\xAD\xA3\xE5\x9C\xA8\xE8\xBF\x9E\xE6\x8E\xA5WiFi..."); /* 正在连接WiFi... */
        status = ESP_ConnectAP("HUAWEI-1FKZ9T", "FSNH82502");
        if (status != ESP_OK) {
            esp_log("WiFi\xE8\xBF\x9E\xE6\x8E\xA5\xE5\xA4\xB1\xE8\xB4\xA5\xEF\xBC\x8C" "5\xE7\xA7\x92\xE5\x90\x8E\xE9\x87\x8D\xE8\xAF\x95..."); /* WiFi连接失败，5秒后重试... */
            osDelay(5000);
            continue;
        }

        wifi_connected = 1;
        esp_log("WiFi\xE8\xBF\x9E\xE6\x8E\xA5\xE6\x88\x90\xE5\x8A\x9F"); /* WiFi连接成功 */
    }

    esp_log("\xE7\xB3\xBB\xE7\xBB\x9F\xE5\xB0\xB1\xE7\xBB\xAA"); /* 系统就绪 */

    /* 上电自动对时 */
    if (!rtc_synced) {
        esp_log("\xE6\xAD\xA3\xE5\x9C\xA8\xE5\x90\x8C\xE6\xAD\xA5\xE6\x97\xB6\xE9\x92\x9F..."); /* 正在同步时钟... */
        esp_log("\xE6\x97\xB6\xE9\x92\x9F\xE5\x90\x8C\xE6\xAD\xA5\xE4\xB8\xAD..."); /* 时钟同步中... */
        if (esp_try_sync_time_once()) {
            esp_log_update_last("\xE6\x97\xB6\xE9\x92\x9F\xE5\x90\x8C\xE6\xAD\xA5\xE6\x88\x90\xE5\x8A\x9F"); /* 时钟同步成功 */
        } else {
            esp_log_update_last("\xE6\x97\xB6\xE9\x92\x9F\xE5\x90\x8C\xE6\xAD\xA5\xE5\xA4\xB1\xE8\xB4\xA5\xEF\xBC\x8C\xE7\xA8\x8D\xE5\x90\x8E\xE9\x87\x8D\xE8\xAF\x95"); /* 时钟同步失败，稍后重试 */
            next_sync_retry_tick = HAL_GetTick() + RTC_SYNC_RETRY_MS;
        }
    }

    for (;;) {
        /* 断网则自动重连 */
        if (!wifi_connected) {
            ESP_Init();
            osDelay(2000);
            status = ESP_ConnectAP("HUAWEI-1FKZ9T", "FSNH82502");
            if (status == ESP_OK) {
                wifi_connected = 1;
            } else {
                osDelay(10000);
            }
            continue;
        }

        /* 等待 AI 对话请求 */
        if (!rtc_synced && (int32_t)(HAL_GetTick() - next_sync_retry_tick) >= 0) {
            esp_log_update_last("\xE6\xAD\xA3\xE5\x9C\xA8\xE9\x87\x8D\xE8\xAF\x95\xE5\x90\x8C\xE6\xAD\xA5\xE6\x97\xB6\xE9\x92\x9F..."); /* 正在重试同步时钟... */
            if (esp_try_sync_time_once()) {
                esp_log_update_last("\xE6\x97\xB6\xE9\x92\x9F\xE5\x90\x8C\xE6\xAD\xA5\xE6\x88\x90\xE5\x8A\x9F"); /* 时钟同步成功 */
            } else {
                esp_log_update_last("\xE6\x97\xB6\xE9\x92\x9F\xE5\x90\x8C\xE6\xAD\xA5\xE5\xA4\xB1\xE8\xB4\xA5\xEF\xBC\x8C\xE7\xA8\x8D\xE5\x90\x8E\xE9\x87\x8D\xE8\xAF\x95"); /* 时钟同步失败，稍后重试 */
                next_sync_retry_tick = HAL_GetTick() + RTC_SYNC_RETRY_MS;
            }
        }

        if (osMessageQueueGet(esp_ai_req_queueHandle, req, NULL, 1000) == osOK) {
            chat_build_json(json, (int)sizeof(json));

            status = ESP_HTTPPostAuth(AI_URL, json, AI_KEY, http_resp, (uint16_t)sizeof(http_resp));

            if (status == ESP_OK) {
                esp_sync_time(http_resp);
                extract_content(http_resp, json, (int)sizeof(json));
                if (json[0] != '\0') {
                    esp_log_ai(json);
                } else {
                    const char *body = strstr(http_resp, "\r\n\r\n");
                    char dump[CHAT_MSG_LEN];
                    snprintf(dump, sizeof(dump), "[\xE6\x97\xA0\xE5\x86\x85\xE5\xAE\xB9] %.200s",
                             body ? body + 4 : http_resp);
                    esp_log_ai(dump);
                }
            } else {
                char b[32];
                snprintf(b, sizeof(b), "[\xE9\x94\x99\xE8\xAF\xAF %d]", (int)status); /* [错误 N] */
                esp_log_ai(b);
                wifi_connected = 0;
            }
        }
    }
  /* USER CODE END esp8266StartTask */
}

/* USER CODE BEGIN Header_asrStartTask */
/**
* @brief Function implementing the asrTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_asrStartTask */
void asrStartTask(void *argument)
{
  /* USER CODE BEGIN asrStartTask */
    asr_msg_t msg;

    ASR_Init();

    for (;;) {
        if (ASR_GetMessage(&msg)) {
            osMessageQueuePut(asr_queueHandle, &msg, 0, 100);
        }
        osDelay(20);
    }
  /* USER CODE END asrStartTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* ---- ESP helper: push text into the status list ---- */
static void esp_log(const char *text)
{
    esp_msg_t m;
    m.type = ESP_MSG_RESP;
    snprintf(m.text, sizeof(m.text), "%s", text);
    osMessageQueuePut(esp_rx_queueHandle, &m, 0, 100);
}

static void esp_log_update_last(const char *text)
{
    esp_msg_t m;
    m.type = ESP_MSG_UPDATE_LAST;
    snprintf(m.text, sizeof(m.text), "%s", text);
    osMessageQueuePut(esp_rx_queueHandle, &m, 0, 100);
}

static void esp_log_ai(const char *text)
{
    char buf[512];
    sanitize_ai_text(buf, (int)sizeof(buf), text);
    osMessageQueuePut(esp_ai_queueHandle, buf, 0, 100);
}

static int hex_val(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static int json_read_u4(const char *s, uint32_t *cp)
{
    uint32_t v = 0;

    for (int i = 0; i < 4; i++) {
        int h = hex_val(s[i]);
        if (h < 0) return 0;
        v = (v << 4) | (uint32_t)h;
    }

    *cp = v;
    return 1;
}

static int utf8_put(char *out, int out_sz, int *j, uint32_t cp)
{
    if (cp <= 0x7F) {
        if (*j >= out_sz - 1) return 0;
        out[(*j)++] = (char)cp;
    } else if (cp <= 0x7FF) {
        if (*j >= out_sz - 2) return 0;
        out[(*j)++] = (char)(0xC0 | (cp >> 6));
        out[(*j)++] = (char)(0x80 | (cp & 0x3F));
    } else {
        if (*j >= out_sz - 3) return 0;
        out[(*j)++] = (char)(0xE0 | (cp >> 12));
        out[(*j)++] = (char)(0x80 | ((cp >> 6) & 0x3F));
        out[(*j)++] = (char)(0x80 | (cp & 0x3F));
    }
    return 1;
}

static int utf8_read(const char **ps, uint32_t *cp)
{
    const unsigned char *s = (const unsigned char *)*ps;

    if (s[0] < 0x80) {
        *cp = s[0];
        *ps += 1;
        return 1;
    }
    if ((s[0] & 0xE0) == 0xC0 && (s[1] & 0xC0) == 0x80) {
        *cp = ((uint32_t)(s[0] & 0x1F) << 6) | (uint32_t)(s[1] & 0x3F);
        *ps += 2;
        return 1;
    }
    if ((s[0] & 0xF0) == 0xE0 && (s[1] & 0xC0) == 0x80 && (s[2] & 0xC0) == 0x80) {
        *cp = ((uint32_t)(s[0] & 0x0F) << 12) |
              ((uint32_t)(s[1] & 0x3F) << 6) |
              (uint32_t)(s[2] & 0x3F);
        *ps += 3;
        return 1;
    }
    if ((s[0] & 0xF8) == 0xF0 && (s[1] & 0xC0) == 0x80 &&
        (s[2] & 0xC0) == 0x80 && (s[3] & 0xC0) == 0x80) {
        *cp = ((uint32_t)(s[0] & 0x07) << 18) |
              ((uint32_t)(s[1] & 0x3F) << 12) |
              ((uint32_t)(s[2] & 0x3F) << 6) |
              (uint32_t)(s[3] & 0x3F);
        *ps += 4;
        return 1;
    }

    *cp = ' ';
    *ps += 1;
    return 0;
}

static int ai_codepoint_supported(uint32_t cp)
{
    if (cp == '\n' || cp == '\r' || cp == '\t') return 1;
    if (cp >= 0x20 && cp <= 0x7E) return 1;
    if (cp >= 0x3000 && cp <= 0x303F) return 1;   /* CJK 标点 */
    if (cp >= 0x4E00 && cp <= 0x9FFF) return 1;   /* 常用中文 */
    if (cp >= 0xFF00 && cp <= 0xFFEF) return 1;   /* 全角字符 */
    return 0;
}

static void sanitize_ai_text(char *dst, int dst_sz, const char *src)
{
    int j = 0;
    int last_space = 0;

    while (*src && j < dst_sz - 1) {
        uint32_t cp;
        const char *old = src;
        utf8_read(&src, &cp);

        if (ai_codepoint_supported(cp)) {
            if (cp == '\r') cp = '\n';
            if (!utf8_put(dst, dst_sz, &j, cp)) break;
            last_space = (cp == ' ' || cp == '\n' || cp == '\t');
        } else if (!last_space && j < dst_sz - 1) {
            (void)old;
            dst[j++] = ' ';
            last_space = 1;
        }
    }

    dst[j] = '\0';
}

static void extract_content(const char *resp, char *out, int out_sz)
{
    char *p = strstr(resp, "\"content\"");
    uint32_t cp;
    uint32_t cp2;
    int j = 0;
    if (!p) { out[0] = '\0'; return; }
    p = strchr(p, ':');
    if (!p) { out[0] = '\0'; return; }
    p = strchr(p, '"');
    if (!p) { out[0] = '\0'; return; }
    p++;
    while (*p && *p != '"' && j < out_sz - 1) {
        if (*p == '\\' && *(p + 1)) {
            p++;
            if (*p == 'n') out[j++] = '\n';
            else if (*p == 't') out[j++] = '\t';
            else if (*p == 'r') out[j++] = '\r';
            else if (*p == '\\') out[j++] = '\\';
            else if (*p == '"') out[j++] = '"';
            else if (*p == 'u' && json_read_u4(p + 1, &cp)) {
                p += 4;
                if (cp >= 0xD800 && cp <= 0xDBFF &&
                    p[1] == '\\' && p[2] == 'u' && json_read_u4(p + 3, &cp2)) {
                    p += 6;
                    if (cp2 >= 0xDC00 && cp2 <= 0xDFFF) {
                        cp = 0x10000 + ((cp - 0xD800) << 10) + (cp2 - 0xDC00);
                    }
                }
                if (ai_codepoint_supported(cp)) {
                    if (!utf8_put(out, out_sz, &j, cp)) break;
                } else if (j < out_sz - 1) {
                    out[j++] = ' ';
                }
            } else out[j++] = *p;
        } else {
            out[j++] = *p;
        }
        p++;
    }
    out[j] = '\0';
}

static void idle_click_handler(lv_event_t *e)
{
    lv_scr_load(main_scr);
}

static void back_click_handler(lv_event_t *e)
{
    lv_scr_load(main_scr);
}

static void main_to_system_handler(lv_event_t *e)
{
    lv_scr_load(system_scr);
}

static void main_to_clock_handler(lv_event_t *e)
{
    lv_scr_load(clock_scr);
}

static void main_to_ai_handler(lv_event_t *e)
{
    lv_scr_load(ai_scr);
}

static void main_to_calendar_handler(lv_event_t *e)
{
    (void)e;
    cal_goto_rtc_month();
    cal_refresh();
    lv_scr_load(calendar_scr);
}

static void main_to_environment_handler(lv_event_t *e)
{
    lv_scr_load(env_scr);
}

static void main_idle_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    if (lv_scr_act() == main_scr) {
        lv_scr_load(idle_scr);
    }
}

static void main_scr_pressed_handler(lv_event_t *e)
{
    (void)e;
    lv_timer_reset(main_idle_timer);
}

static void main_scr_loaded_handler(lv_event_t *e)
{
    (void)e;
    lv_timer_reset(main_idle_timer);
}

static void chat_send_handler(lv_event_t *e)
{
    if (chat_pending) return;

    const char *msg = lv_textarea_get_text(chat_input);
    if (msg[0] == '\0') return;

    /* 触摸发送 → 思考表情 */
    robot_face_set_state(ROBOT_THINKING);
    chat_send_to_ai(msg);
    lv_textarea_set_text(chat_input, "");
}

static void ai_input_click_handler(lv_event_t *e)
{
    if (lv_obj_has_flag(kb, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_keyboard_set_textarea(kb, chat_input);
        lv_obj_align_to(ime_bar, kb, LV_ALIGN_OUT_TOP_MID, 0, 0);
        ime_update_candidates();
        lv_obj_clear_flag(ime_bar, LV_OBJ_FLAG_HIDDEN);
    }
}

static void ai_scr_click_handler(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    if (target == ai_scr) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ime_bar, LV_OBJ_FLAG_HIDDEN);
    }
}

/* ===== 拼音输入法候选栏 ===== */

/* 返回 textarea 末尾连续拼音串（a-z 或 v）的起始字节偏移；无则返回字符串长度 */
static int ime_run_offset(const char *text)
{
    int len = (int)strlen(text);
    int start = len;
    while (start > 0) {
        char c = text[start - 1];
        if ((c >= 'a' && c <= 'z') || c == 'v') start--;
        else break;
    }
    return start;
}

/* 找到 run 中最长、且有候选字的拼音前缀长度（即当前应显示的"音节"） */
static int ime_active_pylen(const char *run, int runlen)
{
    for (int L = runlen; L >= 1; L--) {
        for (int g = 0; g < (int)PY_GROUP_COUNT; g++) {
            if (strncmp(py_groups[g].py, run, L) == 0) return L;
        }
    }
    return 0;
}

/* 读取输入框，刷新候选栏 */
static void ime_update_candidates(void)
{
    const char *text = lv_textarea_get_text(chat_input);
    int len = (int)strlen(text);
    int start = ime_run_offset(text);
    int runlen = len - start;

    if (runlen <= 0) {
        lv_obj_add_flag(ime_bar, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    char run[32];
    if (runlen > (int)sizeof(run) - 1) runlen = (int)sizeof(run) - 1;
    memcpy(run, text + start, runlen);
    run[runlen] = '\0';

    int L = ime_active_pylen(run, runlen);
    if (L <= 0) {
        lv_obj_add_flag(ime_bar, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    /* 收集所有匹配的候选字（先不限制数量） */
    int total = 0;
    for (int g = 0; g < (int)PY_GROUP_COUNT && total < 512; g++) {
        if (strncmp(py_groups[g].py, run, L) == 0) {
            int plen = (int)strlen(py_groups[g].py);
            const char *chs = py_groups[g].chs;
            while (*chs && total < 512) {
                memcpy(ime_all_cand[total], chs, 3);
                ime_all_cand[total][3] = '\0';
                ime_all_plen[total] = plen;
                total++;
                chs += 3;
            }
        }
    }

    ime_total = total;
    ime_page = 0;

    /* 填充当前页 */
    int off = ime_page * IME_MAX_CAND;
    for (int i = 0; i < IME_MAX_CAND; i++) {
        int idx = off + i;
        if (idx < total) {
            memcpy(ime_cand[i], ime_all_cand[idx], 4);
            ime_cand_plen[i] = ime_all_plen[idx];
            lv_label_set_text(ime_lbls[i], ime_cand[i]);
            lv_obj_clear_flag(ime_btns[i], LV_OBJ_FLAG_HIDDEN);
        } else {
            ime_cand[i][0] = '\0';
            lv_obj_add_flag(ime_btns[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
    lv_obj_clear_flag(ime_bar, LV_OBJ_FLAG_HIDDEN);
}

/* 翻页：刷新当前页候选字 */
static void ime_refresh_page(void)
{
    int off = ime_page * IME_MAX_CAND;
    for (int i = 0; i < IME_MAX_CAND; i++) {
        int idx = off + i;
        if (idx < ime_total) {
            memcpy(ime_cand[i], ime_all_cand[idx], 4);
            ime_cand_plen[i] = ime_all_plen[idx];
            lv_label_set_text(ime_lbls[i], ime_cand[i]);
            lv_obj_clear_flag(ime_btns[i], LV_OBJ_FLAG_HIDDEN);
        } else {
            ime_cand[i][0] = '\0';
            lv_obj_add_flag(ime_btns[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
}

/* 上一页 */
static void ime_prev_handler(lv_event_t *e)
{
    (void)e;
    if (ime_page > 0) {
        ime_page--;
        ime_refresh_page();
    }
}

/* 下一页 */
static void ime_next_handler(lv_event_t *e)
{
    (void)e;
    if ((ime_page + 1) * IME_MAX_CAND < ime_total) {
        ime_page++;
        ime_refresh_page();
    }
}

/* 把候选字提交进输入框：替换末尾拼音串的前 pinyin_len 个字母，保留剩余字母 */
static void ime_commit(const char *cand, int pinyin_len)
{
    const char *text = lv_textarea_get_text(chat_input);
    int len = (int)strlen(text);
    int start = ime_run_offset(text);
    int runlen = len - start;
    int k = pinyin_len;
    if (k > runlen) k = runlen;   /* 提前点选时吞掉整个已输入 run */

    char buf[512];
    int w = snprintf(buf, sizeof(buf), "%.*s%s%.*s",
                     start, text,
                     cand,
                     len - (start + k), text + start + k);
    if (w > 0 && w < (int)sizeof(buf)) {
        lv_textarea_set_text(chat_input, buf);
        /* set_text 会触发 VALUE_CHANGED -> ime_update_candidates，无需手动刷新 */
    }
}

/* 候选字按钮点击 */
static void ime_cand_handler(lv_event_t *e)
{
    lv_obj_t *t = lv_event_get_target(e);
    for (int i = 0; i < IME_MAX_CAND; i++) {
        if (ime_btns[i] == t) {
            if (ime_cand[i][0] != '\0') {
                ime_commit(ime_cand[i], ime_cand_plen[i]);
            }
            break;
        }
    }
}

/* 输入框内容变化时刷新候选栏 */
static void ime_value_changed_handler(lv_event_t *e)
{
    (void)e;
    ime_update_candidates();
}



/**
 * @brief  创建主菜单页面UI（手机桌面风格：图标+文字）
 * @note   深色背景上横向排列3个图标卡片
 *         - 左: ⚙ 系统 -> 系统状态页面
 *         - 中: 🔔 时钟 -> 时钟显示页面
 *         - 右: 💬 AI对话 -> AI对话页面
 *         - 10秒无操作自动返回待机画面
 */
static void create_main_ui(void)
{
    main_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(main_scr, lv_color_hex(0x0a0a0a), 0);

    /* 顶部标题 */
    lv_obj_t *title = lv_label_create(main_scr);
    lv_label_set_text(title, "\xE8\x8F\x9C\xE5\x8D\x95"); /* 菜单 */
    lv_obj_set_style_text_font(title, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x00ff41), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    /* ---- 系统（齿轮） ---- */
    lv_obj_t *btn1 = lv_btn_create(main_scr);
    lv_obj_set_size(btn1, 45, 45);
    lv_obj_align(btn1, LV_ALIGN_CENTER, -90, -80);
    lv_obj_set_style_bg_color(btn1, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(btn1, 0, 0);
    lv_obj_set_style_radius(btn1, 10, 0);
    lv_obj_t *icon1 = lv_label_create(btn1);
    lv_label_set_text(icon1, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_text_font(icon1, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(icon1, lv_color_hex(0x00ff41), 0);
    lv_obj_center(icon1);
    lv_obj_t *txt1 = lv_label_create(main_scr);
    lv_label_set_text(txt1, "\xE7\xB3\xBB\xE7\xBB\x9F"); /* 系统 */
    lv_obj_set_style_text_font(txt1, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(txt1, lv_color_hex(0x00ff41), 0);
    lv_obj_align_to(txt1, btn1, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_add_event_cb(btn1, main_to_system_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn1, main_scr_pressed_handler, LV_EVENT_CLICKED, NULL);

    /* ---- 时钟 ---- */
    lv_obj_t *btn2 = lv_btn_create(main_scr);
    lv_obj_set_size(btn2, 45, 45);
    lv_obj_align(btn2, LV_ALIGN_CENTER, -30, -80);
    lv_obj_set_style_bg_color(btn2, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(btn2, 0, 0);
    lv_obj_set_style_radius(btn2, 10, 0);
    lv_obj_t *icon2 = lv_label_create(btn2);
    lv_label_set_text(icon2, LV_SYMBOL_BELL);
    lv_obj_set_style_text_font(icon2, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(icon2, lv_color_hex(0x00ff41), 0);
    lv_obj_center(icon2);
    lv_obj_t *txt2 = lv_label_create(main_scr);
    lv_label_set_text(txt2, "\xE6\x97\xB6\xE9\x92\x9F"); /* 时钟 */
    lv_obj_set_style_text_font(txt2, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(txt2, lv_color_hex(0x00ff41), 0);
    lv_obj_align_to(txt2, btn2, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_add_event_cb(btn2, main_to_clock_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn2, main_scr_pressed_handler, LV_EVENT_CLICKED, NULL);

    /* ---- AI对话 ---- */
    lv_obj_t *btn3 = lv_btn_create(main_scr);
    lv_obj_set_size(btn3, 45, 45);
    lv_obj_align(btn3, LV_ALIGN_CENTER, 30, -80);
    lv_obj_set_style_bg_color(btn3, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(btn3, 0, 0);
    lv_obj_set_style_radius(btn3, 10, 0);
    lv_obj_t *icon3 = lv_label_create(btn3);
    lv_label_set_text(icon3, LV_SYMBOL_CHARGE);
    lv_obj_set_style_text_font(icon3, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(icon3, lv_color_hex(0x00ff41), 0);
    lv_obj_center(icon3);
    lv_obj_t *txt3 = lv_label_create(main_scr);
    lv_label_set_text(txt3, "AI\xE5\xAF\xB9\xE8\xAF\x9D"); /* AI对话 */
    lv_obj_set_style_text_font(txt3, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(txt3, lv_color_hex(0x00ff41), 0);
    lv_obj_align_to(txt3, btn3, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_add_event_cb(btn3, main_to_ai_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn3, main_scr_pressed_handler, LV_EVENT_CLICKED, NULL);

    /* ---- 日历 ---- */
    lv_obj_t *btn4 = lv_btn_create(main_scr);
    lv_obj_set_size(btn4, 45, 45);
    lv_obj_align(btn4, LV_ALIGN_CENTER, 90, -80);
    lv_obj_set_style_bg_color(btn4, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(btn4, 0, 0);
    lv_obj_set_style_radius(btn4, 10, 0);
    lv_obj_t *icon4 = lv_label_create(btn4);
    lv_label_set_text(icon4, LV_SYMBOL_LIST);
    lv_obj_set_style_text_font(icon4, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(icon4, lv_color_hex(0x00ff41), 0);
    lv_obj_center(icon4);
    lv_obj_t *txt4 = lv_label_create(main_scr);
    lv_label_set_text(txt4, "\xE6\x97\xA5\xE5\x8E\x86"); /* 日历 */
    lv_obj_set_style_text_font(txt4, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(txt4, lv_color_hex(0x00ff41), 0);
    lv_obj_align_to(txt4, btn4, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_add_event_cb(btn4, main_to_calendar_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn4, main_scr_pressed_handler, LV_EVENT_CLICKED, NULL);

    /* ---- 环境 ---- */
    lv_obj_t *btn5 = lv_btn_create(main_scr);
    lv_obj_set_size(btn5, 45, 45);
    lv_obj_align(btn5, LV_ALIGN_CENTER, -90, 0);
    lv_obj_set_style_bg_color(btn5, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(btn5, 0, 0);
    lv_obj_set_style_radius(btn5, 10, 0);
    lv_obj_t *icon5 = lv_label_create(btn5);
    lv_label_set_text(icon5, LV_SYMBOL_HOME);
    lv_obj_set_style_text_font(icon5, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(icon5, lv_color_hex(0x00ff41), 0);
    lv_obj_center(icon5);
    lv_obj_t *txt5 = lv_label_create(main_scr);
    lv_label_set_text(txt5, "\xE7\x8E\xAF\xE5\xA2\x83"); /* 环境 */
    lv_obj_set_style_text_font(txt5, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(txt5, lv_color_hex(0x00ff41), 0);
    lv_obj_align_to(txt5, btn5, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_add_event_cb(btn5, main_to_environment_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn5, main_scr_pressed_handler, LV_EVENT_CLICKED, NULL);

    /* 10 秒无操作自动返回待机画面 */
    main_idle_timer = lv_timer_create(main_idle_timer_cb, 10000, NULL);
    lv_obj_add_event_cb(main_scr, main_scr_pressed_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(main_scr, main_scr_loaded_handler, LV_EVENT_SCREEN_LOADED, NULL);
}

/**
 * @brief  创建系统页面UI（合并原状态和设置）
 * @note   包含标题栏、ESP8266状态日志、硬件信息
 *         - 顶部: "系统" 标题 + 返回按钮
 *         - 中部: 状态日志（实时更新）
 *         - 底部: 硬件信息（MCU型号、主频、LCD、框架）
 */
static void create_system_ui(void)
{
    system_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(system_scr, lv_color_hex(0x0a0a0a), 0);

    /* 返回按钮 */
    lv_obj_t *back_btn = lv_btn_create(system_scr);
    lv_obj_set_size(back_btn, 60, 30);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_obj_set_style_bg_color(back_btn, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(back_btn, 0, 0);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "\xE8\xBF\x94\xE5\x9B\x9E"); /* 返回 */
    lv_obj_set_style_text_font(back_label, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(back_label, lv_color_hex(0x00ff41), 0);
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_click_handler, LV_EVENT_CLICKED, NULL);

    /* 标题 */
    lv_obj_t *title = lv_label_create(system_scr);
    lv_label_set_text(title, "\xE7\xB3\xBB\xE7\xBB\x9F"); /* 系统 */
    lv_obj_set_style_text_font(title, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x00ff41), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    /* 状态信息标签: 显示 ESP8266 初始化等实时状态 */
    status_info_label = lv_label_create(system_scr);
    lv_label_set_text(status_info_label, "\xE7\xAD\x89\xE5\xBE\x85\xE5\x88\x9D\xE5\xA7\x8B\xE5\x8C\x96..."); /* 等待初始化... */
    lv_obj_set_style_text_font(status_info_label, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(status_info_label, lv_color_hex(0x00ff41), 0);
    lv_obj_align(status_info_label, LV_ALIGN_CENTER, 0, -20);

    /* 硬件信息标签 */
    lv_obj_t *info = lv_label_create(system_scr);
    lv_label_set_text(info, "MCU: STM32F407VET6\n"
                            "Freq: 168MHz\n"
                            "LCD: 320x240\n"
                            "FreeRTOS + LVGL");
    lv_obj_set_style_text_font(info, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(info, lv_color_hex(0x00ff41), 0);
    lv_obj_align(info, LV_ALIGN_BOTTOM_MID, 0, -20);
}

/**
 * @brief  仿真图形时钟（模拟表盘 + 指针）
 * @note   使用 LVGL 内置 lv_meter 控件，原生支持表盘刻度+指针旋转
 */
#define CLOCK_SIZE  150

static lv_obj_t *clock_meter;
static lv_meter_indicator_t *hour_needle, *min_needle, *sec_needle;

static void analog_clock_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    static uint8_t last_sec = 0xFF;

    if (lv_scr_act() != clock_scr) {
        last_sec = 0xFF;
        return;
    }

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    if (sTime.Seconds == last_sec) {
        return;
    }
    last_sec = sTime.Seconds;

    int32_t hv = (sTime.Hours % 12) * 300 + sTime.Minutes * 5;
    int32_t mv = sTime.Minutes * 60 + sTime.Seconds * 1;
    int32_t sv = sTime.Seconds * 60;

    lv_meter_set_indicator_value(clock_meter, hour_needle, hv);
    lv_meter_set_indicator_value(clock_meter, min_needle, mv);
    lv_meter_set_indicator_value(clock_meter, sec_needle, sv);
}

static void create_clock_ui(void)
{
    clock_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(clock_scr, lv_color_hex(0x0a0a0a), 0);

    /* 返回按钮 */
    lv_obj_t *back_btn = lv_btn_create(clock_scr);
    lv_obj_set_size(back_btn, 60, 30);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_obj_set_style_bg_color(back_btn, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(back_btn, 0, 0);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "\xE8\xBF\x94\xE5\x9B\x9E"); /* 返回 */
    lv_obj_set_style_text_font(back_label, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(back_label, lv_color_hex(0x00ff41), 0);
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_click_handler, LV_EVENT_CLICKED, NULL);

    /* 标题 */
    lv_obj_t *title = lv_label_create(clock_scr);
    lv_label_set_text(title, "\xE6\x97\xB6\xE9\x92\x9F"); /* 时钟 */
    lv_obj_set_style_text_font(title, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x00ff41), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    /* 用 lv_meter 创建模拟时钟 */
    clock_meter = lv_meter_create(clock_scr);
    lv_obj_set_size(clock_meter, CLOCK_SIZE, CLOCK_SIZE);
    lv_obj_align(clock_meter, LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_style_bg_color(clock_meter, lv_color_hex(0x0a0a0a), 0);
    lv_obj_set_style_border_color(clock_meter, lv_color_hex(0x00ff41), 0);
    lv_obj_set_style_border_width(clock_meter, 3, 0);
    lv_obj_set_style_radius(clock_meter, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_pad_all(clock_meter, 0, 0);

    /* 分钟刻度：61 个点让 0/60 首尾重合，视觉上得到完整 60 小格 */
    lv_meter_scale_t *scale_min = lv_meter_add_scale(clock_meter);
    lv_meter_set_scale_ticks(clock_meter, scale_min, 61, 1, 8, lv_color_hex(0x00aa2b));
    lv_meter_set_scale_range(clock_meter, scale_min, 0, 60, 360, 270);

    /* 小时刻度与数字：1~12 分布在近整圈，避免首尾重合造成少一大格 */
    lv_meter_scale_t *scale_hour = lv_meter_add_scale(clock_meter);
    lv_meter_set_scale_ticks(clock_meter, scale_hour, 12, 0, 0, lv_color_hex(0x00ff41));
    lv_meter_set_scale_major_ticks(clock_meter, scale_hour, 1, 3, 15, lv_color_hex(0x00ff41), 8);
    lv_meter_set_scale_range(clock_meter, scale_hour, 1, 12, 330, 300);

    /* 指针层：隐藏刻度，只负责把 0~3600 映射为完整一圈 */
    lv_meter_scale_t *scale_hand = lv_meter_add_scale(clock_meter);
    lv_meter_set_scale_ticks(clock_meter, scale_hand, 0, 0, 0, lv_color_hex(0x00ff41));
    lv_meter_set_scale_range(clock_meter, scale_hand, 0, 3600, 360, 270);

    /* 时针：绿色 4px，较短 */
    hour_needle = lv_meter_add_needle_line(clock_meter, scale_hand, 4, lv_color_hex(0x00ff41), -35);

    /* 分针：绿色 2px，中等 */
    min_needle = lv_meter_add_needle_line(clock_meter, scale_hand, 2, lv_color_hex(0x00ff41), -15);

    /* 秒针：红色 2px，最长 */
    sec_needle = lv_meter_add_needle_line(clock_meter, scale_hand, 2, lv_color_hex(0xff4141), -5);

    /* 100ms 轮询 RTC，秒数变化才重绘，避免任务调度抖动造成肉眼卡一秒 */
    lv_timer_create(analog_clock_timer_cb, 100, NULL);
    analog_clock_timer_cb(NULL);
}

/**
 * @brief  日历页面
 * @note   月视图，可前后翻月，当天高亮显示
 */
#define CAL_YEAR_BASE 2000
#define CAL_YEAR_MAX  2035

static int cal_year, cal_month;
static lv_obj_t *cal_year_dd;
static lv_obj_t *cal_month_dd;
static lv_obj_t *cal_cells[42];

static const char cal_year_options[] =
    "2000\n2001\n2002\n2003\n2004\n2005\n2006\n2007\n2008\n2009\n"
    "2010\n2011\n2012\n2013\n2014\n2015\n2016\n2017\n2018\n2019\n"
    "2020\n2021\n2022\n2023\n2024\n2025\n2026\n2027\n2028\n2029\n"
    "2030\n2031\n2032\n2033\n2034\n2035";

static const char cal_month_options[] =
    "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12";

static void cal_goto_rtc_month(void)
{
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    cal_year = 2000 + sDate.Year;
    cal_month = sDate.Month;
    if (cal_year < CAL_YEAR_BASE || cal_year > CAL_YEAR_MAX) cal_year = CAL_YEAR_BASE;
    if (cal_month < 1 || cal_month > 12) cal_month = 1;
}

static int cal_get_weekday(int y, int m, int d)
{
    if (m < 3) { m += 12; y--; }
    int k = y % 100;
    int j = y / 100;
    return (d + (13 * (m + 1)) / 5 + k + k / 4 + j / 4 - 2 * j) % 7;
}

static int cal_days_in_month(int y, int m)
{
    static const int d[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (m == 2 && (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0))) return 29;
    return d[m - 1];
}

static void cal_refresh(void)
{
    char buf[16];

    if (cal_year_dd != NULL) {
        uint16_t year_sel = (uint16_t)(cal_year - CAL_YEAR_BASE);
        if (lv_dropdown_get_selected(cal_year_dd) != year_sel) {
            lv_dropdown_set_selected(cal_year_dd, year_sel);
        }
    }
    if (cal_month_dd != NULL) {
        uint16_t month_sel = (uint16_t)(cal_month - 1);
        if (lv_dropdown_get_selected(cal_month_dd) != month_sel) {
            lv_dropdown_set_selected(cal_month_dd, month_sel);
        }
    }

    int days = cal_days_in_month(cal_year, cal_month);
    int wd = cal_get_weekday(cal_year, cal_month, 1);

    RTC_DateTypeDef sDate;
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    int today_y = 2000 + sDate.Year;
    int today_m = sDate.Month;
    int today_d = sDate.Date;

    for (int i = 0; i < 42; i++) {
        int d = i - wd + 1;
        if (d >= 1 && d <= days) {
            snprintf(buf, sizeof(buf), "%d", d);
            lv_label_set_text(cal_cells[i], buf);
            lv_obj_clear_flag(cal_cells[i], LV_OBJ_FLAG_HIDDEN);
            if (cal_year == today_y && cal_month == today_m && d == today_d) {
                lv_obj_set_style_bg_color(cal_cells[i], lv_color_hex(0x00ff41), 0);
                lv_obj_set_style_bg_opa(cal_cells[i], LV_OPA_COVER, 0);
                lv_obj_set_style_text_color(cal_cells[i], lv_color_hex(0x0a0a0a), 0);
                lv_obj_set_style_radius(cal_cells[i], 4, 0);
            } else {
                lv_obj_set_style_bg_opa(cal_cells[i], LV_OPA_TRANSP, 0);
                lv_obj_set_style_text_color(cal_cells[i], lv_color_hex(0x00ff41), 0);
            }
        } else {
            lv_label_set_text(cal_cells[i], "");
            lv_obj_add_flag(cal_cells[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static void cal_year_changed_handler(lv_event_t *e)
{
    lv_obj_t *dd = lv_event_get_target(e);
    cal_year = CAL_YEAR_BASE + (int)lv_dropdown_get_selected(dd);
    cal_refresh();
}

static void cal_month_changed_handler(lv_event_t *e)
{
    lv_obj_t *dd = lv_event_get_target(e);
    cal_month = (int)lv_dropdown_get_selected(dd) + 1;
    cal_refresh();
}

static void create_calendar_ui(void)
{
    calendar_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(calendar_scr, lv_color_hex(0x0a0a0a), 0);

    /* 返回按钮 */
    lv_obj_t *back_btn = lv_btn_create(calendar_scr);
    lv_obj_set_size(back_btn, 60, 30);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_obj_set_style_bg_color(back_btn, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(back_btn, 0, 0);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "\xE8\xBF\x94\xE5\x9B\x9E"); /* 返回 */
    lv_obj_set_style_text_font(back_label, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(back_label, lv_color_hex(0x00ff41), 0);
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_click_handler, LV_EVENT_CLICKED, NULL);

    /* 标题 */
    lv_obj_t *title = lv_label_create(calendar_scr);
    lv_label_set_text(title, "\xE6\x97\xA5\xE5\x8E\x86"); /* 日历 */
    lv_obj_set_style_text_font(title, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x00ff41), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    /* 年月下拉选择 */
    cal_year_dd = lv_dropdown_create(calendar_scr);
    lv_dropdown_set_options_static(cal_year_dd, cal_year_options);
    lv_dropdown_set_symbol(cal_year_dd, NULL);
    lv_obj_set_size(cal_year_dd, 74, 26);
    lv_obj_align(cal_year_dd, LV_ALIGN_TOP_MID, -52, 43);
    lv_obj_set_style_bg_color(cal_year_dd, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(cal_year_dd, 0, 0);
    lv_obj_set_style_radius(cal_year_dd, 6, 0);
    lv_obj_set_style_text_font(cal_year_dd, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(cal_year_dd, lv_color_hex(0x00ff41), 0);
    lv_obj_add_event_cb(cal_year_dd, cal_year_changed_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *year_lbl = lv_label_create(calendar_scr);
    lv_label_set_text(year_lbl, "\xE5\xB9\xB4"); /* 年 */
    lv_obj_set_style_text_font(year_lbl, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(year_lbl, lv_color_hex(0x00ff41), 0);
    lv_obj_align(year_lbl, LV_ALIGN_TOP_MID, -8, 48);

    cal_month_dd = lv_dropdown_create(calendar_scr);
    lv_dropdown_set_options_static(cal_month_dd, cal_month_options);
    lv_dropdown_set_symbol(cal_month_dd, NULL);
    lv_obj_set_size(cal_month_dd, 45, 26);
    lv_obj_align(cal_month_dd, LV_ALIGN_TOP_MID, 35, 43);
    lv_obj_set_style_bg_color(cal_month_dd, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(cal_month_dd, 0, 0);
    lv_obj_set_style_radius(cal_month_dd, 6, 0);
    lv_obj_set_style_text_font(cal_month_dd, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(cal_month_dd, lv_color_hex(0x00ff41), 0);
    lv_obj_add_event_cb(cal_month_dd, cal_month_changed_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *month_lbl = lv_label_create(calendar_scr);
    lv_label_set_text(month_lbl, "\xE6\x9C\x88"); /* 月 */
    lv_obj_set_style_text_font(month_lbl, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(month_lbl, lv_color_hex(0x00ff41), 0);
    lv_obj_align(month_lbl, LV_ALIGN_TOP_MID, 69, 48);

    /* 星期标题 */
    static const char *wd[] = {"\xE6\x97\xA5","\xE4\xB8\x80","\xE4\xBA\x8C",
                               "\xE4\xB8\x89","\xE5\x9B\x9B","\xE4\xBA\x94","\xE5\x85\xAD"};
    for (int i = 0; i < 7; i++) {
        lv_obj_t *lbl = lv_label_create(calendar_scr);
        lv_label_set_text(lbl, wd[i]);
        lv_obj_set_style_text_font(lbl, &lv_font_cjk16, 0);
        lv_obj_set_style_text_color(lbl, lv_color_hex(0x00ff41), 0);
        lv_obj_align(lbl, LV_ALIGN_TOP_MID, (i - 3) * 35, 75);
    }

    /* 日期格子 */
    for (int i = 0; i < 42; i++) {
        int row = i / 7;
        int col = i % 7;
        cal_cells[i] = lv_label_create(calendar_scr);
        lv_obj_set_style_text_font(cal_cells[i], &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(cal_cells[i], lv_color_hex(0x00ff41), 0);
        lv_obj_set_style_pad_all(cal_cells[i], 4, 0);
        lv_obj_align(cal_cells[i], LV_ALIGN_TOP_MID, (col - 3) * 35, 95 + row * 24);
    }

    /* 初始化当前年月 */
    cal_goto_rtc_month();
    cal_refresh();
}

static void env_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    if (lv_scr_act() != env_scr) return;
    char buf[64];

    /* ---- AHT20 温湿度 ---- */
    uint32_t ct[2];
    ATH20_Read_CTdata(ct);
    float aht_temp = (float)ct[1] * 200.0f / 1048576.0f - 50.0f;
    float aht_humi = (float)ct[0] * 100.0f / 1048576.0f;

    snprintf(buf, sizeof(buf), "\xE6\xB8\xA9\xE5\xBA\xA6: %.1f \xE2\x84\x83", aht_temp);
    lv_label_set_text(env_temp_label, buf);

    snprintf(buf, sizeof(buf), "\xE6\xB9\xBF\xE5\xBA\xA6: %.1f %%", aht_humi);
    lv_label_set_text(env_humi_label, buf);

    /* ---- BMP280 气压/海拔 ---- */
    float bmp_press, bmp_temp, bmp_asl;
    BMP280GetData(&bmp_press, &bmp_temp, &bmp_asl);

    snprintf(buf, sizeof(buf), "\xE6\xB0\x94\xE5\x8E\x8B: %.1f hPa", bmp_press);
    lv_label_set_text(env_press_label, buf);

    snprintf(buf, sizeof(buf), "\xE6\xB5\xB7\xE6\x8B\x94: %.1f m", bmp_asl);
    lv_label_set_text(env_alti_label, buf);
}

static void create_environment_ui(void)
{
    env_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(env_scr, lv_color_hex(0x0a0a0a), 0);

    /* 返回按钮 */
    lv_obj_t *back_btn = lv_btn_create(env_scr);
    lv_obj_set_size(back_btn, 60, 30);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_obj_set_style_bg_color(back_btn, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(back_btn, 0, 0);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "\xE8\xBF\x94\xE5\x9B\x9E"); /* 返回 */
    lv_obj_set_style_text_font(back_label, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(back_label, lv_color_hex(0x00ff41), 0);
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_click_handler, LV_EVENT_CLICKED, NULL);

    /* 标题 */
    lv_obj_t *title = lv_label_create(env_scr);
    lv_label_set_text(title, "\xE7\x8E\xAF\xE5\xA2\x83"); /* 环境 */
    lv_obj_set_style_text_font(title, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x00ff41), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    /* 温度 */
    env_temp_label = lv_label_create(env_scr);
    lv_label_set_text(env_temp_label, "\xE6\xB8\xA9\xE5\xBA\xA6: --");
    lv_obj_set_style_text_font(env_temp_label, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(env_temp_label, lv_color_hex(0x00ff41), 0);
    lv_obj_align(env_temp_label, LV_ALIGN_TOP_LEFT, 10, 50);

    /* 湿度 */
    env_humi_label = lv_label_create(env_scr);
    lv_label_set_text(env_humi_label, "\xE6\xB9\xBF\xE5\xBA\xA6: --");
    lv_obj_set_style_text_font(env_humi_label, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(env_humi_label, lv_color_hex(0x00ff41), 0);
    lv_obj_align(env_humi_label, LV_ALIGN_TOP_LEFT, 10, 80);

    /* 气压 */
    env_press_label = lv_label_create(env_scr);
    lv_label_set_text(env_press_label, "\xE6\xB0\x94\xE5\x8E\x8B: --");
    lv_obj_set_style_text_font(env_press_label, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(env_press_label, lv_color_hex(0x00ff41), 0);
    lv_obj_align(env_press_label, LV_ALIGN_TOP_LEFT, 10, 110);

    /* 海拔 */
    env_alti_label = lv_label_create(env_scr);
    lv_label_set_text(env_alti_label, "\xE6\xB5\xB7\xE6\x8B\x94: --");
    lv_obj_set_style_text_font(env_alti_label, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(env_alti_label, lv_color_hex(0x00ff41), 0);
    lv_obj_align(env_alti_label, LV_ALIGN_TOP_LEFT, 10, 140);

    /* 每 2 秒刷新一次传感器数据 */
    lv_timer_create(env_timer_cb, 2000, NULL);
}

/**
 * @brief  创建机器人脸部UI（待机画面）
 * @note   黑色背景上用圆形和圆角矩形绘制简易机器人面孔
 *         - 双眼: 青色圆形(36x36), 黑色瞳孔(14x14)
 *         - 嘴巴: 青色圆角矩形(80x20)
 *         - 点击屏幕触发 idle_click_handler 回调
 */
static void create_robot_face(void)
{
    /* 创建空白屏幕，黑色背景 */
    idle_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(idle_scr, lv_color_black(), 0);

    /* 左眼: 绿色圆形, 36x36, 位置偏左上 */
    robot_eye_l = lv_obj_create(idle_scr);
    lv_obj_set_size(robot_eye_l, 36, 36);
    lv_obj_set_style_bg_color(robot_eye_l, lv_color_white(), 0);
    lv_obj_set_style_radius(robot_eye_l, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(robot_eye_l, 0, 0);
    lv_obj_align(robot_eye_l, LV_ALIGN_CENTER, -50, -40);

    /* 右眼: 绿色圆形, 36x36, 位置偏右上 */
    robot_eye_r = lv_obj_create(idle_scr);
    lv_obj_set_size(robot_eye_r, 36, 36);
    lv_obj_set_style_bg_color(robot_eye_r, lv_color_white(), 0);
    lv_obj_set_style_radius(robot_eye_r, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(robot_eye_r, 0, 0);
    lv_obj_align(robot_eye_r, LV_ALIGN_CENTER, 50, -40);

    /* 左瞳孔: 黑色圆形, 14x14, 叠加在左眼位置 */
    robot_pupil_l = lv_obj_create(idle_scr);
    lv_obj_set_size(robot_pupil_l, 14, 14);
    lv_obj_set_style_bg_color(robot_pupil_l, lv_color_black(), 0);
    lv_obj_set_style_radius(robot_pupil_l, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(robot_pupil_l, 0, 0);
    {
        lv_coord_t pw = lv_obj_get_width(idle_scr);
        lv_coord_t ph = lv_obj_get_height(idle_scr);
        lv_obj_set_pos(robot_pupil_l, pw / 2 - 50 - 7, ph / 2 - 40 - 7);
    }

    /* 右瞳孔: 黑色圆形, 14x14, 叠加在右眼位置 */
    robot_pupil_r = lv_obj_create(idle_scr);
    lv_obj_set_size(robot_pupil_r, 14, 14);
    lv_obj_set_style_bg_color(robot_pupil_r, lv_color_black(), 0);
    lv_obj_set_style_radius(robot_pupil_r, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(robot_pupil_r, 0, 0);
    {
        lv_coord_t pw = lv_obj_get_width(idle_scr);
        lv_coord_t ph = lv_obj_get_height(idle_scr);
        lv_obj_set_pos(robot_pupil_r, pw / 2 + 50 - 7, ph / 2 - 40 - 7);
    }

    /* 嘴巴: 绿色圆角矩形, 80x20, 居中偏下 */
    robot_mouth = lv_obj_create(idle_scr);
    lv_obj_set_size(robot_mouth, 80, 5);
    lv_obj_set_style_bg_color(robot_mouth, lv_color_white(), 0);
    lv_obj_set_style_radius(robot_mouth, 10, 0);
    lv_obj_set_style_border_width(robot_mouth, 0, 0);
    lv_obj_align(robot_mouth, LV_ALIGN_CENTER, 0, 30);

    /* 备选提示文字（已注释）: "Touch to Start" */
    //lv_obj_t *tip = lv_label_create(idle_scr);
    //lv_label_set_text(tip, "Touch to Start");
    //lv_obj_set_style_text_color(tip, lv_color_white(), 0);
    //lv_obj_align(tip, LV_ALIGN_BOTTOM_MID, 0, -30);

    /* 绑定点击事件: 点击屏幕任意位置触发 idle_click_handler */
    lv_obj_add_event_cb(idle_scr, idle_click_handler, LV_EVENT_CLICKED, NULL);
}

/**
 * @brief  创建AI对话页面UI
 * @note   提供输入框+折叠键盘+对话气泡列表
 *         - 顶部: 返回按钮 + "AI对话" 标题
 *         - 输入区: 输入框 + 键盘开关 + 发送按钮
 *         - 中部: 对话消息列表
 *         - 底部: 虚拟键盘（默认折叠）
 */
static void create_ai_ui(void)
{
    ai_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ai_scr, lv_color_hex(0x0a0a0a), 0);

    /* 返回按钮 */
    lv_obj_t *back_btn = lv_btn_create(ai_scr);
    lv_obj_set_size(back_btn, 60, 30);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_obj_set_style_bg_color(back_btn, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(back_btn, 0, 0);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "\xE8\xBF\x94\xE5\x9B\x9E"); /* 返回 */
    lv_obj_set_style_text_font(back_label, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(back_label, lv_color_hex(0x00ff41), 0);
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_click_handler, LV_EVENT_CLICKED, NULL);

    /* 标题 */
    lv_obj_t *title = lv_label_create(ai_scr);
    lv_label_set_text(title, "AI \xE5\xAF\xB9\xE8\xAF\x9D"); /* AI 对话 */
    lv_obj_set_style_text_font(title, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x00ff41), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    /* 输入框 + 发送按钮 + 键盘开关 */
    chat_input = lv_textarea_create(ai_scr);
    lv_obj_set_size(chat_input, 195, 36);
    lv_obj_align(chat_input, LV_ALIGN_TOP_MID, -15, 41);
    lv_obj_set_style_text_font(chat_input, &lv_font_cjk16, 0);
    lv_textarea_set_max_length(chat_input, 128);
    lv_textarea_set_placeholder_text(chat_input, "\xE8\xBE\x93\xE5\x85\xA5\xE6\xB6\x88\xE6\x81\xAF..."); /* 输入消息... */

    chat_send_btn = lv_btn_create(ai_scr);
    lv_obj_set_size(chat_send_btn, 42, 36);
    lv_obj_align(chat_send_btn, LV_ALIGN_TOP_RIGHT, -5, 41);
    lv_obj_set_style_bg_color(chat_send_btn, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(chat_send_btn, 0, 0);
    lv_obj_t *send_label = lv_label_create(chat_send_btn);
    lv_label_set_text(send_label, "\xE5\x8F\x91\xE9\x80\x81"); /* 发送 */
    lv_obj_set_style_text_font(send_label, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(send_label, lv_color_hex(0x00ff41), 0);
    lv_obj_center(send_label);
    lv_obj_add_event_cb(chat_send_btn, chat_send_handler, LV_EVENT_CLICKED, NULL);

    /* 对话列表 */
    chat_list = lv_obj_create(ai_scr);
    lv_obj_set_size(chat_list, 230, 150);
    lv_obj_align(chat_list, LV_ALIGN_TOP_MID, 0, 85);
    lv_obj_set_style_bg_color(chat_list, lv_color_hex(0x0d0d0d), 0);
    lv_obj_set_style_border_width(chat_list, 0, 0);
    lv_obj_set_scroll_dir(chat_list, LV_DIR_VER);
    lv_obj_set_flex_flow(chat_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(chat_list, 4, 0);

    /* 虚拟键盘: 最后创建=最上层，默认隐藏 */
    kb = lv_keyboard_create(ai_scr);
    lv_keyboard_set_textarea(kb, chat_input);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

    /* 欢迎语：仅 UI 显示，不加入对话历史（避免以 assistant 开头发给 API 被拒） */
    lv_obj_t *welcome_label = lv_label_create(chat_list);
    lv_label_set_text(welcome_label, "AI: \xE4\xBD\xA0\xE5\xA5\xBD\xEF\xBC\x8C\xE6\x88\x91\xE6\x98\xAF AI \xE5\x8A\xA9\xE6\x89\x8B"); /* AI: 你好，我是 AI 助手 */
    lv_obj_set_style_text_font(welcome_label, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(welcome_label, lv_color_hex(0xcccccc), 0);
    lv_label_set_long_mode(welcome_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(welcome_label, 220);

    /* ---- 拼音候选栏：左右箭头 + 6 个候选字按钮，贴键盘上方，默认隐藏 ---- */
    ime_bar = lv_obj_create(ai_scr);
    lv_obj_set_size(ime_bar, 240, 40);
    lv_obj_set_style_bg_opa(ime_bar, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(ime_bar, 0, 0);
    lv_obj_set_style_pad_all(ime_bar, 0, 0);
    lv_obj_align_to(ime_bar, kb, LV_ALIGN_OUT_TOP_MID, 0, 0);

    /* 左箭头 */
    lv_obj_t *prev_btn = lv_btn_create(ime_bar);
    lv_obj_set_size(prev_btn, 20, 34);
    lv_obj_align(prev_btn, LV_ALIGN_LEFT_MID, 5, 0);
    lv_obj_set_style_bg_color(prev_btn, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_radius(prev_btn, 6, 0);
    lv_obj_set_style_border_width(prev_btn, 0, 0);
    lv_obj_t *pl = lv_label_create(prev_btn);
    lv_label_set_text(pl, LV_SYMBOL_PREV);
    lv_obj_set_style_text_font(pl, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(pl, lv_color_white(), 0);
    lv_obj_center(pl);
    lv_obj_add_event_cb(prev_btn, ime_prev_handler, LV_EVENT_CLICKED, NULL);

    /* 右箭头 */
    lv_obj_t *next_btn = lv_btn_create(ime_bar);
    lv_obj_set_size(next_btn, 20, 34);
    lv_obj_align(next_btn, LV_ALIGN_RIGHT_MID, -5, 0);
    lv_obj_set_style_bg_color(next_btn, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_radius(next_btn, 6, 0);
    lv_obj_set_style_border_width(next_btn, 0, 0);
    lv_obj_t *nl = lv_label_create(next_btn);
    lv_label_set_text(nl, LV_SYMBOL_NEXT);
    lv_obj_set_style_text_font(nl, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(nl, lv_color_white(), 0);
    lv_obj_center(nl);
    lv_obj_add_event_cb(next_btn, ime_next_handler, LV_EVENT_CLICKED, NULL);

    /* 6 个候选字按钮 */
    int first_center = -(IME_MAX_CAND * 26 + (IME_MAX_CAND - 1) * 3) / 2 + 13;
    for (int i = 0; i < IME_MAX_CAND; i++) {
        ime_btns[i] = lv_btn_create(ime_bar);
        lv_obj_set_size(ime_btns[i], 26, 34);
        lv_obj_align(ime_btns[i], LV_ALIGN_CENTER, first_center + i * 29, 0);
        lv_obj_set_style_bg_color(ime_btns[i], lv_color_hex(0x0d2818), 0);
        lv_obj_set_style_radius(ime_btns[i], 6, 0);
        lv_obj_set_style_border_width(ime_btns[i], 0, 0);
        ime_lbls[i] = lv_label_create(ime_btns[i]);
        lv_label_set_text(ime_lbls[i], "");
        lv_obj_set_style_text_font(ime_lbls[i], &lv_font_cjk16, 0);
        lv_obj_set_style_text_color(ime_lbls[i], lv_color_white(), 0);
        lv_obj_center(ime_lbls[i]);
        lv_obj_add_event_cb(ime_btns[i], ime_cand_handler, LV_EVENT_CLICKED, NULL);
    }
    lv_obj_add_flag(ime_bar, LV_OBJ_FLAG_HIDDEN);

    /* 输入框内容变化 -> 刷新候选栏 */
    lv_obj_add_event_cb(chat_input, ime_value_changed_handler, LV_EVENT_VALUE_CHANGED, NULL);

    /* 点击输入框弹出键盘，点击空白处收起 */
    lv_obj_add_event_cb(chat_input, ai_input_click_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ai_scr, ai_scr_click_handler, LV_EVENT_CLICKED, NULL);
}

/* 添加一条消息到对话历史 */
static void chat_add_message(const char *role, const char *content)
{
    if (chat_count >= CHAT_HISTORY_MAX) {
        /* 移除最旧的一条，腾出空间 */
        for (int i = 1; i < CHAT_HISTORY_MAX; i++) {
            chat_history[i - 1] = chat_history[i];
        }
        chat_count = CHAT_HISTORY_MAX - 1;
    }
    snprintf(chat_history[chat_count].role, sizeof(chat_history[chat_count].role), "%s", role);
    snprintf(chat_history[chat_count].content, sizeof(chat_history[chat_count].content), "%s", content);
    chat_count++;
    chat_ui_update();
}

/* 把字符串按 JSON 规则转义后追加到 dst（cap=总容量，pos=当前写位置，返回新位置） */
static int json_append(char *dst, int cap, int pos, const char *s)
{
    static const char hex[] = "0123456789abcdef";
    while (*s && pos < cap - 1) {
        unsigned char c = (unsigned char)*s;
        if (c == '"' || c == '\\') {
            if (pos < cap - 2) { dst[pos++] = '\\'; dst[pos++] = (char)c; }
        } else if (c == '\n') {
            if (pos < cap - 2) { dst[pos++] = '\\'; dst[pos++] = 'n'; }
        } else if (c == '\r') {
            if (pos < cap - 2) { dst[pos++] = '\\'; dst[pos++] = 'r'; }
        } else if (c == '\t') {
            if (pos < cap - 2) { dst[pos++] = '\\'; dst[pos++] = 't'; }
        } else if (c < 0x20) {
            if (pos < cap - 6) {
                dst[pos++] = '\\';
                dst[pos++] = 'u';
                dst[pos++] = '0';
                dst[pos++] = '0';
                dst[pos++] = hex[c >> 4];
                dst[pos++] = hex[c & 0x0F];
            }
        } else {
            dst[pos++] = (char)c;
        }
        s++;
    }
    dst[pos] = '\0';
    return pos;
}

/* 构建 JSON 请求体，包含完整对话历史（只对 content 做 JSON 转义，结构用原始引号） */
static void chat_build_json(char *json, int json_sz)
{
    int pos = snprintf(json, json_sz,
        "{\"model\":\"%s\",\"messages\":[{\"role\":\"system\",\"content\":\"%s\"}%s",
        AI_MODEL, AI_SYSTEM_PROMPT, (chat_count > 0) ? "," : "");
    if (pos >= json_sz) pos = json_sz - 1;

    for (int i = 0; i < chat_count; i++) {
        if (pos >= json_sz - 1) break;
        pos += snprintf(json + pos, json_sz - pos,
            "{\"role\":\"%s\",\"content\":\"", chat_history[i].role);
        if (pos >= json_sz) pos = json_sz - 1;

        pos = json_append(json, json_sz, pos, chat_history[i].content);
        if (pos >= json_sz - 1) break;

        pos += snprintf(json + pos, json_sz - pos, "\"}%s",
            (i < chat_count - 1) ? "," : "");
        if (pos >= json_sz) pos = json_sz - 1;
    }

    if (pos < json_sz - 1) {
        snprintf(json + pos, json_sz - pos, "]}");
    }
    json[json_sz - 1] = '\0';
}

/* 发送用户消息并等待 AI 回复 */
static void chat_send_to_ai(const char *user_msg)
{
    chat_add_message("user", user_msg);
    chat_pending = 1;

    /* 把请求交给 esp8266Task 处理，避免阻塞 lvglTask（HTTP 需 10~60s） */
    char req[256];
    snprintf(req, sizeof(req), "%s", user_msg);
    if (osMessageQueuePut(esp_ai_req_queueHandle, req, 0, 100) != osOK) {
        chat_pending = 0;
        chat_add_message("assistant", "(\xE7\xB3\xBB\xE7\xBB\x9F\xE5\xBF\x99\xE7\xA2\x8C)"); /* (系统忙碌) */
    }
}

/* 刷新对话列表到 UI */
static void chat_ui_update(void)
{
    lv_obj_clean(chat_list);
    for (int i = 0; i < chat_count; i++) {
        char disp[280];
        if (strcmp(chat_history[i].role, "user") == 0) {
            snprintf(disp, sizeof(disp), "\xE4\xBD\xA0: %s", chat_history[i].content); /* 你: */
        } else {
            snprintf(disp, sizeof(disp), "AI: %s", chat_history[i].content);
        }
        lv_obj_t *label = lv_label_create(chat_list);
        lv_label_set_text(label, disp);
        lv_obj_set_style_text_font(label, &lv_font_cjk16, 0);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(label, 220);
        if (strcmp(chat_history[i].role, "user") == 0) {
            lv_obj_set_style_text_color(label, lv_color_hex(0x00ff41), 0);
        } else {
            lv_obj_set_style_text_color(label, lv_color_hex(0xcccccc), 0);
        }
    }
    lv_obj_scroll_to_view(lv_obj_get_child(chat_list, -1), LV_ANIM_OFF);
}

/* USER CODE END Application */
