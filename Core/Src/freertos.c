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
#include "lcd.h"
#include "touch.h"
#include "lvgl.h"

LV_FONT_DECLARE(lv_font_cjk16);
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "esp8266.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
static lv_obj_t *ai_text;

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
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

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
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for esp8266Task */
osThreadId_t esp8266TaskHandle;
const osThreadAttr_t esp8266Task_attributes = {
  .name = "esp8266Task",
  .stack_size = 1024 * 4,
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

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static void create_main_ui(void);
static void create_robot_face(void);
static void create_status_ui(void);
static void create_settings_ui(void);

static lv_obj_t *idle_scr;
static lv_obj_t *main_scr;
static lv_obj_t *status_scr;
static lv_obj_t *settings_scr;
static lv_obj_t *status_list;
static void esp_log(const char *text);
static void esp_log_ai(const char *text);
static void extract_content(const char *resp, char *out, int out_sz);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void lvglStartTask(void *argument);
void esp8266StartTask(void *argument);

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
static lv_obj_t *pending_scr = NULL;
/* USER CODE END Header_lvglStartTask */
void lvglStartTask(void *argument)
{
  /* USER CODE BEGIN lvglStartTask */
    static char ai_msg[512];
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    create_robot_face();
    create_status_ui();
    create_settings_ui();
    create_main_ui();
    
    lv_scr_load(status_scr);

    esp_msg_t msg;
    static int ai_reply_countdown = 0;

    for (;;) {
        if (osMessageQueueGet(esp_rx_queueHandle, &msg, NULL, 0) == osOK) {
            char display_text[64];
            
            switch (msg.type) {
                case ESP_MSG_INIT:
                    lv_list_add_text(status_list, "[OK] System Ready");
                    break;
                case ESP_MSG_AT_OK:
                    lv_list_add_text(status_list, "[OK] ESP8266 Connected");
                    break;
                case ESP_MSG_AT_FAIL:
                    snprintf(display_text, sizeof(display_text), "[ERR] AT Error: %s", msg.text);
                    lv_list_add_btn(status_list, NULL, display_text);
                    break;
                case ESP_MSG_WIFI_OK:
                    lv_list_add_text(status_list, "[OK] WiFi Connected");
                    lv_list_add_text(status_list, msg.text);
                    break;
                case ESP_MSG_WIFI_FAIL:
                    snprintf(display_text, sizeof(display_text), "[ERR] WiFi Failed: %s", msg.text);
                    lv_list_add_btn(status_list, NULL, display_text);
                    break;
                case 10:
                    lv_list_add_btn(status_list, NULL, msg.text);
                    break;
            }
        }
        if (osMessageQueueGet(esp_ai_queueHandle, ai_msg, NULL, 0) == osOK) {
            if (ai_text) lv_textarea_set_text(ai_text, ai_msg);
            ai_reply_countdown = 400;
        }
        if (ai_reply_countdown > 0) {
            ai_reply_countdown--;
            if (ai_reply_countdown == 0) {
                lv_scr_load(idle_scr);
            }
        }

        if (pending_scr) {
            lv_scr_load(pending_scr);
            pending_scr = NULL;
        }

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
    char json[256];

#define AI_URL "https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions"
#define AI_KEY "sk-ws-H.EPLHEIP.hWXu.MEYCIQDytiueAUS-gKpkG1X7QQ9DxoDTPCFth471SALe1gZnjgIhAJFrKg4RZ9edeHxWVbypeJmRSSaAmhASte47U3XpsJ0R"
#define AI_MODEL "qwen3.8-max"
#define AI_PROMPT "\xE4\xBD\xA0\xE5\xA5\xBD\xEF\xBC\x8C\xE8\xAF\xB7\xE7\x94\xA8\xE4\xB8\x80\xE5\x8F\xA5\xE8\xAF\x9D\xE4\xBB\x8B\xE7\xBB\x8D\xE4\xBD\xA0\xE8\x87\xAA\xE5\xB7\xB1\xE3\x80\x82"

    ESP_Init();
    esp_log("ESP Init");
    osDelay(2000);

    /* AT self-check */
    status = ESP_SendCmd("AT\r\n", "OK", 2000);
    if (status != ESP_OK) {
        esp_log("AT fail");
        for (;;) osDelay(1000);
    }

    /* connect WiFi */
    status = ESP_ConnectAP("HUAWEI-1FKZ9T", "FSNH82502");
    if (status != ESP_OK) {
        esp_log("WiFi fail");
        for (;;) osDelay(1000);
    }
    esp_log("WiFi OK");

    /* build request body */
    snprintf(json, sizeof(json),
        "{\"model\":\"%s\",\"messages\":[{\"role\":\"user\",\"content\":\"%s\"}]}",
        AI_MODEL, AI_PROMPT);

    esp_log("POST to Qwen...");
    status = ESP_HTTPPostAuth(AI_URL, json, AI_KEY, http_resp, (uint16_t)sizeof(http_resp));

    if (status == ESP_OK) {
        esp_log("HTTP OK, answer:");
        extract_content(http_resp, json, (int)sizeof(json));
        if (json[0] != '\0') {
            esp_log_ai(json);
        } else {
            esp_log("(no content)");
        }
    } else {
        char b[32];
        snprintf(b, sizeof(b), "HTTP fail %d", (int)status);
        esp_log(b);
    }

    for (;;) {
        osDelay(1000);
    }
  /* USER CODE END esp8266StartTask */
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

//static lv_obj_t *pending_scr = NULL;

static void main_menu_handler(lv_event_t *e)
{
    pending_scr = lv_event_get_user_data(e);
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
    lv_obj_set_style_bg_color(main_scr, lv_color_hex(0x1a1a2e), 0);

    /* 顶部标题: "菜单" */
    lv_obj_t *title = lv_label_create(main_scr);
    lv_label_set_text(title, "\xE8\x8F\x9C\xE5\x8D\x95"); /* 菜单 */
    lv_obj_set_style_text_font(title, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 15);

    /* 按钮1: "表情", 100x75, 左上位置, 深紫色 (#2d2d5e) */
    lv_obj_t *btn1 = lv_btn_create(main_scr);
    lv_obj_set_size(btn1, 100, 75);
    lv_obj_align(btn1, LV_ALIGN_CENTER, -65, -40);
    lv_obj_set_style_bg_color(btn1, lv_color_hex(0x2d2d5e), 0);
    lv_obj_set_style_radius(btn1, 12, 0);
    lv_obj_t *lbl1 = lv_label_create(btn1);
    lv_label_set_text(lbl1, "\xE8\xA1\xA8\xE6\x83\x85"); /* 表情 */
    lv_obj_set_style_text_font(lbl1, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(lbl1, lv_color_white(), 0);
    lv_obj_center(lbl1);
    lv_obj_add_event_cb(btn1, main_menu_handler, LV_EVENT_RELEASED, idle_scr);

    /* 按钮2: "状态", 100x75, 右上位置, 深紫色 (#2d2d5e) */
    lv_obj_t *btn2 = lv_btn_create(main_scr);
    lv_obj_set_size(btn2, 100, 75);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 65, -40);
    lv_obj_set_style_bg_color(btn2, lv_color_hex(0x2d2d5e), 0);
    lv_obj_set_style_radius(btn2, 12, 0);
    lv_obj_t *lbl2 = lv_label_create(btn2);
    lv_label_set_text(lbl2, "\xE7\x8A\xB6\xE6\x80\x81"); /* 状态 */
    lv_obj_set_style_text_font(lbl2, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(lbl2, lv_color_white(), 0);
    lv_obj_center(lbl2);
    lv_obj_add_event_cb(btn2, main_menu_handler, LV_EVENT_RELEASED, status_scr);

    /* 按钮3: "设置", 100x75, 左下位置, 深紫色 (#2d2d5e) */
    lv_obj_t *btn3 = lv_btn_create(main_scr);
    lv_obj_set_size(btn3, 100, 75);
    lv_obj_align(btn3, LV_ALIGN_CENTER, -65, 55);
    lv_obj_set_style_bg_color(btn3, lv_color_hex(0x2d2d5e), 0);
    lv_obj_set_style_radius(btn3, 12, 0);
    lv_obj_t *lbl3 = lv_label_create(btn3);
    lv_label_set_text(lbl3, "\xE8\xAE\xBE\xE7\xBD\xAE"); /* 设置 */
    lv_obj_set_style_text_font(lbl3, &lv_font_cjk16, 0);
    lv_obj_set_style_text_color(lbl3, lv_color_white(), 0);
    lv_obj_center(lbl3);
    lv_obj_add_event_cb(btn3, main_menu_handler, LV_EVENT_RELEASED, settings_scr);

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
    /* 创建空白屏幕作为容器 */
    status_scr = lv_obj_create(NULL);
    
    /* 返回按钮: 60x30, 左上角 */
    lv_obj_t *back_btn = lv_btn_create(status_scr);
    lv_obj_set_size(back_btn, 60, 30);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "\xE8\x8F\x9C\xE5\x8D\x95"); /* ← */
    lv_obj_set_style_text_font(back_label, &lv_font_cjk16, 0);
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_click_handler, LV_EVENT_CLICKED, NULL);

    /* 顶部标题: "系统状态" */
    lv_obj_t *title = lv_label_create(status_scr);
    lv_label_set_text(title, "\xE7\xB3\xBB\xE7\xBB\x9F\xE7\x8A\xB6\xE6\x80\x81"); /* 系统状态 */
    lv_obj_set_style_text_font(title, &lv_font_cjk16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    /* 状态列表: 230x180, 居中靠上 */
    status_list = lv_list_create(status_scr);
    lv_obj_set_size(status_list, 230, 180);
    lv_obj_align(status_list, LV_ALIGN_TOP_MID, 0, 45);

    /* 初始化占位项，提示用户系统正在启动 */
    lv_obj_t *init_item = lv_list_add_text(status_list, "--- \xE5\x88\x9D\xE5\xA7\x8B\xE5\x8C\x96\xE4\xB8\xAD..."); /* --- 初始化中... */
    lv_obj_set_style_text_font(init_item, &lv_font_cjk16, 0);

    /* 底部AI输出文本区域: 220x140, 白色背景, 灰色边框 */
    ai_text = lv_textarea_create(status_scr);
    lv_textarea_set_text(ai_text, "");
    lv_obj_set_size(ai_text, 220, 140);
    lv_obj_align(ai_text, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_text_font(ai_text, &lv_font_cjk16, 0);
    lv_obj_set_style_bg_color(ai_text, lv_color_white(), 0);
    lv_obj_set_style_border_width(ai_text, 1, 0);
    lv_obj_set_style_border_color(ai_text, lv_color_hex(0x404040), 0);
    lv_textarea_set_cursor_click_pos(ai_text, false);      /* 禁止点击定位光标 */
    lv_textarea_set_max_length(ai_text, 512);               /* 限制最大512字符 */
    lv_obj_add_flag(ai_text, LV_OBJ_FLAG_SCROLLABLE);       /* 启用滚动 */
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

    /* 左眼: 青色圆形, 36x36, 位置偏左上 */
    lv_obj_t *eye_left = lv_obj_create(idle_scr);
    lv_obj_set_size(eye_left, 36, 36);
    lv_obj_set_style_bg_color(eye_left, lv_palette_main(LV_PALETTE_CYAN), 0);
    lv_obj_set_style_radius(eye_left, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(eye_left, 0, 0);
    lv_obj_align(eye_left, LV_ALIGN_CENTER, -50, -40);

    /* 右眼: 青色圆形, 36x36, 位置偏右上 */
    lv_obj_t *eye_right = lv_obj_create(idle_scr);
    lv_obj_set_size(eye_right, 36, 36);
    lv_obj_set_style_bg_color(eye_right, lv_palette_main(LV_PALETTE_CYAN), 0);
    lv_obj_set_style_radius(eye_right, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(eye_right, 0, 0);
    lv_obj_align(eye_right, LV_ALIGN_CENTER, 50, -40);

    /* 左瞳孔: 黑色圆形, 14x14, 叠加在左眼位置 */
    lv_obj_t *eye_pupil_l = lv_obj_create(idle_scr);
    lv_obj_set_size(eye_pupil_l, 14, 14);
    lv_obj_set_style_bg_color(eye_pupil_l, lv_color_black(), 0);
    lv_obj_set_style_radius(eye_pupil_l, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(eye_pupil_l, 0, 0);
    lv_obj_align(eye_pupil_l, LV_ALIGN_CENTER, -50, -40);

    /* 右瞳孔: 黑色圆形, 14x14, 叠加在右眼位置 */
    lv_obj_t *eye_pupil_r = lv_obj_create(idle_scr);
    lv_obj_set_size(eye_pupil_r, 14, 14);
    lv_obj_set_style_bg_color(eye_pupil_r, lv_color_black(), 0);
    lv_obj_set_style_radius(eye_pupil_r, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(eye_pupil_r, 0, 0);
    lv_obj_align(eye_pupil_r, LV_ALIGN_CENTER, 50, -40);

    /* 嘴巴: 青色圆角矩形, 80x20, 居中偏下 */
    lv_obj_t *mouth = lv_obj_create(idle_scr);
    lv_obj_set_size(mouth, 80, 20);
    lv_obj_set_style_bg_color(mouth, lv_palette_main(LV_PALETTE_CYAN), 0);
    lv_obj_set_style_radius(mouth, 10, 0);
    lv_obj_set_style_border_width(mouth, 0, 0);
    lv_obj_align(mouth, LV_ALIGN_CENTER, 0, 30);

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

    /* 顶部标题: "设置" */
    lv_obj_t *title = lv_label_create(settings_scr);
    lv_label_set_text(title, "\xE8\xAE\xBE\xE7\xBD\xAE"); /* 设置 */
    lv_obj_set_style_text_font(title, &lv_font_cjk16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    /* 系统信息标签: 显示MCU型号、主频、LCD分辨率、RTOS框架 */
    lv_obj_t *info = lv_label_create(settings_scr);
    lv_label_set_text(info, "MCU: STM32F407VET6\n"
                            "Freq: 168MHz\n"
                            "LCD: 320x240\n"
                            "FreeRTOS + LVGL");
    lv_obj_set_style_text_font(info, &lv_font_cjk16, 0);
    lv_obj_align(info, LV_ALIGN_CENTER, 0, -20);

    /* 返回按钮: 100x40, 底部居中, 点击返回主菜单 */
    lv_obj_t *back_btn = lv_btn_create(settings_scr);
    lv_obj_set_size(back_btn, 100, 40);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "\xE8\xBF\x94\xE5\x9B\x9E"); /* 返回 */
    lv_obj_set_style_text_font(back_label, &lv_font_cjk16, 0);
    lv_obj_center(back_label);
    /* 绑定返回事件: 点击触发 back_click_handler 回到主菜单 */
    lv_obj_add_event_cb(back_btn, back_click_handler, LV_EVENT_CLICKED, NULL);
}

/* USER CODE END Application */