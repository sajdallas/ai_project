#ifndef _HAODA_AI_CAM_H_
#define _HAODA_AI_CAM_H_ 

#include "asr.h"
#include "asr_softiic.h"
#include "HardwareSerial.h"
//设备地址
#define     HAODA_AICAM_ADDRESS             0x60
//
#define     haodaAI_HARDIIC              0   
#define     haodaAI_SOFTIIC              1

/* aicam card label */
enum card_label_e {
    kCardForward = 1,
    kCardLeft = 2,
    kCardRight = 3,
    kCardTurnAround = 4,
    kCardPark = 5,
    kCardGreenLight = 6,
    kCardRedLight = 7,
    kCardSpeed40 = 8,
    kCardSpeed60 = 9,
    kCardSpeed80 = 10,
    kCardCheck = 11,
    kCardCross = 12,
    kCardCircle = 13,
    kCardSquare = 14,
    kCardTriangle = 15,
    kCardPlus = 16,
    kCardMinus = 17,
    kCardDivide = 18,
    kCardEqual = 19,
    kCardZero = 20,
    kCardOne = 21,
    kCardTwo = 22,
    kCardThree = 23,
    kCardFour = 24,
    kCardFive = 25,
    kCardSix = 26,
    kCardSeven = 27,
    kCardEight = 28,
    kCardNine = 29,
    kCardA = 31,
    kCardB = 32,
    kCardC = 33,
    kCardD = 34,
    kCardE = 35,
    kCardF = 36,
    kCardG = 37,
    kCardH = 38,
    kCardI = 39,
    kCardJ = 40,
    kCardK = 41,
    kCardL = 42,
    kCardM = 43,
    kCardN = 44,
    kCardO = 45,
    kCardP = 46,
    kCardQ = 47,
    kCardR = 48,
    kCardS = 49,
    kCardT = 50,
    kCardU = 51,
    kCardV = 52,
    kCardW = 53,
    kCardX = 54,
    kCardY = 55,
    kCardZ = 56
};
/* aicamFactory 20 classes label */
enum class20_label_e {
    kAirplane = 1,
    kBicycle = 2,
    kBird = 3,
    kBoat = 4,
    kBottle = 5,
    kBus = 6,
    kCar = 7,
    kCat = 8,
    kChair = 9,
    kCow = 10,
    kTable = 11,
    kDog = 12,
    kHorse = 13,
    kMotorBike = 14,
    kPerson = 15,
    kPlant = 16,
    kSheep = 17,
    kSofa = 18,
    kTrain = 19,
    kMonitor = 20
};
/* aicamFactory color label */
enum color_label_e {
    kColorBlack = 1,
    kColorWhite = 2,
    kColorRed = 3,
    kColorGreen = 4,
    kColorBlue = 5,
    kColorYellow = 6
};

class haodaAI
{  
public:
    //register address define
    enum aicam_reg_e{
        kRegDeviceId        = 0x01,
        kRegFirmwareVersion = 0x02,
        kRegRestart         = 0x03,
        kRegSensorConfig1   = 0x04,
        kRegLock            = 0x05,
        kRegLedConfig       = 0x06,
        kRegLedLevel        = 0x08,
        kRegUart            = 0x09,
        kRegUSBConfig       = 0x0B,
        kRegScreenConfig    = 0x0C,
        kRegWiFiConfig      = 0x0E,
        kRegHWConfig        = 0x0F,
        kRegCameraConfig1   = 0x10,
        kRegCameraConfig2   = 0x11,
        kRegCameraConfig3   = 0x12,
        kRegCameraConfig4   = 0x13,
        kRegCameraConfig5   = 0x14,
        kRegSnapshot        = 0x1A,
        kRegFrameWidthH     = 0x1B,
        kRegFrameWidthL     = 0x1C,
        kRegFrameHeightH    = 0x1D,
        kRegFrameHeightL    = 0x1E,
        kRegFrameCount      = 0x1F,
        kRegVisionId        = 0x20,
        kRegVisionConfig1   = 0x21,
        kRegVisionConfig2   = 0x22,
        kRegParamNum        = 0x23,
        kRegParamId         = 0x24,
        kRegVisionStatus1   = 0x2A,
        kRegVisionStatus2   = 0x2B,
        kRegVisionDetect1   = 0x30,
        kRegVisionDetect2   = 0x31,
        kRegResultNumber    = 0x34,
        kRegResultId        = 0x35,
        kRegReadStatus1     = 0x36,
        kRegParamValue1H    = 0x70,
        kRegParamValue1L    = 0x71,
        kRegParamValue2H    = 0x72,
        kRegParamValue2L    = 0x73,
        kRegParamValue3H    = 0x74,
        kRegParamValue3L    = 0x75,
        kRegParamValue4H    = 0x76,
        kRegParamValue4L    = 0x77,
        kRegParamValue5H    = 0x78,
        kRegParamValue5L    = 0x79,
        kRegResultData1H    = 0x80,
        kRegResultData1L    = 0x81,
        kRegResultData2H    = 0x82,
        kRegResultData2L    = 0x83,
        kRegResultData3H    = 0x84,
        kRegResultData3L    = 0x85,
        kRegResultData4H    = 0x86,
        kRegResultData4L    = 0x87,
        kRegResultData5H    = 0x88,
        kRegResultData5L    = 0x89,
        kRegImageID         = 0x90,
        kRegImageConfig     = 0x91,
        kRegImageWriteAddr  = 0x92,
        kRegImageReadAddr   = 0x93,
        kRegImageXH         = 0x94,
        kRegImageXL         = 0x95,
        kRegImageYH         = 0x96,
        kRegImageYL         = 0x97,
        kRegImageWidthH     = 0x98,
        kRegImageWidthL     = 0x99,
        kRegImageHeightH    = 0x9A,
        kRegImageHeightL    = 0x9B,
        kRegScreenFillR     = 0x9C,
        kRegScreenFillG     = 0x9D,
        kRegScreenFillB     = 0x9E,
        kRegSn              = 0xD0,
    };

    enum aicam_vision_e {
        kVisionColor = 1,
        kVisionBlob = 2,
        kVisionAprilTag = 3,
        kVisionLine = 4,
        kVisionLearning = 5,
        kVisionCard = 6,
        kVisionFace = 7,
        kVision20Classes = 8,
        kVisionQrCode = 9,
        kVisionMotionDetect = 11,
        kVisionMaxType,
    };

    typedef union {
        struct {
            unsigned char status :1;
            unsigned char default_setting :1;  //!< set 1 to reset vision configuration
        };
        unsigned char vision_config_reg_value;
    } aicam_vision_conf1_t;
   
    haodaAI(uint8_t addr = HAODA_AICAM_ADDRESS);
    haodaAI(uint8_t sdapin, uint8_t sclpin, uint8_t addr = HAODA_AICAM_ADDRESS);
    void begin(); //初始化
    void haodaAI::set_algorithm_card();//开启卡片识别算法
    void haodaAI::set_algorithm_Blob();//开启色块识别算法
    void haodaAI::set_algorithm_20Classes();//开启物体识别算法
    void haodaAI::set_algorithm_Motion();//开启运动物体侦测算法
    void haodaAI::set_algorithm_qr();//开启二维码识别算法
    void haodaAI::set_algorithm_Line();//开启线条识别算法
    void haodaAI::set_algorithm_oc();//开启物体分类算法
    void haodaAI::set_algorithm_cr();//开启颜色识别算法
    void haodaAI::set_algorithm_tr();//开启标签识别算法
    void haodaAI::set_algorithm_fr();//开启人脸识别算法
    void haodaAI::set_param_Num(uint8_t num);//设置算法参数总数量
    void haodaAI::set_param_Blob(uint8_t param_id, uint8_t w, uint8_t h, uint8_t color);//设置色块识别算法附加参数
    void haodaAI::set_param_Color(uint8_t param_id, uint8_t x, uint8_t y, uint8_t w, uint8_t h);//设置颜色识别算法附加参数
    uint8_t haodaAI::getLabel();//获取结果标签
    uint8_t haodaAI::getResult();//获取检测结果数量
    void haodaAI::getResult(uint8_t param_id, uint8_t *result_data_buf);//获取全部检测结果实际数据(10个字节)
    String haodaAI::getQrCodeValue();//获取二维码数据（最多25个字符）

private:
    uint8_t _addr;
    uint8_t _sdapin;
    uint8_t _sclpin;
    uint8_t _connect;
    void write_byte(uint8_t reg, uint8_t value);
    uint8_t read_byte(uint8_t reg);
    void write_nbyte(uint16_t addr, uint8_t *pbuf, uint16_t len);
    void read_nbyte(uint16_t addr, uint8_t *pbuf, uint16_t len);
    uint8_t haodaAI::getDeviceID();
    uint8_t haodaAI::isDeviceReady();
    void haodaAI::VisionBegin(aicam_vision_e vision_type);
};

haodaAI::haodaAI(uint8_t addr = HAODA_AICAM_ADDRESS)
{
    _addr = addr;
    _connect = haodaAI_HARDIIC;
}

haodaAI::haodaAI(uint8_t sdapin, uint8_t sclpin,uint8_t addr = HAODA_AICAM_ADDRESS)
{
    _addr = addr;
    _connect = haodaAI_SOFTIIC;
    _sdapin = sdapin;
    _sclpin = sclpin;
}

//========================================================================
// 描述: 写数据.
// 参数: addr:写入的寄存器地址; p:写入的数据.
// 返回: none.
//========================================================================
void haodaAI::write_byte(uint8_t reg, uint8_t value) 
{
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = value;
    if(_connect == haodaAI_HARDIIC){
#if defined(tskKERNEL_VERSION_NUMBER)
        i2c_master_only_send(_addr,buf,2);
#endif
    }else{
#if defined(tskKERNEL_VERSION_NUMBER)
        vTaskSuspendAll();
#endif
        softiic.start((_addr<<1)|KI2C_WRITE);
        softiic.write(buf[0]);
        softiic.write(buf[1]);
        softiic.stop();
#if defined(tskKERNEL_VERSION_NUMBER)
        ( void ) xTaskResumeAll();
#endif
    }
}

//========================================================================
// 描述: 读取数据.
// 参数: addr:读取的寄存器地址.
// 返回: 读到的数据.
//========================================================================
uint8_t haodaAI::read_byte(uint8_t reg) 
{
    uint8_t buf[2] = {0};
    buf[0] = reg;
    if(_connect == haodaAI_HARDIIC){
#if defined(tskKERNEL_VERSION_NUMBER)
        i2c_master_send_recv(_addr,buf,1,1);
#endif
    }else{
#if defined(tskKERNEL_VERSION_NUMBER)
        vTaskSuspendAll();
#endif
        softiic.start((_addr<<1)|KI2C_WRITE);
        softiic.write((uint8_t)reg);
        softiic.restart((_addr<<1)|KI2C_READ);
        buf[0] = softiic.read(1);
        softiic.stop();
#if defined(tskKERNEL_VERSION_NUMBER)
        ( void ) xTaskResumeAll();
#endif
    }
    return buf[0];
}

//========================================================================
// 描述: 写多个字节数据.
// 参数: addr:写入的寄存器地址; p:写入的数据; number:需要写的数据个数.
// 返回: none.
//========================================================================
void haodaAI::write_nbyte(uint16_t addr, uint8_t *pbuf, uint16_t len)
{
    uint8_t i;
    for(i=0;i<len;i++)
    {
        write_byte(addr, *pbuf);
        // delay(5);
        addr++;
        pbuf++;
    }
}

//========================================================================
// 描述: 读取多个数据.
// 参数: addr:读取的寄存器地址; p:读回来的数据; number:需要读的数据个数.
// 返回: none.
//========================================================================
void haodaAI::read_nbyte(uint16_t addr, uint8_t *pbuf, uint16_t len)
{
    uint8_t i;
    for(i=0;i<len;i++)
    {
        *pbuf = read_byte(addr);
        addr++;
        pbuf++;
    }
}

uint8_t haodaAI::getDeviceID()
{
    return read_byte(kRegDeviceId);
}

uint8_t haodaAI::isDeviceReady()
{
    return read_byte(kRegSensorConfig1) & 0x01;
}

void haodaAI::VisionBegin(aicam_vision_e vision_type) 
{
    aicam_vision_conf1_t conf1;
    write_byte(kRegVisionId,vision_type);//设置算法类型
    conf1.default_setting = 1;//恢复默认参数
    conf1.status = 1;//开启算法
    write_byte(kRegVisionConfig1,conf1.default_setting);//
    write_byte(kRegVisionConfig1,conf1.status);//
}

void haodaAI::set_algorithm_card() 
{
    VisionBegin(kVisionCard);
}

void haodaAI::set_algorithm_Blob() 
{
    VisionBegin(kVisionBlob);
}

void haodaAI::set_algorithm_20Classes() 
{
    VisionBegin(kVision20Classes);
}

void haodaAI::set_algorithm_Motion() 
{
    VisionBegin(kVisionMotionDetect);
}

void haodaAI::set_algorithm_qr() 
{
    VisionBegin(kVisionQrCode);
}

void haodaAI::set_algorithm_Line() 
{
    VisionBegin(kVisionLine);
}

void haodaAI::set_algorithm_oc() 
{
    VisionBegin(kVisionLearning);
}

void haodaAI::set_algorithm_cr() 
{
    VisionBegin(kVisionColor);
}

void haodaAI::set_algorithm_tr() 
{
    VisionBegin(kVisionAprilTag);
}

void haodaAI::set_algorithm_fr() 
{
    VisionBegin(kVisionFace);
}

uint8_t haodaAI::getLabel()
{
    write_byte(kRegResultId,1);//设置结果ID1
    return read_byte(kRegResultData5L);
}

void haodaAI::set_param_Num(uint8_t num)
{
    write_byte(kRegParamNum,num);//设置算法参数总数量
}

void haodaAI::set_param_Blob(uint8_t param_id, uint8_t w, uint8_t h, uint8_t color)
{
    write_byte(kRegParamId,param_id);//设置算法参数编号
    write_byte(kRegParamValue3L,w);
    write_byte(kRegParamValue4L,h);
    write_byte(kRegParamValue5L,color);
}

void haodaAI::set_param_Color(uint8_t param_id, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    write_byte(kRegParamId,param_id);//设置算法参数编号
    write_byte(kRegParamValue1L,x);
    write_byte(kRegParamValue2L,y);
    write_byte(kRegParamValue3L,w);
    write_byte(kRegParamValue4L,h);
}

uint8_t haodaAI::getResult()
{
    return read_byte(kRegResultNumber);
}

void haodaAI::getResult(uint8_t param_id, uint8_t *result_data_buf)
{
    write_byte(kRegResultId,param_id);//设置结果参数编号
    read_nbyte(kRegResultData1H, result_data_buf, 10);
}

String haodaAI::getQrCodeValue()
{
    uint8_t result_num,charNum;
    uint8_t buf[50];
    char str[25];
    result_num = read_byte(kRegResultNumber);

    write_byte(kRegResultId,1);//设置结果ID1
    charNum = read_byte(kRegResultData5L);

    if ((result_num == 0) || (charNum == 0))
    {
        return String("err");
    }

    for(uint8_t i=0;i<result_num;i++)
    {
        write_byte(kRegResultId,i+2);//设置结果参数编号
        read_nbyte(kRegResultData1H, &buf[i*10], 10);
    }

    for(uint8_t i=0;i<charNum;i++)
    {
        str[i]=buf[i*2+1];
    }

    return String(str).substring(0,charNum);
}
//========================================================================
// 描述: 初始化.
// 参数: none.
// 返回: none.
//========================================================================
void haodaAI::begin()
{
    uint8_t value = 0;

    if(_connect == haodaAI_HARDIIC){
#if defined(tskKERNEL_VERSION_NUMBER)
    #if defined(TW_ASR_PRO)
        dpmu_set_io_reuse(PA2,THIRD_FUNCTION);
        dpmu_set_io_reuse(PA3,THIRD_FUNCTION);//配置IIC引脚
        iic_polling_init(IIC0,100,0,LONG_TIME_OUT);
    #else
        i2c_io_init();
        i2c_master_init(IIC0,100,LONG_TIME_OUT);
    #endif
#endif
    }else{
        softiic.begin(_sdapin,_sclpin);
    }

    if (!isDeviceReady())
    {
        Serial.println("haodaAI maybe have somethings err");
    }
    else
    {
        Serial.println("haodaAI is ready!");
    }
}


#endif