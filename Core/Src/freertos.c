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

/* ---- 拼音输入法候选栏 ---- */
#define IME_MAX_CAND 8                  /* 候选栏最多显示的候选字数 */
static lv_obj_t *ime_bar;               /* 候选栏容器 */
static lv_obj_t *ime_btns[IME_MAX_CAND];  /* 候选字按钮 */
static lv_obj_t *ime_lbls[IME_MAX_CAND];  /* 候选字标签 */
static char ime_cand[IME_MAX_CAND][4];    /* 候选字 UTF-8（3 字节 + \0） */
static int  ime_cand_plen[IME_MAX_CAND];  /* 候选字来源拼音组的拼音长度 */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define AI_URL "https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions"
#define AI_KEY "sk-ws-H.EPLHEIP.hWXu.MEYCIQDytiueAUS-gKpkG1X7QQ9DxoDTPCFth471SALe1gZnjgIhAJFrKg4RZ9edeHxWVbypeJmRSSaAmhASte47U3XpsJ0R"
#define AI_MODEL "qwen3.8-max"
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
static void create_status_ui(void);
static void create_settings_ui(void);
static void create_ai_ui(void);

static lv_obj_t *idle_scr;
static lv_obj_t *main_scr;
static lv_obj_t *status_scr;
static lv_obj_t *settings_scr;
static lv_obj_t *ai_scr;
static lv_obj_t *status_info_label;   /* 系统状态页面的信息标签 */
static lv_obj_t *robot_eye_l, *robot_eye_r, *robot_pupil_l, *robot_pupil_r, *robot_mouth; /* 机器人表情 */
static void esp_log(const char *text);
static void esp_log_ai(const char *text);
static void extract_content(const char *resp, char *out, int out_sz);

static void chat_add_message(const char *role, const char *content);
static void chat_send_to_ai(const char *user_msg);
static void chat_build_json(char *json, int json_sz);
static void chat_ui_update(void);
static void chat_send_handler(lv_event_t *e);
static void ai_input_click_handler(lv_event_t *e);
static void ai_scr_click_handler(lv_event_t *e);
static void ime_update_candidates(void);
static void ime_commit(const char *cand, int pinyin_len);
static void ime_cand_handler(lv_event_t *e);
static void ime_value_changed_handler(lv_event_t *e);

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
    create_status_ui();
    create_settings_ui();
    create_ai_ui();
    create_main_ui();
    
    lv_scr_load(idle_scr);

    /* 屏幕加载后初始化表情动画（此时布局已确定，坐标正确） */
    robot_face_anim_init(robot_eye_l, robot_eye_r, robot_pupil_l, robot_pupil_r, robot_mouth, idle_scr);

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
            if (status_line > 0) {
                strncat(status_buf, "\n", sizeof(status_buf) - strlen(status_buf) - 1);
            }
            strncat(status_buf, m.text, sizeof(status_buf) - strlen(status_buf) - 1);
            status_line++;
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
                lv_scr_load(status_scr);
                break;
            case ASR_CMD_AI_CHAT:
                lv_scr_load(ai_scr);
                break;
            case ASR_CMD_SETTINGS:
                lv_scr_load(settings_scr);
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

    esp_log("\xE6\xAD\xA3\xE5\x9C\xA8\xE5\x88\x9D\xE5\xA7\x8B\xE5\x8C\x96ESP8266..."); /* 正在初始化ESP8266... */
    ESP_Init();
    osDelay(2000);

    status = ESP_SendCmd("AT\r\n", "OK", 2000);
    if (status == ESP_OK) {
        esp_log("AT\xE9\x80\x9A\xE8\xAE\xAF\xE6\xAD\xA3\xE5\xB8\xB8"); /* AT通讯正常 */
    } else {
        esp_log("AT\xE9\x80\x9A\xE8\xAE\xAF\xE5\xA4\xB1\xE8\xB4\xA5"); /* AT通讯失败 */
        for (;;) osDelay(1000);
    }

    esp_log("\xE6\xAD\xA3\xE5\x9C\xA8\xE8\xBF\x9E\xE6\x8E\xA5WiFi..."); /* 正在连接WiFi... */
    status = ESP_ConnectAP("HUAWEI-1FKZ9T", "FSNH82502");
    if (status == ESP_OK) {
        esp_log("WiFi\xE8\xBF\x9E\xE6\x8E\xA5\xE6\x88\x90\xE5\x8A\x9F"); /* WiFi连接成功 */
    } else {
        esp_log("WiFi\xE8\xBF\x9E\xE6\x8E\xA5\xE5\xA4\xB1\xE8\xB4\xA5"); /* WiFi连接失败 */
        for (;;) osDelay(1000);
    }

    esp_log("\xE7\xB3\xBB\xE7\xBB\x9F\xE5\xB0\xB1\xE7\xBB\xAA"); /* 系统就绪 */

    for (;;) {
        /* 等待 lvglTask 发来的对话请求 */
        if (osMessageQueueGet(esp_ai_req_queueHandle, req, NULL, osWaitForever) == osOK) {
            chat_build_json(json, (int)sizeof(json));

            esp_log("POST to Qwen...");
            status = ESP_HTTPPostAuth(AI_URL, json, AI_KEY, http_resp, (uint16_t)sizeof(http_resp));

            if (status == ESP_OK) {
                extract_content(http_resp, json, (int)sizeof(json));
                if (json[0] != '\0') {
                    esp_log_ai(json);
                } else {
                    /* 解析不到 content：把服务器原始响应打出来（优先显示响应体，跳过 HTTP 头） */
                    char dump[CHAT_MSG_LEN];
                    const char *body = strstr(http_resp, "\r\n\r\n");
                    snprintf(dump, sizeof(dump), "[\xE6\x97\xA0\xE5\x86\x85\xE5\xAE\xB9] %.200s",
                             body ? body + 4 : http_resp);
                    esp_log_ai(dump);
                }
            } else {
                char b[32];
                snprintf(b, sizeof(b), "[\xE9\x94\x99\xE8\xAF\xAF %d]", (int)status); /* [错误 N] */
                esp_log_ai(b);
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
    m.type = 10;
    snprintf(m.text, sizeof(m.text), "%s", text);
    osMessageQueuePut(esp_rx_queueHandle, &m, 0, 100);
}

static void esp_log_ai(const char *text)
{
    char buf[512];
    snprintf(buf, sizeof(buf), "%s", text);
    osMessageQueuePut(esp_ai_queueHandle, buf, 0, 100);
}

static void extract_content(const char *resp, char *out, int out_sz)
{
    char *p = strstr(resp, "\"content\"");
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
            else out[j++] = *p;
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

static void main_to_idle_handler(lv_event_t *e)
{
    lv_scr_load(idle_scr);
}

static void main_to_status_handler(lv_event_t *e)
{
    lv_scr_load(status_scr);
}

static void main_to_settings_handler(lv_event_t *e)
{
    lv_scr_load(settings_scr);
}

static void main_to_ai_handler(lv_event_t *e)
{
    lv_scr_load(ai_scr);
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

    /* 收集所有以 run 前 L 个字母开头的拼音组里的候选字 */
    int cand_count = 0;
    for (int g = 0; g < (int)PY_GROUP_COUNT && cand_count < IME_MAX_CAND; g++) {
        if (strncmp(py_groups[g].py, run, L) == 0) {
            int plen = (int)strlen(py_groups[g].py);
            const char *chs = py_groups[g].chs;
            while (*chs && cand_count < IME_MAX_CAND) {
                memcpy(ime_cand[cand_count], chs, 3);   /* 每个汉字固定 3 字节 UTF-8 */
                ime_cand[cand_count][3] = '\0';
                ime_cand_plen[cand_count] = plen;
                cand_count++;
                chs += 3;
            }
        }
    }

    for (int i = 0; i < IME_MAX_CAND; i++) {
        if (i < cand_count) {
            lv_label_set_text(ime_lbls[i], ime_cand[i]);
            lv_obj_clear_flag(ime_btns[i], LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(ime_btns[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
    lv_obj_clear_flag(ime_bar, LV_OBJ_FLAG_HIDDEN);
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
 * @brief  创建主菜单页面UI
 * @note   深色背景上布局三个功能入口按钮，呈2列排列
 *         - 左上: "表情" 按钮 -> 跳转至机器人脸部画面
 *         - 右上: "状态" 按钮 -> 跳转至系统状态页面
 *         - 左下: "设置" 按钮 -> 跳转至设置页面
 *         - 按钮引用保存到全局变量 m_btn1/m_btn2/m_btn3
 */
static void create_main_ui(void)
{
    /* 创建空白屏幕，深色背景 (#1a1a2e) */
    main_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(main_scr, lv_color_hex(0x0a0a0a), 0);

    /* 顶部标题: "菜单" */
    lv_obj_t *title = lv_label_create(main_scr);
    lv_label_set_text(title, "\xE8\x8F\x9C\xE5\x8D\x95"); /* 菜单 */
    lv_obj_set_style_text_font(title, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x00ff41), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 15);

    /* 按钮1: "表情", 100x75, 左上位置, 暗绿底 (#0d2818) + 绿色边框 */
    lv_obj_t *btn1 = lv_btn_create(main_scr);
    lv_obj_set_size(btn1, 100, 75);
    lv_obj_align(btn1, LV_ALIGN_CENTER, -55, -40);
    lv_obj_set_style_bg_color(btn1, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(btn1, 0, 0);
    lv_obj_set_style_radius(btn1, 12, 0);
    lv_obj_t *lbl1 = lv_label_create(btn1);
    lv_label_set_text(lbl1, "\xE8\xA1\xA8\xE6\x83\x85"); /* 表情 */
    lv_obj_set_style_text_font(lbl1, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(lbl1, lv_color_hex(0x00ff41), 0);
    lv_obj_center(lbl1);
    lv_obj_add_event_cb(btn1, main_to_idle_handler, LV_EVENT_CLICKED, NULL);

    /* 按钮2: "状态", 100x75, 右上位置, 暗绿底 (#0d2818) + 绿色边框 */
    lv_obj_t *btn2 = lv_btn_create(main_scr);
    lv_obj_set_size(btn2, 100, 75);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 55, -40);
    lv_obj_set_style_bg_color(btn2, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(btn2, 0, 0);
    lv_obj_set_style_radius(btn2, 12, 0);
    lv_obj_t *lbl2 = lv_label_create(btn2);
    lv_label_set_text(lbl2, "\xE7\x8A\xB6\xE6\x80\x81"); /* 状态 */
    lv_obj_set_style_text_font(lbl2, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(lbl2, lv_color_hex(0x00ff41), 0);
    lv_obj_center(lbl2);
    lv_obj_add_event_cb(btn2, main_to_status_handler, LV_EVENT_CLICKED, NULL);

    /* 按钮3: "AI对话", 100x75, 左下位置, 暗绿底 (#0d2818) + 绿色边框 */
    lv_obj_t *btn3 = lv_btn_create(main_scr);
    lv_obj_set_size(btn3, 100, 75);
    lv_obj_align(btn3, LV_ALIGN_CENTER, -55, 55);
    lv_obj_set_style_bg_color(btn3, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(btn3, 0, 0);
    lv_obj_set_style_radius(btn3, 12, 0);
    lv_obj_t *lbl3 = lv_label_create(btn3);
    lv_label_set_text(lbl3, "AI\xE5\xAF\xB9\xE8\xAF\x9D"); /* AI对话 */
    lv_obj_set_style_text_font(lbl3, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(lbl3, lv_color_hex(0x00ff41), 0);
    lv_obj_center(lbl3);
    lv_obj_add_event_cb(btn3, main_to_ai_handler, LV_EVENT_CLICKED, NULL);

    /* 按钮4: "设置", 100x75, 右下位置, 暗绿底 (#0d2818) + 绿色边框 */
    lv_obj_t *btn4 = lv_btn_create(main_scr);
    lv_obj_set_size(btn4, 100, 75);
    lv_obj_align(btn4, LV_ALIGN_CENTER, 55, 55);
    lv_obj_set_style_bg_color(btn4, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(btn4, 0, 0);
    lv_obj_set_style_radius(btn4, 12, 0);
    lv_obj_t *lbl4 = lv_label_create(btn4);
    lv_label_set_text(lbl4, "\xE8\xAE\xBE\xE7\xBD\xAE"); /* 设置 */
    lv_obj_set_style_text_font(lbl4, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(lbl4, lv_color_hex(0x00ff41), 0);
    lv_obj_center(lbl4);
    lv_obj_add_event_cb(btn4, main_to_settings_handler, LV_EVENT_CLICKED, NULL);

}

/**
 * @brief  创建系统状态页面UI
 * @note   包含标题栏、状态列表和AI输出文本区域
 *         - 顶部: "系统状态" 标题
 *         - 中部: 状态列表，用于显示系统运行状态信息
 *         - 底部: 文本区域，用于显示AI交互输出
 */
static void create_status_ui(void)
{
    status_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(status_scr, lv_color_hex(0x0a0a0a), 0);

    /* 返回按钮 */
    lv_obj_t *back_btn = lv_btn_create(status_scr);
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
    lv_obj_t *title = lv_label_create(status_scr);
    lv_label_set_text(title, "\xE7\xB3\xBB\xE7\xBB\x9F\xE7\x8A\xB6\xE6\x80\x81"); /* 系统状态 */
    lv_obj_set_style_text_font(title, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x00ff41), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    /* 状态信息标签: 显示 ESP8266 初始化等实时状态 */
    status_info_label = lv_label_create(status_scr);
    lv_label_set_text(status_info_label, "\xE7\xAD\x89\xE5\xBE\x85\xE5\x88\x9D\xE5\xA7\x8B\xE5\x8C\x96..."); /* 等待初始化... */
    lv_obj_set_style_text_font(status_info_label, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(status_info_label, lv_color_hex(0x00ff41), 0);
    lv_obj_align(status_info_label, LV_ALIGN_CENTER, 0, -20);
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
 * @brief  创建设置页面UI
 * @note   显示系统硬件信息及开发环境，提供返回按钮
 *         - 顶部: "设置" 标题
 *         - 中部: 系统信息（MCU型号、主频、LCD分辨率、软件框架）
 *         - 底部: "返回" 按钮，点击触发 back_click_handler 返回上一页
 */
static void create_settings_ui(void)
{
    /* 创建空白屏幕作为容器 */
    settings_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(settings_scr, lv_color_hex(0x0a0a0a), 0);

    /* 顶部标题: "设置" */
    lv_obj_t *title = lv_label_create(settings_scr);
    lv_label_set_text(title, "\xE8\xAE\xBE\xE7\xBD\xAE"); /* 设置 */
    lv_obj_set_style_text_font(title, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x00ff41), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    /* 系统信息标签: 显示MCU型号、主频、LCD分辨率、RTOS框架 */
    lv_obj_t *info = lv_label_create(settings_scr);
    lv_label_set_text(info, "MCU: STM32F407VET6\n"
                            "Freq: 168MHz\n"
                            "LCD: 320x240\n"
                            "FreeRTOS + LVGL");
    lv_obj_set_style_text_font(info, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(info, lv_color_hex(0x00ff41), 0);
    lv_obj_align(info, LV_ALIGN_CENTER, 0, -20);

    /* 返回按钮: 100x40, 底部居中, 点击返回主菜单 */
    lv_obj_t *back_btn = lv_btn_create(settings_scr);
    lv_obj_set_size(back_btn, 100, 40);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_bg_color(back_btn, lv_color_hex(0x0d2818), 0);
    lv_obj_set_style_border_width(back_btn, 0, 0);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "\xE8\xBF\x94\xE5\x9B\x9E"); /* 返回 */
    lv_obj_set_style_text_font(back_label, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(back_label, lv_color_hex(0x00ff41), 0);
    lv_obj_center(back_label);
    /* 绑定返回事件: 点击触发 back_click_handler 回到主菜单 */
    lv_obj_add_event_cb(back_btn, back_click_handler, LV_EVENT_CLICKED, NULL);
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
    chat_list = lv_list_create(ai_scr);
    lv_obj_set_size(chat_list, 230, 120);
    lv_obj_align(chat_list, LV_ALIGN_TOP_MID, 0, 85);
    lv_obj_set_style_bg_color(chat_list, lv_color_hex(0x0d0d0d), 0);
    lv_obj_set_style_border_width(chat_list, 0, 0);

    /* 虚拟键盘: 最后创建=最上层，默认隐藏 */
    kb = lv_keyboard_create(ai_scr);
    lv_keyboard_set_textarea(kb, chat_input);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

    /* 欢迎语：仅 UI 显示，不加入对话历史（避免以 assistant 开头发给 API 被拒） */
    lv_obj_t *welcome_item = lv_list_add_text(chat_list, "AI: \xE4\xBD\xA0\xE5\xA5\xBD\xEF\xBC\x8C\xE6\x88\x91\xE6\x98\xAF AI \xE5\x8A\xA9\xE6\x89\x8B"); /* AI: 你好，我是 AI 助手 */
    lv_obj_set_style_text_font(welcome_item, &lv_font_cjk16, 0);

    /* ---- 拼音候选栏：透明容器 + 8 个候选字按钮，贴键盘上方，默认隐藏 ---- */
    ime_bar = lv_obj_create(ai_scr);
    lv_obj_set_size(ime_bar, 240, 40);
    lv_obj_set_style_bg_opa(ime_bar, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(ime_bar, 0, 0);
    lv_obj_set_style_pad_all(ime_bar, 0, 0);
    lv_obj_align_to(ime_bar, kb, LV_ALIGN_OUT_TOP_MID, 0, 0);

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
        "{\"model\":\"%s\",\"messages\":[", AI_MODEL);
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
        lv_obj_t *item = lv_list_add_text(chat_list, disp);
        lv_obj_set_style_text_font(item, &lv_font_cjk16, 0);
    }
}

/* USER CODE END Application */