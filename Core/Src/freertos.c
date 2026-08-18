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
#include "lcd.h"
#include "touch.h"
#include <stdio.h>
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "esp8266.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
u16 test_colors[] = {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE};
const char* color_names[] = {"RED", "GREEN", "BLUE", "YELLOW", "CYAN", "MAGENTA", "WHITE"};
int color_index = 0;
int test_step = 0;
char display_buf[64];

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
  .stack_size = 1024 * 8,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for esp8266Task */
osThreadId_t esp8266TaskHandle;
const osThreadAttr_t esp8266Task_attributes = {
  .name = "esp8266Task",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for esp_tx_queue */
osMessageQueueId_t esp_tx_queueHandle;
const osMessageQueueAttr_t esp_tx_queue_attributes = {
  .name = "esp_tx_queue"
};
/* Definitions for esp_rx_queue */
osMessageQueueId_t esp_rx_queueHandle;
const osMessageQueueAttr_t esp_rx_queue_attributes = {
  .name = "esp_rx_queue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static void create_main_ui(void);
static void create_robot_face(void);
static void create_status_ui(void);

static lv_obj_t *idle_scr;
static lv_obj_t *main_scr;
static lv_obj_t *status_scr;
static lv_obj_t *status_list;
static void esp_log(const char *text);
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
  /* creation of esp_tx_queue */
  esp_tx_queueHandle = osMessageQueueNew (4, sizeof(esp_msg_t), &esp_tx_queue_attributes);

  /* creation of esp_rx_queue */
  esp_rx_queueHandle = osMessageQueueNew (4, sizeof(esp_msg_t), &esp_rx_queue_attributes);

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
    osDelay(1);
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
    create_main_ui();
    create_status_ui();
    
    lv_scr_load(status_scr);

    esp_msg_t msg;

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
                case ESP_MSG_RESP:
                    {
                        char clean_text[48];
                        int i, j = 0;
                        for (i = 0; msg.text[i] && j < 47; i++) {
                            if (msg.text[i] >= 32 && msg.text[i] < 127) {
                                clean_text[j++] = msg.text[i];
                            }
                        }
                        clean_text[j] = '\0';
                        snprintf(display_text, sizeof(display_text), ">> %s", clean_text);
                        lv_list_add_btn(status_list, NULL, display_text);
                    }
                    break;
                case 10:
                    lv_list_add_btn(status_list, NULL, msg.text);
                    break;
            }
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
#define AI_PROMPT "Hi! Please introduce yourself in one short English sentence."

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
            esp_log(json);
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
    lv_scr_load(idle_scr);
}



static void create_main_ui(void)
{
    main_scr = lv_obj_create(NULL);

    lv_obj_t *title = lv_label_create(main_scr);
    lv_label_set_text(title, "Robot Control");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t *btn1 = lv_btn_create(main_scr);
    lv_obj_set_size(btn1, 160, 50);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -30);
    lv_obj_t *btn1_label = lv_label_create(btn1);
    lv_label_set_text(btn1_label, "Menu 1");
    lv_obj_center(btn1_label);

    lv_obj_t *btn2 = lv_btn_create(main_scr);
    lv_obj_set_size(btn2, 160, 50);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 40);
    lv_obj_t *btn2_label = lv_label_create(btn2);
    lv_label_set_text(btn2_label, "Menu 2");
    lv_obj_center(btn2_label);

    lv_obj_t *back_btn = lv_btn_create(main_scr);
    lv_obj_set_size(back_btn, 100, 40);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, back_click_handler, LV_EVENT_CLICKED, NULL);
}

static void create_status_ui(void)
{
    status_scr = lv_obj_create(NULL);
    
    lv_obj_t *title = lv_label_create(status_scr);
    lv_label_set_text(title, "System Status");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    status_list = lv_list_create(status_scr);
    lv_obj_set_size(status_list, 230, 270);
    lv_obj_align(status_list, LV_ALIGN_TOP_MID, 0, 45);

    lv_obj_t *init_item = lv_list_add_text(status_list, "--- Initializing...");
}

static void create_robot_face(void)
{
    idle_scr = lv_obj_create(NULL);

    lv_obj_set_style_bg_color(idle_scr, lv_color_black(), 0);

    lv_obj_t *eye_left = lv_obj_create(idle_scr);
    lv_obj_set_size(eye_left, 36, 36);
    lv_obj_set_style_bg_color(eye_left, lv_palette_main(LV_PALETTE_CYAN), 0);
    lv_obj_set_style_radius(eye_left, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(eye_left, 0, 0);
    lv_obj_align(eye_left, LV_ALIGN_CENTER, -50, -40);

    lv_obj_t *eye_right = lv_obj_create(idle_scr);
    lv_obj_set_size(eye_right, 36, 36);
    lv_obj_set_style_bg_color(eye_right, lv_palette_main(LV_PALETTE_CYAN), 0);
    lv_obj_set_style_radius(eye_right, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(eye_right, 0, 0);
    lv_obj_align(eye_right, LV_ALIGN_CENTER, 50, -40);

    lv_obj_t *eye_pupil_l = lv_obj_create(idle_scr);
    lv_obj_set_size(eye_pupil_l, 14, 14);
    lv_obj_set_style_bg_color(eye_pupil_l, lv_color_black(), 0);
    lv_obj_set_style_radius(eye_pupil_l, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(eye_pupil_l, 0, 0);
    lv_obj_align(eye_pupil_l, LV_ALIGN_CENTER, -50, -40);

    lv_obj_t *eye_pupil_r = lv_obj_create(idle_scr);
    lv_obj_set_size(eye_pupil_r, 14, 14);
    lv_obj_set_style_bg_color(eye_pupil_r, lv_color_black(), 0);
    lv_obj_set_style_radius(eye_pupil_r, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(eye_pupil_r, 0, 0);
    lv_obj_align(eye_pupil_r, LV_ALIGN_CENTER, 50, -40);

    lv_obj_t *mouth = lv_obj_create(idle_scr);
    lv_obj_set_size(mouth, 80, 20);
    lv_obj_set_style_bg_color(mouth, lv_palette_main(LV_PALETTE_CYAN), 0);
    lv_obj_set_style_radius(mouth, 10, 0);
    lv_obj_set_style_border_width(mouth, 0, 0);
    lv_obj_align(mouth, LV_ALIGN_CENTER, 0, 30);

    //lv_obj_t *tip = lv_label_create(idle_scr);
    //lv_label_set_text(tip, "Touch to Start");
    //lv_obj_set_style_text_color(tip, lv_color_white(), 0);
    //lv_obj_align(tip, LV_ALIGN_BOTTOM_MID, 0, -30);

    lv_obj_add_event_cb(idle_scr, idle_click_handler, LV_EVENT_CLICKED, NULL);
}

/* USER CODE END Application */