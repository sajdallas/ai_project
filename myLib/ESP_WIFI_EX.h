/**
 * @file    ESP_WIFI.h
 * @brief   AT指令驱动esp8266/esp32的wifi头文件
 * @version 0.1
 * @date    2022-11-23
 * @author  hhdd
 * @copyright Copyright (c) 2021 TWen51 Technology Co., Ltd.
 */

#ifndef _ESP_WIFI_H
#define _ESP_WIFI_H

#include "asr.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "HardwareSerial.h"

#define ESP_IDLE 1          // 模块处于空闲模式
#define TCPIP_NEW_MESSAGE 2 // 接收到一帧数据
#define TCPIP_CLOSED 3      // 服务器已关闭
#define TCPIP_CLIENT_ON 4   // 客户端连接
#define MQTT_RECV_TOPIC 5   // MQTT接收到订阅的主题消息

/*----------新增加部分------------*/

#define WIFI_AP 101                   // AP配网消息
#define WIFI_WEB 102                  // WEB配网消息
#define WIFI_WECHAT 103               // 微信配网消息
#define WIFI_CONNECT 104              // WIFI连接成功消息
#define WIFI_DISCONNECT 105           // WIFI断开连接消息
#define WIFI_SMARTCONFIG 106          // SmartConfig配网消息
#define WIFI_AIRKISSING 107           // AirKiss配网消息
#define WIFI_WEBCONFIG_COMPLETE 108   // 网页配网已成功连接
#define WIFI_WEBCONFIG_EXECUTEING 109 // 网页配网处理中
#define WIFI_SMARTCONFIG_COMPLETE 110 // 智能配网已成功连接

/*----------新增加部分------------*/

// 工作模式
#define WIFI_MODE_STATION '1' // STA模式
#define WIFI_MODE_AP '2'      // AP模式
#define WIFI_MODE_BOTH '3'    // STA+AP模式

// 网络传输层协议，枚举类型
typedef enum
{
    enumTCP,
    enumUDP,
} ENUM_NetPro_TypeDef;

// ESP8266的AP模式加密方式选择
typedef enum
{
    OPEN = 0,
    WPA_PSK = 1,
    WPA2_PSK = 2,
    WPA_WPA2_PSK = 3,
} ENUM_ECN_TypeDef;

class ESPWIFI : public Stream
{
public:
    ESPWIFI(HardwareSerial *serialx);
    void begin(); // 初始化

    /* 基础AT命令 */
    bool check();    // 确认esp设备是否存在
    bool reset();    // 复位
    bool restore();  // 恢复出厂设置
    String getGMR(); // 查询版本信息

    /* WIFI命令 */
    bool setMode(char mode);                                                      // 设置Wi-Fi模式
    char checkMode();                                                             // 获取Wi-Fi模式
    String getIP();                                                               // 查询本地IP地址
    String getCWLAP();                                                            // 查询附近wifi
    bool connectAP(String ssid, String password);                                 // 连接网络
    bool enableAP(String ssid, String password, uint8 chl, ENUM_ECN_TypeDef ecn); // 配置AP参数
    bool openTCPServer(int port, int timeout);                                    // 建立TCP服务器

    /* TCP/IP命令 */
    bool connectServer(ENUM_NetPro_TypeDef enumE, String serverIP, String serverPort); // 连接服务器
    bool disconnectServer();                                                           // 断开当前连接的服务器
    bool setSingleConnect();                                                           // 设为单连接
    bool setMultiConnect();                                                            // 设为多连接
    bool setPureDataMode();                                                            // 进入透传模式
    bool exitPureDataMode();                                                           // 退出透传模式
    bool sendMessage(String str);                                                      // 发送消息
    bool sendMessage(int index, String str);                                           // 发送消息（多连接）
    uint8_t getWorkingID();                                                            // 多连接时接收到的ID（多连接）
    int getFailConnectID();                                                            // 获取连接失败的ID（多连接）

    /*----------新增加部分------------*/
    bool openSmartConfig(void);  // 开启智能配网
    bool openWebConfig(void);    // 开启WEB配网
    bool closeSmartConfig(void); // 关闭智能配网
    bool closeWebConfig(void);   // 关闭WEB配网

    String checkCmdAndGetString(String cmd_s); // 检查命令是否正确，返回命令结果字符串
    bool checkCmdIsOK(String cmd_s);           // 检查命令结果是否OK，返回true/false
    uint8_t checkConfigMessage(void);          // 接收配网信息
    /*----------新增加部分------------*/
    /* http命令 */
    String http_get(String url);
    String http_post(String url, String dat);

    /* MQTT命令 */
    bool MQTTClient(String id, String user, String pswd, int keepalive); // 设置连接属性
    bool MQTTConnect(String server, uint32_t port);                      // 连接MQTT
    bool MQTTPublish(String topic, String str);                          // 发送主题
    bool MQTTSubscribe(String topic);                                    // 订阅主题
    bool MQTTUnsubscribe(String topic);                                  // 取消订阅
    bool MQTTClean();                                                    // MQTT断开连接

    /* 接收到的消息处理函数 */
    int checkMessage();    // 接收消息，并返回消息类型
    String getMessage();   // 获取接收到的消息
    String getTopicName(); // 获取接收到的主题名

    /* 继承自Stream类可以像使用串口类来通信 */
    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual void flush(void);
    virtual size_t write(uint8_t);
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }

    /* 串口读写数据 */
    void writeData(String str);
    String readData(uint32_t timeout);
    String readData(uint32_t timeout, String s);

private:
    HardwareSerial *_serial;
    int connectID; // 多连接的ID
    int workingID;
    int failConnectID;
    bool multiFlag;
    String message;
    String staIP;
    String apIP;

    String topicName; // 接收到的订阅消息的主题名

    String temp_str;
    String str_t; // 保存字符串用临时变量

    char wifiMode;                // 模式
    volatile bool isPureDataMode; // 是否开启了透传模式
    bool setMux(int flag);
};

ESPWIFI::ESPWIFI(HardwareSerial *serialx)
{
    _serial = serialx;
    wifiMode = WIFI_MODE_STATION;
}

/************************************************************************
 * @brief  初始化.
 * @param  serialx 串口.
 * @return none
 **********************************************************************/
void ESPWIFI::begin()
{
    this->isPureDataMode = false;
    this->_serial->begin(115200);
}

/************************************************************************
 * @brief  确认esp模块是否存在.
 * @param  none
 * @return ture,检测到ESP模块; false,没有检测到模块
 **********************************************************************/
bool ESPWIFI::check()
{
    this->_serial->flush(); // 清空缓存区数据
    writeData("AT");
    if (readData(200, "OK\r\n") != "")
    {
        return true;
    }
    return false;
}

/************************************************************************
 * @brief  复位.
 * @param  none
 * @return ture,复位成功; false,复位失败
 **********************************************************************/
bool ESPWIFI::reset()
{
    str_t = "";
    this->_serial->flush();
    writeData("AT+RST");
    str_t = readData(200);
    if (str_t.indexOf("OK") > 0)
    {
        return true;
    }
    return false;
}

/************************************************************************
 * @brief  恢复出厂设置并复位.
 * @param  none
 * @return ture,复位成功; false,复位失败
 **********************************************************************/
bool ESPWIFI::restore()
{
    str_t = "";
    this->_serial->flush();
    writeData("AT+RESTORE");
    if (readData(1000, "OK\r\n") != "")
    {
        delay(5000); // 等待重启完成
        return true;
    }
    delay(5000); // 等待重启完成
    return false;
}

/************************************************************************
 * @brief  设置esp工作模式.
 * @param  mode -  WIFI_MODE_STATION   -   STA模式
 *                 WIFI_MODE_AP        -   AP模式
 *                 WIFI_MODE_BOTH      -   STA+AP模式
 * @return true:成功；false:失败.
 **********************************************************************/
bool ESPWIFI::setMode(char mode)
{
    str_t = "";
    this->_serial->flush();
    writeData("AT+CWMODE=" + String(mode));
    str_t = readData(200);
    if ((str_t.indexOf("no change") > 0) || (str_t.indexOf("OK") > 0))
    {
        this->wifiMode = mode;
        return true;
    }
    else
    {
        this->wifiMode = checkMode();
        return false;
    }
}

/************************************************************************
 * @brief  连接网络.
 * @param  ssid      账号.
 * @param  password  密码.
 * @return true:成功；false:失败.
 **********************************************************************/
bool ESPWIFI::connectAP(String ssid, String password)
{
    this->_serial->flush();
    this->_serial->println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\""); // 连接路由器

    if (readData(5000, "OK\r\n") != "")
    {
        return true;
    }

    return false;
}

/************************************************************************
 * @brief  开启多连接（内部使用）.
 * @param  flag      1,多连接; 0,单连接.
 * @return true:成功；false:失败.
 **********************************************************************/
bool ESPWIFI::setMux(int flag)
{
    str_t = "";
    this->_serial->flush();
    writeData("AT+CIPMUX=" + String(flag));
    str_t = readData(100);
    if ((str_t.indexOf("OK") > 0) || (str_t.indexOf("link is builded") > 0))
    {
        return true;
    }
    return false;
}

/************************************************************************
 * @brief  关闭多连接.
 * @param  none
 * @return true:成功；false:失败.
 **********************************************************************/
bool ESPWIFI::setSingleConnect()
{
    this->connectID = 0;
    this->multiFlag = false;
    return setMux(0);
}

/************************************************************************
 * @brief  开启多连接.
 * @param  none
 * @return true:成功；false:失败.
 **********************************************************************/
bool ESPWIFI::setMultiConnect()
{
    this->connectID = 0;
    this->multiFlag = true;
    return setMux(1);
}

/************************************************************************
 * @brief  连接服务器.
 * @param  enumE       enumTCP-TCP连接;
 *                     enumUDP-UDP连接.
 * @param  serverIP    ip地址.
 * @param  serverPort  端口号.
 * @return true:成功；false:失败.
 **********************************************************************/
bool ESPWIFI::connectServer(ENUM_NetPro_TypeDef enumE, String serverIP, String serverPort)
{
    this->_serial->flush();
    if (!this->multiFlag)
    { // 没有开启多连接
        if (enumE == enumTCP)
        {
            writeData("AT+CIPSTART=\"TCP\",\"" + serverIP + "\"," + serverPort);
        }
        else
        {
            writeData("AT+CIPSTART=\"UDP\",\"" + serverIP + "\"," + serverPort);
        }
        if (readData(5000, "OK\r\n") != "")
        {
            return true;
        }
        return false;
    }
    else
    {
        if (enumE == enumTCP)
        {
            writeData("AT+CIPSTART=" + String(this->connectID) + ",\"TCP\",\"" + serverIP + "\"," + serverPort);
        }
        else
        {
            writeData("AT+CIPSTART=" + String(this->connectID) + ",\"UDP\",\"" + serverIP + "\"," + serverPort);
        }
        if (readData(5000, "OK\r\n") != "")
        {
            return true;
        }
        return false;
    }
}

/************************************************************************
 * @brief  断开当前连接的服务器.
 * @param  none
 * @return true:成功；false:失败.
 **********************************************************************/
bool ESPWIFI::disconnectServer()
{
    this->_serial->flush();
    writeData("AT+CIPCLOSE");
    if (readData(200, "OK\r\n") != "")
    {
        return true;
    }
    return false;
}

/************************************************************************
 * @brief  发送消息.
 * @param  str  消息.
 * @return true:成功；false:失败.
 **********************************************************************/
bool ESPWIFI::sendMessage(String str)
{
    if (this->isPureDataMode)
    {
        taskENTER_CRITICAL();
        this->_serial->print(str);
        taskEXIT_CRITICAL();
        return true;
    }
    else
    {
        int len = 0;
        len = str.length();
        writeData("AT+CIPSEND=" + String(len));
        if (readData(200, ">") != "")
        {
            taskENTER_CRITICAL();
            this->_serial->print(str);
            taskEXIT_CRITICAL();
            if (readData(1000, "SEND OK\r\n") != "")
            {
                return true;
            }
        }
    }
    return false;
}

/************************************************************************
 * @brief  发送消息(用于多连接).
 * @param  index  给哪个设备连接发
 * @param  str    消息
 * @return true:成功；false:失败.
 **********************************************************************/
bool ESPWIFI::sendMessage(int index, String str)
{
    int len = 0;

    writeData("AT+CIPSEND=" + String(index) + "," + String(str.length()));
    if (readData(200, ">\r\n") != "")
    {
        taskENTER_CRITICAL();
        this->_serial->print(str);
        taskEXIT_CRITICAL();
        if (readData(1000, "SEND OK\r\n") != "")
        {
            return true;
        }
    }
    return false;
}

/************************************************************************
 * @brief  获取WorkingID（用于多连接）.
 * @param  none
 * @return id.
 **********************************************************************/
uint8_t ESPWIFI::getWorkingID()
{
    return this->workingID;
}

/************************************************************************
 * @brief  获取failConnectID.
 * @param  none
 * @return id.
 **********************************************************************/
int ESPWIFI::getFailConnectID()
{
    return this->failConnectID;
}

/************************************************************************
 * @brief  创建服务器.
 * @param  port        端口
 * @param  timeout     超时时间
 * @return true:成功；false:失败.
 **********************************************************************/
bool ESPWIFI::openTCPServer(int port, int timeout)
{
    if (setMux(1))
    {
        String str = "";
        writeData("AT+CIPSERVER=1," + String(port));
        str = readData(200);
        if (str.indexOf("OK") > 0)
        {
            writeData("AT+CIPSTO=" + String(timeout));
            str = readData(200);
            if (str.indexOf("OK"))
            {
                return true;
            }
        }
    }
    return false;
}

/************************************************************************
 * @brief  使能AP模式.
 * @param  ssid        接入点名称
 * @param  password    密码(8-64字节ASCII)
 * @param  chl         信道号
 * @param  ecn         加密方式(OPEN, WPA_PSK,WPA2_PSK,WPA_WPA2_PSK)
 * @return true:成功；false:失败.
 **********************************************************************/
bool ESPWIFI::enableAP(String ssid, String password, uint8 chl, ENUM_ECN_TypeDef ecn)
{
    String str = "";
    writeData("AT+CWSAP=\"" + ssid + "\",\"" + password + "\"," + String(chl) + String((uint8_t)ecn));
    str = readData(200);
    if (str.indexOf("OK") > 0)
    {
        return true;
    }
    return false;
}

/************************************************************************
 * @brief  获取当前工作模式.
 * @param  none
 * @return WIFI_MODE_STATION/WIFI_MODE_AP/WIFI_MODE_BOTH.
 **********************************************************************/
char ESPWIFI::checkMode()
{
    this->_serial->flush();
    writeData("AT+CWMODE?");
    String str = readData(200);
    if (str.indexOf('1') > 0)
        return WIFI_MODE_STATION;
    else if (str.indexOf('2') > 0)
        return WIFI_MODE_AP;
    else if (str.indexOf('3') > 0)
        return WIFI_MODE_BOTH;
    else
        return '0';
}

/************************************************************************
 * @brief  获取版本信息.
 * @param  none
 * @return 版本信息字符串.
 **********************************************************************/
String ESPWIFI::getGMR()
{
    writeData("AT+GMR");
    return readData(200);
}

/************************************************************************
 * @brief  获取IP.
 * @param  none
 * @return IP信息字符串.
 **********************************************************************/
String ESPWIFI::getIP()
{
    writeData("AT+CIFSR");
    String tmp = readData(200);
    if (this->wifiMode == WIFI_MODE_STATION)
    {
        int index1 = tmp.indexOf("STAIP");
        int index2 = tmp.indexOf("+CIFSR:STAMAC");
        this->staIP = tmp.substring(index1 + 7, index2 - 3);
        return this->staIP;
    }
    else
    {
        int index1 = tmp.indexOf("APIP");
        int index2 = tmp.indexOf("+CIFSR:APMAC");
        this->apIP = tmp.substring(index1 + 6, index2 - 3);
        return this->apIP;
    }
}

/************************************************************************
 * @brief  获取附近wifi信息.
 * @param  none
 * @return wifi信息字符串.
 **********************************************************************/
String ESPWIFI::getCWLAP()
{
    writeData("AT+CWLAP");
    return readData(200);
}

/************************************************************************
 * @brief  设置为透传模式.
 * @param  none
 * @return true,成功; false,失败.
 **********************************************************************/
bool ESPWIFI::setPureDataMode()
{
    writeData("AT+CIPMODE=1");
    String tmp = readData(200);
    if (tmp.indexOf("OK") > 0)
    {
        writeData("AT+CIPSEND");
        this->isPureDataMode = true;
        return true;
    }
    else
        return false;
}

/************************************************************************
 * @brief  退出透传模式.
 * @param  none
 * @return true,成功; false,失败.
 **********************************************************************/
bool ESPWIFI::exitPureDataMode()
{
    this->_serial->print("+++");
    delay(200);
    this->isPureDataMode = false;
    return check();
}

/*************************************** HTTP *******************************************/
/************************************************************************
 * @brief   http发送get请求
 * @param   url 网址
 * @return  返回的数据
 **********************************************************************/
String ESPWIFI::http_get(String url)
{
    uint32_t wait_count = 0;
    int index_e = 0, index_s = 0;
    int http_size = 0;
    int length = 0;
    str_t = "";
    temp_str = "";

    this->_serial->flush();
    writeData("AT+HTTPGETSIZE=\"" + url + "\""); // 获取资源大小
    str_t = readData(5000, "OK\r\n");
    index_s = str_t.indexOf("+HTTPGETSIZE:");
    if (index_s > 0)
    {
        index_e = str_t.indexOf("\r\n", index_s);
        http_size = str_t.substring(index_s + 13, index_e).toInt(); // 获取HTTP资源大小
    }
    delay(10);
    this->_serial->flush();
    writeData("AT+HTTPCLIENT=2,0,\"" + url + "\",,,1");
    str_t = readData(5000, "OK\r\n");
    if (http_size <= 360)
    {
        index_s = str_t.indexOf("+HTTPCLIENT:");
        if (index_s > 0)
        {
            index_e = str_t.indexOf(",", index_s);
            length = str_t.substring(index_s + 12, index_e).toInt(); // 获取数据的zise
            return str_t.substring(index_e + 1, str_t.length() - 4);
        }
    }
    else
    {
        index_s = 0;
        index_e = 0;
        index_s = str_t.indexOf("+HTTPCLIENT:");
        for (int i = 0; i < (http_size / 360 + 1); i++)
        {
            if (index_s > 0)
            {
                index_e = str_t.indexOf(",", index_s);
                length = str_t.substring(index_s + 12, index_e).toInt(); // 获取数据长度
                if (i == (http_size / 360))
                { // 最后一帧数据
                    temp_str += str_t.substring(index_e + 1, index_e + length + 1);
                    str_t = "";
                    return temp_str;
                }
                else
                {
                    temp_str += str_t.substring(index_e + 1, index_e + length + 1);
                    index_s += (length + 18);
                }
            }
        }
    }

    return "";
}

/************************************************************************
 * @brief   http发送post请求
 * @param   url 网址
 * @param   dat 数据
 * @return  返回的数据
 **********************************************************************/
String ESPWIFI::http_post(String url, String dat)
{
    uint32_t count = 0;
    this->str_t = "";
    this->_serial->flush();
    writeData("AT+HTTPCLIENT=3,0,\"" + url + "\",,,1,\"" + dat + "\"");
    this->str_t = readData(5000);
    int index_s = (this->str_t).indexOf("+HTTPCLIENT:");
    if (index_s > 0)
    {
        int index_e = (this->str_t).indexOf(",", index_s);
        int length = (this->str_t).substring(index_s + 12, index_e).toInt(); // 获取数据的zise
        return (this->str_t).substring(index_e + 1, (this->str_t).length() - 4);
    }
    return "";
}

/************************************** HTTP END ****************************************/

/*************************************** MQTT *******************************************/
/************************************************************************
 * @brief   设置MQTT属性.
 * @param   id             MQTT客户端ID
 * @param   user           用户名
 * @param   pswd           密码
 * @param   keepalive      MQTT ping超时时间
 * @return  返回的数据
 **********************************************************************/
bool ESPWIFI::MQTTClient(String id, String user, String pswd, int keepalive)
{
    String tmp = "";
    writeData("AT+MQTTUSERCFG=0,1,\"\",\"\",\"\",0,0,\"\"");
    tmp = "";
    tmp = readData(200);
    if (tmp.indexOf("OK") <= 0)
    {
        return false;
    }
    writeData("AT+MQTTUSERNAME=0,\"" + user + "\""); // 设置用户名
    tmp = "";
    tmp = readData(200);
    if (tmp.indexOf("OK") <= 0)
    {
        return false;
    }
    writeData("AT+MQTTPASSWORD=0,\"" + pswd + "\""); // 设置密码
    tmp = "";
    tmp = readData(200);
    if (tmp.indexOf("OK") <= 0)
    {
        return false;
    }
    writeData("AT+MQTTCLIENTID=0,\"" + id + "\""); // 设置客户端ID
    tmp = "";
    tmp = readData(200);
    if (tmp.indexOf("OK") <= 0)
    {
        return false;
    }
    writeData("AT+MQTTCONNCFG=0," + String(keepalive) + ",1,\"\",\"\",0,0"); // 设置ping超时时间
    tmp = "";
    tmp = readData(200);
    if (tmp.indexOf("OK") <= 0)
    {
        return false;
    }
    return true;
}

/************************************************************************
 * @brief   连接MQTT.
 * @param   server         域名
 * @param   port           端口
 * @return  返回的数据
 **********************************************************************/
bool ESPWIFI::MQTTConnect(String server, uint32_t port)
{
    String tmp = "";

    writeData("AT+MQTTCONN=0,\"" + server + "\"," + String(port) + ",1"); // 连接MQTT,自动重连
    if (readData(500, "OK\r\n") != "")
    {
        return true;
    }
    return false;
}

/************************************************************************
 * @brief   MQTT发送主题.
 * @param   topic          主题
 * @param   str            数据
 * @return  返回的数据
 **********************************************************************/
bool ESPWIFI::MQTTPublish(String topic, String str)
{
    writeData("AT+MQTTPUB=0,\"" + topic + "\",\"" + str + "\",1,0");
    if (readData(500, "OK\r\n") != "")
    {
        return true;
    }
    return false;
}

/************************************************************************
 * @brief   MQTT订阅主题.
 * @param   topic          主题
 * @return  返回的数据
 **********************************************************************/
bool ESPWIFI::MQTTSubscribe(String topic)
{
    writeData("AT+MQTTSUB=0,\"" + topic + "\",1");
    if (readData(500, "OK\r\n") != "")
    {
        return true;
    }
    return false;
}

/************************************************************************
 * @brief   MQTT取消订阅主题.
 * @param   topic          主题
 * @return  返回的数据
 **********************************************************************/
bool ESPWIFI::MQTTUnsubscribe(String topic)
{
    writeData("AT+MQTTUNSUB=0,\"" + topic + "\"");
    if (readData(500, "OK\r\n") != "")
    {
        return true;
    }
    return false;
}

/************************************************************************
 * @brief   MQTT断开连接.
 * @param   topic          主题
 * @return  返回的数据
 **********************************************************************/
bool ESPWIFI::MQTTClean()
{
    writeData("AT+MQTTCLEAN=0");
    if (readData(500, "OK\r\n") != "")
    {
        return true;
    }
    return false;
}
/************************************** MQTT END ****************************************/

/************************************************************************
 * @brief  确认消息(不能用于透传模式).
 * @param  none
 * @return .
 **********************************************************************/
int ESPWIFI::checkMessage()
{
    this->str_t = "";
    if (this->_serial->available() > 0)
    {
        (this->str_t) = this->_serial->readString();
    }
    if ((this->str_t) != "")
    {
        if ((this->str_t).substring(2, 6) == "+IPD")
        {
            if (!(this->multiFlag))
            {
                int index = (this->str_t).indexOf(":");
                int length = (this->str_t).substring(7, index + 1).toInt();
                this->message = (this->str_t).substring(index + 1, index + length + 1);
                return TCPIP_NEW_MESSAGE;
            }
            else
            {
                int id = 0, length = 0, index = 0;
                id = (this->str_t).substring(7, 8).toInt();
                index = (this->str_t).indexOf(":");
                length = (this->str_t).substring(9, index + 1).toInt();
                this->workingID = id;
                this->message = (this->str_t).substring(index + 1, index + length + 1);
                return TCPIP_NEW_MESSAGE;
            }
        }
        else if ((this->str_t).substring(0, 6) == "CLOSED" || ((this->str_t).charAt(1) == ',' && (this->str_t).substring(2, 8) == "CLOSED"))
        {
            if (!(this->multiFlag))
            {
                return TCPIP_CLOSED;
            }
            else
            {
                this->failConnectID = (this->str_t).charAt(0) - '0';
                return TCPIP_CLOSED;
            }
        }
        else if ((this->str_t).substring(1, 9) == ",CONNECT")
        {
            int index = (this->str_t).charAt(0) - '0';
            this->workingID = index;
            return TCPIP_CLIENT_ON;
        }
        else if ((this->str_t).substring(0, 12) == "+MQTTSUBRECV")
        {
            int index_s = (this->str_t).indexOf("\"");
            int index_e = (this->str_t).indexOf("\"", index_s + 1);
            this->topicName = (this->str_t).substring(index_s + 1, index_e);
            index_s = (this->str_t).indexOf(",", index_e + 2);
            int length = (this->str_t).substring(index_e + 2, index_s).toInt();
            this->message = (this->str_t).substring(index_s + 1, (this->str_t).length());
            return MQTT_RECV_TOPIC;
        }
        else
        {
            Serial.println("ESP_IDLE:" + this->str_t);
            return ESP_IDLE;
        }
    }
    else
    {
        return ESP_IDLE;
    }
}

/************************************************************************
 * @brief  获取消息.
 * @param  none
 * @return true:成功；false:失败.
 **********************************************************************/
String ESPWIFI::getMessage()
{
    return this->message;
}

/************************************************************************
 * @brief  接收到的订阅消息主题名.
 * @param  none
 * @return true:成功；false:失败.
 **********************************************************************/
String ESPWIFI::getTopicName()
{
    return this->topicName;
}

/************************************************************************
 * @brief  串口读写函数.
 * @param  none
 * @return true,成功; false,失败.
 **********************************************************************/
int ESPWIFI::available(void)
{
    return this->_serial->available();
}

int ESPWIFI::peek(void)
{
    return this->_serial->peek();
}

int ESPWIFI::read(void)
{
    return this->_serial->read();
}

void ESPWIFI::flush(void)
{
    this->_serial->flush();
}

size_t ESPWIFI::write(uint8_t c)
{
    return this->_serial->write(c);
}

//========================================================================
// 描述: 串口写数据，最后带回车与换行.
// 参数: str:数据.
// 返回: none.
//========================================================================
void ESPWIFI::writeData(String str)
{
    // taskENTER_CRITICAL();
    Serial2.println(str);
    // taskEXIT_CRITICAL();
}

/************************************************************************
 * @brief  读取数据,如果接收到数据或者超时则返回.
 * @param  timeout 超时时间(ms)
 * @return 读取到的字符串.
 **********************************************************************/
String ESPWIFI::readData(uint32_t timeout)
{
    str_t = "";
    uint32_t _startMillis = millis();
    do
    {
        if (this->_serial->available() > 0)
        {
            _startMillis = millis();
            str_t += (char)this->_serial->read();
        }
        else
        {
            delay(2);
        }
    } while ((millis() - _startMillis) < timeout);
    return str_t;
}

/************************************************************************
 * @brief  读取数据,如果接收到数据且包含字符串s则返回或者超时返回.
 * @param  timeout 超时时间(ms)
 * @param  s       结尾字符串
 * @return 读取到的字符串,如果错误则返回空字符串.
 **********************************************************************/
String ESPWIFI::readData(uint32_t timeout, String s)
{
    str_t = "";
    uint32_t _startMillis = millis();
    do
    {
        if (this->_serial->available() > 0)
        {
            _startMillis = millis();
            str_t += (char)this->_serial->read();
            if (str_t.indexOf(s) > 0)
            {
                return str_t;
            }
        }
        else
        {
            delay(2);
        }
    } while ((millis() - _startMillis) < timeout);
    return "";
}

/************************************************************************
 * @brief  检查配网信息.
 * @param  none
 * @return int.
 **********************************************************************/
uint8_t ESPWIFI::checkConfigMessage(void)
{
    String str = "";
    if (this->_serial->available() > 0)
    {
        str = this->_serial->readString();
    }
    if ((str) != "")
    {
        Serial.println("------------message------------");
        Serial.println(str);
        Serial.println("-------------------------------");
        if (str.indexOf("type:AIRKISS") > 0)
        {
            return WIFI_AIRKISSING;
        }

        if (str.indexOf("smartconfig connected wifi") > 0)
        {
            return WIFI_SMARTCONFIG_COMPLETE;
        }
        if (str.indexOf("WEBSERVERRSP:1") > 0)
        {
            if (str.indexOf("WEBSERVERRSP:2") > 0)
            {
                Serial.println("------2--------");
                return WIFI_WEBCONFIG_COMPLETE;
            }
            else
            {
                Serial.println("------1--------");
                return WIFI_WEBCONFIG_EXECUTEING;
            }
        }
        if (str.indexOf("WEBSERVERRSP:2") > 0)
        {
            Serial.println("------2--------");
            return WIFI_WEBCONFIG_COMPLETE;
        }

        if (str.indexOf("WIFI CONNECTED") > 0)
        {
            return WIFI_CONNECT;
        }
    }
    else
    {
        return ESP_IDLE;
    }
}

/************************************************************************
 * @brief  检查命令并返回字符串.
 * @param  cmd_s 需要执行的AT命令
 * @return 读取到的字符串,如果错误则返回空字符串.
 **********************************************************************/
String ESPWIFI::checkCmdAndGetString(String cmd_s)
{
    String s = "";
    // this->_serial->flush();
    this->writeData(cmd_s);
    s = readData(300);
    return s;
}

/************************************************************************
 * @brief  检查命令是否成功.
 * @param  cmd_s 需要执行的AT命令
 * @return true,成功; false,失败.
 **********************************************************************/
bool ESPWIFI::checkCmdIsOK(String cmd_s)
{
    String str = this->checkCmdAndGetString(cmd_s);
    // Serial.println("------------------------");
    // Serial.println("readData:" + str);
    // Serial.println("------------------------");
    // if ((str.indexOf("ERROR") > 0 || str.indexOf("OK") < 0))
    // {
    //     return false;
    // }
    // return true;
    return !(str.indexOf("ERROR") > 0 || str.indexOf("OK") < 0);
}

/************************************************************************
 * @brief  关闭智能配网并释放资源.
 * @param  none
 * @return 模式.
 **********************************************************************/
bool ESPWIFI::closeSmartConfig(void)
{
    return this->checkCmdIsOK("AT+CWSTOPSMART");
}

/************************************************************************
 * @brief  关闭WEB配网并释放资源.
 * @param  none
 * @return 模式.
 **********************************************************************/
bool ESPWIFI::closeWebConfig(void)
{
    return this->checkCmdIsOK("AT+WEBSERVER=0");
}
/************************************************************************
 * @brief  开启微信配网.
 * @return true,成功; false,失败.
 **********************************************************************/
bool ESPWIFI::openSmartConfig(void)
{
    char mode = this->checkMode();
    if (mode != WIFI_MODE_STATION)
    {
        if (!this->setMode(WIFI_MODE_STATION))
        {
            return false;
        }
    }
    // 先停止配网
    if (!this->checkCmdIsOK("AT+CWSTOPSMART"))
    {
        return false;
    }

    if (!this->checkCmdIsOK("AT+CWSTARTSMART=3"))
    {
        return false;
    }

    return true;
}

/************************************************************************
 * @brief  开启web配网.
 * @return true,成功; false,失败.
 **********************************************************************/
bool ESPWIFI::openWebConfig(void)
{
    char mode = this->checkMode();

    if (mode != WIFI_MODE_BOTH)
    {
        if (!this->setMode(WIFI_MODE_BOTH))
        {
            return false;
        }
    }
    return this->checkCmdIsOK("AT+WEBSERVER=1,80,50");
}

#endif
