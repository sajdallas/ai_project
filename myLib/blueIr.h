/**
 * @file blueIr.h
 * @brief 
 * @author hhdd 
 * @version 0.3
 * @date 2023-12-13
 * 
 * @copyright Copyright (c) 2023 haohaodada.com
 * 
 */
#ifndef _BULEIR_H_
#define _BULEIR_H_

#include "asr.h"
#include "pro_nvdata.h"
#ifdef __cplusplus
extern "C"{
#endif
#include "ir_data.h"
#ifdef __cplusplus
}
#endif

typedef enum _eDeviceMsgType
{
    TYPE_DEVICEMSG_LEARN_IR,            //来自语音数据的处理
    TYPE_DEVICEMSG_LEARN_KEY,           //timer来的数据，接收到的IR压缩后的数据(学码)
    TYPE_DEVICEMSG_AIR_MATCH,           //timer来的数据，接收到的IR压缩后的数据(匹配)
    TYPE_DEVICEMSG_SOFTTIM,             //软timer,1S超时，计时心跳统计
    TYPE_DEVICEMSG_SEND_KEY,            //连续发送按键的msg
    TYPE_DEVICEMSG_UART,                //连续发送按键的msg
    TYPE_DEVICEMSG_AIRC_CHECK_CALLBACK, //空调动作能否执行检测回调
    TYPE_DEVICEMSG_AIRC_SEND_CALLBACK,  //空调数据发送后回信息
    TYPE_DEVICEMSG_LEARN_SEND_CALLBACK, //学码数据发送回调信息
    TYPE_DEVICEMSG_LEARN_KEY_UART,      //UART串口学码
    TYPE_DEVICEMSG_MAX,
} eDeviceMsgType;

typedef struct
{
    eDeviceMsgType type;
    union
    {
        ir_data_t* pir_data;
        int        ir_code;
    }msg_data;
}device_msg;

typedef enum
{
    E_DEVICE_MODE_NORMAL,//无效模式
    E_DEVICE_MODE_LEARN,//学习模式
}eDevice_Mode;

typedef enum
{
    E_DEVICE_TYPE_AIR,      //空调设备
    E_DEVICE_TYPE_IR,       //普通红外
}eDevice_Type;

typedef struct _stDevice_S
{
    int device_index;   //设备ID   自动赋值
    eDevice_Type ir_type;
    uint32_t start_id;      
    uint32_t end_id;
    uint32_t start_keyworld;      
    uint32_t end_keyworld;
    uint32_t exti_keyworld;     //退出匹配的语音识别id
    eDevice_Mode ir_mode;
    int timeout;
    struct _stDevice_S* NextDevice;//指向下一个设备
}stDevice;

static   stDevice   *device_all = NULL;     //管理全部红外设备

volatile uint8_t    learnKeyIndex;  //学习按键的时候，学习到哪里了？
volatile uint8_t    ir_exti_flag;   //学码退出匹配标志

QueueHandle_t device_queue = NULL;

//用户自定义回调函数
typedef void (*blueAirCB)(void);
typedef void (*blueIrCB)(stDevice *device);

blueAirCB blueIrMatchAirTimeout;  //空调匹配超时回调函数
blueAirCB blueIrMatchAirSuccess;  //空调学习成功回调函数
blueIrCB blueIrMatchIrTimeout;    //红外匹配超时回调函数
blueIrCB blueIrMatchIrSuccess;    //红外学习成功回调函数

//全局函数
void blueIrPlayEndCB(){}
void blueIrTaskProcess();

/**
 * @brief 获取第一个设备
 * @return stDevice* 第一个设备指针
 */
static stDevice* blueIrGetFirstDevice(void)
{
    return device_all;
}

/**
 * @brief 获取最后一个设备 
 * @return stDevice* 
 */
static stDevice* blueIrGetFinalDevice(void)
{
    stDevice *device_temp = NULL;
    if (device_all == NULL){
        return NULL;
    }
    device_temp = device_all;
    while(device_temp->NextDevice)
    {
        device_temp = device_temp->NextDevice;
    }
    return device_temp;
}

/**
 * @brief 添加新设备之前做检查
 * 
 * @param device 
 * @return int 
 */
static int blueIrCheckDeviceBeforAdd(stDevice *device)
{
    stDevice *device_temp = NULL;
    if(device == NULL){
        return 0;
    }
    device_temp = device_all;
    while(device_temp){
        if (device_temp->device_index == device->device_index)  //设备列表里已经有该id
        {
            return 0;
        }
        device_temp = device_temp->NextDevice;
    }
    return 1;
}

/**
 * @brief 所有设备清除学习模式
 */
void blueIrCleanAllLearnMode()
{
    stDevice *device_temp = NULL;
    if(device_all == NULL){
        return;
    }
    device_temp = device_all;
    while(device_temp){
        if(device_temp->ir_mode == E_DEVICE_MODE_LEARN){
            device_temp->ir_mode = E_DEVICE_MODE_NORMAL;
        }
        device_temp = device_temp->NextDevice;
    }
}

/**
 * @brief 添加空调设备
 * 
 * @param  device 
 * @return int 
 */
int blueIrAddAirDevice(stDevice *device)
{
    stDevice *device_temp = NULL;
    int32_t    ir_air_id;      //空调id
    if(device == NULL){
        return 0;
    }
    device->device_index = (unsigned int)device;
    device->timeout = 12;      //自学习超时时间
    device->ir_type = E_DEVICE_TYPE_AIR;
    device->ir_mode = E_DEVICE_MODE_NORMAL;
    /* 初始化存储空间 */
    delay(100);
    //flash存储地址减10000
    asrpro_nvdata_init((void *)&ir_air_id,sizeof(ir_air_id),(30200+device->start_id));

    if(device_all == NULL){
        device_all = device;
    }else{
        if(blueIrCheckDeviceBeforAdd(device) == 0){
            return 0;
        }
        device_temp = blueIrGetFinalDevice();
        if(device_temp == NULL){
            return 0;
        }
        device_temp->NextDevice = device;
    }
    return 1;
}


/**
 * @brief 添加普通红外设备
 * 
 * @param device 
 * @return int 
 */
int blueIrAddIrDevice(stDevice *device)
{
    stDevice *device_temp = NULL;
    ir_data_t send_data;  //初始化一个红外数据的结构体
    if(device == NULL){
        return 0;
    }
    device->device_index   = (unsigned int)device;
    device->timeout = 12;      //自学习超时时间
    device->start_keyworld = device->start_id + 200;
    device->end_keyworld   = device->end_id + 200 - 1;
    device->ir_type = E_DEVICE_TYPE_IR;
    device->ir_mode = E_DEVICE_MODE_NORMAL;
    /* 初始化存储空间 */
    delay(100);
    memset(&send_data,0,sizeof(ir_data_t));
    for(uint32_t i=device->start_id; i<device->end_id; i++){
        asrpro_nvdata_init(&send_data,sizeof(ir_data_t),(30200+i));
    }

    if(device_all == NULL){
        device_all = device;
    }else{
        if(blueIrCheckDeviceBeforAdd(device) == 0){
            return 0;
        }
        device_temp = blueIrGetFinalDevice();
        if(device_temp == NULL){
            return 0;
        }
        device_temp->NextDevice = device;
    }
    return 1;
}

/**
 * @brief 空调设备学习,当接收到空调码或者学习超时运行该函数
 */
void blueIrLearnAir(eDeviceMsgType type, void* data)
{
    int ir_code = -1;
    stDevice *device_temp = NULL;
    stDevice *device = NULL;
    if(device_all == NULL){
        return;
    }
    device_temp = blueIrGetFirstDevice();
    while(device_temp){
        if((device_temp->ir_mode == E_DEVICE_MODE_LEARN)&&(device_temp->ir_type==E_DEVICE_TYPE_AIR)){
            device = device_temp;
            break;
        }
        device_temp = device_temp->NextDevice;
    }

    if(type == TYPE_DEVICEMSG_AIR_MATCH){
        if(!data){      //学习失败
            return;
        }
        ir_code = *(int *)data;
        if(ir_code < 1000){
            if(ir_code == 1){       //学习超时
                device->ir_mode = E_DEVICE_MODE_NORMAL;
                if(blueIrMatchAirTimeout != NULL){
                    blueIrMatchAirTimeout();
                }
            }
        }else{
            device->ir_mode = E_DEVICE_MODE_NORMAL;
            asrpro_nvdata_write((void *)&ir_code,sizeof(ir_code), (30200+device->start_id));    //保存空调数据
            prompt_play_by_cmd_id(device->end_id, -1, (play_done_callback_t)blueIrPlayEndCB, true);
            if (blueIrMatchAirSuccess != NULL){
                blueIrMatchAirSuccess();
            }
        }
    }
}

/**
 * @brief 红外设备学习,当接收到红外码或者学习超时运行该函数
 */
void blueIrLearnIr(eDeviceMsgType type, void* data)
{
    stDevice *device_temp = NULL;
    stDevice *device = NULL;
    ir_data_t *ir_data = NULL;

    if(device_all == NULL){
        return;
    }
    device_temp = blueIrGetFirstDevice();
    while(device_temp){
        if((device_temp->ir_mode == E_DEVICE_MODE_LEARN)&&(device_temp->ir_type==E_DEVICE_TYPE_IR)){
            device = device_temp;
            break;
        }
        device_temp = device_temp->NextDevice;
    }

    if(type == TYPE_DEVICEMSG_LEARN_KEY){
        int *pReturn = (int *)data;
        if(1 == *pReturn){      //错误播报
            if(ir_exti_flag == 1){     //退出匹配
                ir_exti_flag = 0;
                // Serial.println("ir exti ok");
            }else{
                ir_data_learn_ctl(IR_STOP_KEY_LEARN, NULL);
                if(blueIrMatchIrTimeout != NULL){
                    blueIrMatchIrTimeout(device);
                }
            }
            learnKeyIndex = 0;
            device->ir_mode = E_DEVICE_MODE_NORMAL;
            return;
        }
        ir_data = (ir_data_t *)data;
        if(ir_data == NULL){            //学习失败
            return;
        }

        // Serial.print("recv_addr:");
        // Serial.println(40000+device->start_id+learnKeyIndex);

        asrpro_nvdata_write(ir_data, sizeof(ir_data_t), (30200+device->start_id+learnKeyIndex));  //保存红外数据
        //判断是否到最后一个按键
        if( (device->start_id+learnKeyIndex+1) == device->end_id){
            ir_data_learn_ctl(IR_STOP_KEY_LEARN, NULL);
            prompt_play_by_cmd_id(device->end_id, -1, (play_done_callback_t)blueIrPlayEndCB, true);    //播报“学习成功” 
            learnKeyIndex = 0;
            device->ir_mode = E_DEVICE_MODE_NORMAL;
            if(blueIrMatchIrSuccess != NULL){
                blueIrMatchIrSuccess(device);
            }
        }else{
            learnKeyIndex++;
            prompt_play_by_cmd_id(device->start_id+learnKeyIndex, -1, (play_done_callback_t)blueIrPlayEndCB, false);
            set_state_enter_wakeup(20000);
            ir_data_learn_ctl(IR_START_KEY_LEARN, &device->timeout);
        }
    }
}

/**
 * @brief 开始学习空调/红外码
 * @param snid 
 */
void blueIrMatchAll(uint32_t snid)
{
    stDevice *device_temp = NULL;

    // Serial.println(device_all->start_id);

    if(device_all == NULL){
        return;
    }
    device_temp = device_all;
    while(device_temp){
        if(snid == device_temp->start_id){
            learnKeyIndex = 0;
            // Serial.println("learnIr"+String(device_temp->start_id));
            blueIrCleanAllLearnMode();
            device_temp->ir_mode = E_DEVICE_MODE_LEARN; //进入学习模式
            if(device_temp->ir_type == E_DEVICE_TYPE_AIR){      //学习空调码
                ir_data_learn_ctl(IR_START_AIR_KEY_MATCH, &device_temp->timeout);
            }else if(device_temp->ir_type == E_DEVICE_TYPE_IR){     //学习红外码
                ir_data_learn_ctl(IR_START_KEY_LEARN, &device_temp->timeout);
            }
            return;
        }
        device_temp = device_temp->NextDevice;
    }
}

/**
 * @brief 退出空调/红外码学习
 */
void blueIrExtiAll(uint32_t snid)
{
    stDevice *device_temp = NULL;
    if(device_all == NULL){
        return;
    }
    device_temp = blueIrGetFirstDevice();
    while(device_temp){
        if(snid == device_temp->exti_keyworld){
            if(device_temp->ir_mode == E_DEVICE_MODE_LEARN){
                if(device_temp->ir_type == E_DEVICE_TYPE_AIR){      //退出空调码学习
                    // Serial.println("exti air");
                    device_temp->ir_mode = E_DEVICE_MODE_NORMAL;
                    ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    return;
                }else if(device_temp->ir_type == E_DEVICE_TYPE_IR){     //退出红外码学习
                    // Serial.println("exti ir");
                    ir_exti_flag = 1;
                    ir_data_learn_ctl(IR_STOP_KEY_LEARN, NULL);
                    return;
                }
            }
        }
        device_temp = device_temp->NextDevice;
    }
}

/**
 * @brief 发送空调/红外数据
 * 
 */
void blueIrSendAll(uint32_t snid)
{
    stDevice *device_temp = NULL;
    if(device_all == NULL){
        return;
    }
    device_temp = device_all;
    while(device_temp){
        if( (snid >= device_temp->start_keyworld)&&(snid <= device_temp->end_keyworld) ){
            if(device_temp->ir_type == E_DEVICE_TYPE_AIR){
                int codeid = -1;
                // Serial.print("air_data_addr:");
                // Serial.println((40200+device_temp->start_id)); 
                asrpro_nvdata_read((30200+device_temp->start_id),sizeof(codeid),&codeid);
                ir_data_Air_Send_Ctl(codeid, snid - (device_temp->start_keyworld - 5));
                // Serial.print("send_air");
            }else if(device_temp->ir_type == E_DEVICE_TYPE_IR){
                // Serial.print("ir_data_addr:");
                // Serial.println(40000+snid); 

                ir_data_t send_data;  //初始化一个红外数据的结构体
                asrpro_nvdata_read((30000 + snid),sizeof(ir_data_t),&send_data);
                ir_Send_IR(&send_data);
                // Serial.print("send_ir");
            }
            return;
        }
        device_temp = device_temp->NextDevice;
    }
}

/**
 * @brief 语音识别处理函数
 * 
 * @param snid  语音识别id
 */
void blueIrDealASRMsg(uint32_t snid)
{
    /* 退出学习 */
    blueIrExtiAll(snid);

    /* 开始学习空调/红外码 */
    blueIrMatchAll(snid);

    /* 发送空调码 */
    blueIrSendAll(snid);
}

/**
 * @brief 红外伴侣程序开始运行
 * 
 * @param outpin 
 * @param revpin 
 * @param timerx 
 */
void blueIrStart(uint8_t outpin, uint8_t revpin, timer_base_t timerx)
{
    stIrPinInfo irPinInfo;
    memset(&irPinInfo, 0x00, sizeof(stIrPinInfo));

    //config outpin
    irPinInfo.outPin.PinName    = (PinPad_Name)pinToFun[outpin];
    if(outpin<16){
        irPinInfo.outPin.GpioBase = (gpio_base_t)(HAL_PA_BASE+(outpin/8)*0x1000);
    }
    else if(outpin<24)
    {
        irPinInfo.outPin.GpioBase = (gpio_base_t)HAL_PC_BASE;
    }else{
        irPinInfo.outPin.GpioBase = (gpio_base_t)HAL_PD_BASE;
    }
    irPinInfo.outPin.PinNum = (gpio_pin_t)(1 << (outpin % 8));
    if((outpin<=23) && (outpin>=17)) //设置功能为GPIO
    {
        irPinInfo.outPin.IoFun      = SECOND_FUNCTION;
    }
    else
    {
        irPinInfo.outPin.IoFun      = FIRST_FUNCTION;
    }

    if(outpin==0)
    {
        irPinInfo.outPin.PwmFun     = SECOND_FUNCTION;
        irPinInfo.outPin.PwmBase    = PWM5;
    }
    if(outpin>=2&&outpin<=6)
    {
        irPinInfo.outPin.PwmFun     = FIFTH_FUNCTION;
        if(outpin<6){
            irPinInfo.outPin.PwmBase = (pwm_base_t)(PWM0 + 0x1000 * (outpin-2));
        }else{
            irPinInfo.outPin.PwmBase = PWM4;
        }
    }else if(outpin>=7&&outpin<=12)
    {
        irPinInfo.outPin.PwmFun     = SECOND_FUNCTION;
        if(outpin<11){
            irPinInfo.outPin.PwmBase = (pwm_base_t)(PWM0 + 0x1000 * (outpin-7));
        }else{
            irPinInfo.outPin.PwmBase = (pwm_base_t)(PWM4 + 0x1000 * (outpin-11));
        }
    }else if(outpin>=13&&outpin<=16)
    {
        irPinInfo.outPin.PwmFun     = FORTH_FUNCTION;
        if(outpin<16){
            irPinInfo.outPin.PwmBase = (pwm_base_t)(PWM1 + 0x1000 * (outpin-13));
        }else{
            irPinInfo.outPin.PwmBase = PWM4;
        }
    }else if(outpin>=17&&outpin<=20)
    {
        irPinInfo.outPin.PwmFun     = FORTH_FUNCTION;
        irPinInfo.outPin.PwmBase    = (pwm_base_t)(PWM3 - 0x1000 * (outpin-17));
    } 

    //config revpin
    irPinInfo.revPin.PinName    = (PinPad_Name)pinToFun[revpin];
    if(revpin<16){
        irPinInfo.revPin.GpioBase = (gpio_base_t)(HAL_PA_BASE+(revpin/8)*0x1000);
    }
    else if(revpin<24)
    {
        irPinInfo.revPin.GpioBase = (gpio_base_t)HAL_PC_BASE;
    }else{
        irPinInfo.revPin.GpioBase = (gpio_base_t)HAL_PD_BASE;
    }
    irPinInfo.revPin.PinNum = (gpio_pin_t)(1 << (revpin % 8));
    if((revpin<=23) && (revpin>=17)) //设置功能为GPIO
    {
        irPinInfo.revPin.IoFun      = SECOND_FUNCTION;
    }else{
        irPinInfo.revPin.IoFun      = FIRST_FUNCTION;
    }
    irPinInfo.revPin.GpioIRQ    = (IRQn_Type)(PA_IRQn+(revpin/8));

    //timer config
    irPinInfo.irTimer.ir_use_timer      = timerx;
    if(timerx == TIMER0){
        irPinInfo.irTimer.ir_use_timer_IRQ  = TIMER0_IRQn;
    }else if(timerx == TIMER1){
        irPinInfo.irTimer.ir_use_timer_IRQ  = TIMER1_IRQn;
    }else if(timerx == TIMER2){
        irPinInfo.irTimer.ir_use_timer_IRQ  = TIMER2_IRQn;
    }else if(timerx == TIMER3){
        irPinInfo.irTimer.ir_use_timer_IRQ  = TIMER3_IRQn;
    }
    ir_init(&irPinInfo);
    ir_hw_init();

    device_queue = xQueueCreate(10, sizeof(device_msg));
    xTaskCreate((TaskFunction_t)blueIrTaskProcess,"blueIrTaskProcess",512,NULL,4,NULL);
}

/**
 * @brief 匹配或学码进程处理函数
 */
void blueIrTaskProcess()
{
    ir_data_t   *ir_data;
    device_msg  rev_msg;
    BaseType_t  err = pdFAIL;
    while (1){
        memset(&rev_msg, 0x00, sizeof(device_msg));
        err = xQueueReceive(device_queue,&rev_msg,portMAX_DELAY);
        if(pdPASS != err){
            continue;
        }
        switch(rev_msg.type){
            case TYPE_DEVICEMSG_LEARN_KEY:  //红外学码
                blueIrLearnIr(rev_msg.type, (void *)rev_msg.msg_data.pir_data);
            break;
            case TYPE_DEVICEMSG_AIR_MATCH:  //匹配(空调)
                blueIrLearnAir(rev_msg.type, (void *)&rev_msg.msg_data.ir_code);
                break;
            case TYPE_DEVICEMSG_AIRC_SEND_CALLBACK:
            case TYPE_DEVICEMSG_LEARN_SEND_CALLBACK:
                // blueIrSendCB(); 
                break;
            case TYPE_DEVICEMSG_AIRC_CHECK_CALLBACK:
                //null
                break;
            default:
                break;
        }
         vTaskDelay(1);
    }
    vTaskDelete(NULL);
}

/**
 * @brief 发送学习到的命令码
 * @param device    设备
 * @param command   发送第几条指令
 */
void blueIrSendAllCommand(stDevice *device, uint32_t command)
{
    uint32_t keyworld;
    if(device_all == NULL){
        return;
    }
    if(device->ir_type == E_DEVICE_TYPE_AIR){       //如果为空调设备
        int codeid = -1;
        asrpro_nvdata_read((30200+device->start_id), sizeof(codeid), &codeid);
        ir_data_Air_Send_Ctl(codeid, 5+command);
    }else if(device->ir_type == E_DEVICE_TYPE_IR){
        //231213修改，调整判断位置，空调部分不满足下方判断条件
        if((device->start_id + command) > device->end_id){
            return;
        }        
        ir_data_t send_data;  //初始化一个红外数据的结构体
        asrpro_nvdata_read((30200 + device->start_id + command),sizeof(ir_data_t),&send_data);
        ir_Send_IR(&send_data);
    }
}



/**
 * @brief 注册空调学习超时回调函数
 */
void blueIrRegisterAirTimeoutFun(blueAirCB fun)
{
    if(fun == NULL)return;
    blueIrMatchAirTimeout = fun;
}

/**
 * @brief 注册空调学习成功回调函数
 */
void blueIrRegisterAirSuccessFun(blueAirCB fun)
{
    if(fun == NULL)return;
    blueIrMatchAirSuccess = fun;
}

/**
 * @brief 注册红外设备学习成功回调函数
 */
void blueIrRegisterIrTimeoutFun(blueIrCB fun)
{
    if(fun == NULL)return;
    blueIrMatchIrTimeout = fun;
}

/**
 * @brief 注册红外设备学习成功回调函数
 */
void blueIrRegisterIrSuccessFun(blueIrCB fun)
{
    if(fun == NULL)return;
    blueIrMatchIrSuccess = fun;
}

/**
 * @brief 红外自学习回调函数
 * @param msgtyp 消息类型，见eDeviceMsgType.
 * @param pDevice
 * 
 */
#ifdef __cplusplus
extern "C"{
#endif
int device_common_sendmsg(eDeviceMsgType msgtyp, void *pDevice, void* pKey ,void* dat)
{
    device_msg rev_msg;
    int ret = -2;
    memset(&rev_msg, 0x00, sizeof(device_msg));
    switch (msgtyp)
    {
        case TYPE_DEVICEMSG_LEARN_KEY:  //红外学码
            rev_msg.type = msgtyp;
            rev_msg.msg_data.pir_data = (ir_data_t*)dat;
            xQueueSend(device_queue, &rev_msg, 0);
          break;
        case TYPE_DEVICEMSG_AIR_MATCH:  //匹配(空调)
            rev_msg.type = msgtyp;
            if (NULL == dat){
                rev_msg.msg_data.ir_code = 0;
            }else{
                rev_msg.msg_data.ir_code = (*(int*)dat);
            }        
            xQueueSend(device_queue,&rev_msg,0);
            break;
        case TYPE_DEVICEMSG_AIRC_SEND_CALLBACK:
        case TYPE_DEVICEMSG_LEARN_SEND_CALLBACK:
            rev_msg.type = msgtyp;
            xQueueSend(device_queue,&rev_msg,0);
            break;
        default:
            break;
    }
    return 0;
}
#ifdef __cplusplus
}
#endif



#endif
