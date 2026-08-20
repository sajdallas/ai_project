/**
 * @file    ESP_BLUE.h
 * @brief   AT指令驱动esp32的blue头文件
 *              该版本暂只支持作为服务器端,
 *              不支持自定义添加属性和特征，如果需要自定义请自行查阅esp32的相关文档。
 * @version 0.1
 * @date    2022-11-25
 * @author  hhdd
 * @copyright Copyright (c) 2021 TWen51 Technology Co., Ltd.
 */
#ifndef _ESP_BLUE_H_
#define _ESP_BLUE_H_

#include "asr.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "HardwareSerial.h"

#define	ESPBLUE_IDLE			    0   //模块处于空闲模式
#define BLUE_CONNECTED	            1	//有客户端连接上
#define BLUE_NEW_MESSAGE	        2	//接收到一帧数据

#define  ESPBLUE_CANCE       '0'
#define  ESPBLUE_CLIENT      '1'
#define  ESPBLUE_SERVER      '2'

class ESPBLUE : public Stream
{
public:
	ESPBLUE(HardwareSerial *serialx);
	void begin();  	//初始化

    /* 基础AT命令 */
	bool check();  	//确认esp设备是否存在
    bool reset();   //复位
	bool restore();	//恢复出厂设置
	String getGMR();  //查询版本信息
	
    /* BLUE命令 */
    bool setMode(char mode);	//设置BLE模式
    bool set_name(String n);    //设置设备名称
    bool set_adv_data(String n); //设置广播数据
    bool adv_start(); //开始广播
    bool adv_stop(); //停止广播

    bool GATTS_start(); //创建并开启服务

    bool GATTS_notify(String str); //发送notify值
    bool GATTS_indicate(String str); //发送indicate值

    // String get_GATTS();    //发现服务特征

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


    int checkMessage();     //获取消息并返回消息类型
	
	String getMessage();	//获取消息值
	uint8_t getServerID();	//获取服务序号
	uint8_t getCharID();	//获取客户端序号
	String getClientAddr();	//获取客户端地址

    /* 串口读写数据 */
	void writeData(String str);
	String readData(uint32_t timeout);
    String readData(uint32_t timeout, String s);
private:
	HardwareSerial *_serial;

	uint8_t ServerID;  //保存服务号id
	uint8_t CharID;  //保存特征id
	String message;		//接收到的消息数据
	String client_addr;		//客户端地址



	volatile bool isPureDataMode; //是否开启了透传模式
};

ESPBLUE::ESPBLUE(HardwareSerial *serialx) 
{
    _serial = serialx;
    this->isPureDataMode = false;
}

/************************************************************************
  * @brief  初始化.
  * @param  serialx 串口.
  * @return none
  **********************************************************************/
void ESPBLUE::begin()
{ 
    this->_serial->begin(115200);
    this->_serial->setTimeout(1000);
}

/************************************************************************
  * @brief  确认esp模块是否存在.
  * @param  none
  * @return ture,检测到ESP模块; false,没有检测到模块
  **********************************************************************/
bool ESPBLUE::check() 
{
	this->_serial->flush(); //清空缓存区数据
	writeData("AT");
    if(readData(200, "OK\r\n") != ""){
        return true;
    }
    return false;
}


/************************************************************************
  * @brief  复位.
  * @param  none
  * @return ture,复位成功; false,复位失败
  **********************************************************************/
bool ESPBLUE::reset() 
{
	this->_serial->flush();
	writeData("AT+RST");
    String str = readData(100);
    if(str.indexOf("OK") > 0){
        return true;
    }
	return false;	
}

/************************************************************************
  * @brief  恢复出厂设置并复位.
  * @param  none
  * @return ture,复位成功; false,复位失败
  **********************************************************************/
bool ESPBLUE::restore() 
{
	this->_serial->flush();
	writeData("AT+RESTORE");
    String str = readData(100);
    if(str.indexOf("OK") > 0){
        delay(5000); //等待重启完成
        return true;
    }
	return false;
}

/************************************************************************
  * @brief  设置BLE初始化角色.
  * @param  mode    ESPBLUE_CANCE       注销Bluetooth LE
  *                 ESPBLUE_CLIENT      客户端角色
  *                 ESPBLUE_SERVER      服务器角色
  * @return ture,复位成功; false,复位失败
  **********************************************************************/
bool ESPBLUE::setMode(char mode)
{
	this->_serial->flush();
	writeData("AT+BLEINIT="+String(mode));
    String str = readData(100);
    if(str.indexOf("OK") > 0){
        return true;
    }
	return false;
}

/************************************************************************
  * @brief  设置ble设备名称.
  * @param  n - 设备名称（最大长度32字节）
  * @return ture,复位成功; false,复位失败
  **********************************************************************/
bool ESPBLUE::set_name(String n)
{
	this->_serial->flush();
	writeData("AT+BLENAME=\"" + n + "\"");
    if(readData(100, "OK\r\n") != ""){
        return true;
    }
    return false;
}

/************************************ 服务端 ******************************************/
/************************************************************************
  * @brief  设置ble广播数据
  * @param  n - 设备名称（最大长度32字节）
  * @return none
  * 
  * 广播包数据格式:
  *     数据长度(1字节) + 广播数据类型(1字节) + 数据内容
  * 
  * 类型(举几个常用的)：
  *     0x01 - 广播模式     
  *     0x02 - 非完整的16bit UUID列表
  *     0x09 - 完整的设备名称
  * 
  * 例如:
  *     02 01 06   //广播模式
  *     0A 09 41 69 54 68 69 6E 6B 65 72  //完整的设备名称 AiThinker
  **********************************************************************/
bool ESPBLUE::set_adv_data(String n)
{
    writeData("AT+BLEADVDATA=\"" + n + "\"");
    if(readData(100, "OK\r\n") != ""){
        return true;
    }
    return false;
}

/************************************************************************
  * @brief  开始ble广播.
  * @param  none
  * @return ture,复位成功; false,复位失败
  **********************************************************************/
bool ESPBLUE::adv_start()
{
    writeData("AT+BLEADVSTART");
    if(readData(100, "OK\r\n") != ""){
        return true;
    }
    return false;
}

/************************************************************************
  * @brief  停止ble广播.
  * @param  none
  * @return ture,复位成功; false,复位失败
  **********************************************************************/
bool ESPBLUE::adv_stop()
{
    writeData("AT+BLEADVSTOP");
    if(readData(100, "OK\r\n") != ""){
        return true;
    }
    return false;
}

/************************************************************************
  * @brief  GATTS创建并开启服务.
  * @param  none
  * @return ture,复位成功; false,复位失败
  **********************************************************************/
bool ESPBLUE::GATTS_start()
{
    writeData("AT+BLEGATTSSRVCRE");
    if(readData(100, "OK\r\n") == ""){
        return false;
    }
    writeData("AT+BLEGATTSSRVSTART");
    if(readData(100, "OK\r\n") != ""){
        return true;
    }
    return false;
}

/************************************************************************
  * @brief  发送notify服务特征值给客户端(通知，不需要客户端应答).
  * @param  str         需要发送的字符串
  * @return ture,成功; false,失败
  * 		 			
  * 在ESP32的默认固件中, NOTIFY连接号默认为2, 服务序号为1, 服务特征序号为6.
  **********************************************************************/
bool ESPBLUE::GATTS_notify(String str)
{
    writeData("AT+BLEGATTSNTFY=0,1,6" + String(str.length()));
    if(readData(100, ">") == ""){
        return false;
    }
    writeData(str);
    if(readData(300, "OK\r\n") != ""){
        return true;
    }
    return false;
}

/************************************************************************
  * @brief  发送indicate服务特征值给客户端(指示，需要客户端应答).
  * @param  str         需要发送的字符串
  * @return ture,成功; false,失败
  * 
  * 在ESP32的默认固件中, INDICATE连接号默认为2, 服务序号为1, 服务特征序号为7.
  **********************************************************************/
bool ESPBLUE::GATTS_indicate(String str)
{
    writeData("AT+BLEGATTSIND=0,1,7" + String(str.length()));
    if(readData(100, ">") == ""){
        return false;
    }
    writeData(str);
    if(readData(300, "OK\r\n") != ""){
        return true;
    }
    return false;
}


/************************************ 服务端end ******************************************/
/************************************************************************
  * @brief  获取消息并返回消息类型.
  * @param  none
  * @return .
  * 	客户端发消息或者开启订阅功能说明：
  * 	+Write:<conn_index>,<srv_index>,<char_index>,<>,<length>,
  * 	参数说明	 <conn_index>:Bluetooth LE连接号，范围0-2.
  * 			    <srv_index>:服务序号
  * 				<char_index>:服务特征的序号
  * 				<>:
  * 				<length>: 数据长度
  **********************************************************************/
int ESPBLUE::checkMessage() 
{
	String tmp = "";
    if(this->_serial->available() > 0){
        tmp = this->_serial->readString();
		Serial.print( tmp );
    }
	// if (tmp!="") {
	// 	if (tmp.substring(0, 9) == "+BLECONN:") {   //有客户端连接
    //         int index_s = tmp.indexOf("\"");
    //         int index_e = tmp.indexOf("\"", index_s+1);
	// 		this->client_addr = tmp.substring(index_s+1, index_e);
    //         return BLUE_CONNECTED;

	// 	} else if (tmp.substring(0,6) == "+WRITE") {    //服务端接收到客户端发送的数据
    //         this->ServerID = tmp.substring(9, 10).toInt();
	// 		this->CharID = tmp.substring(11, 12).toInt();
	// 		this->message = tmp.substring(tmp.indexOf(",", 15)+1, tmp.length()-2);    
    //         return BLUE_NEW_MESSAGE;
	// 	}
    //     else {
	// 		return ESPBLUE_IDLE;
	// 	}
	// }
	return ESPBLUE_IDLE;
}

/************************************************************************
  * @brief  获取客户端地址.
  * @param  none
  * @return 客户端地址字符串.
  **********************************************************************/
String ESPBLUE::getClientAddr()
{
	return this->client_addr;
}

/************************************************************************
  * @brief  获取消息.
  * @param  none
  * @return 客户端地址字符串.
  **********************************************************************/
String ESPBLUE::getMessage()
{
	return this->message;
}

/************************************************************************
  * @brief  获取服务序号.
  * @param  none
  * @return 客户端地址字符串.
  **********************************************************************/
uint8_t ESPBLUE::getServerID()
{
	return this->ServerID;
}

/************************************************************************
  * @brief  获取特征序号.
  * @param  none
  * @return 客户端地址字符串.
  **********************************************************************/
uint8_t ESPBLUE::getCharID()
{
	return this->CharID;
}

/************************************************************************
  * @brief  串口读写函数.
  * @param  none
  * @return true,成功; false,失败.
  **********************************************************************/
int ESPBLUE::available(void)
{
	return this->_serial->available();
}

int ESPBLUE::peek(void)
{
	return this->_serial->peek();
}

int ESPBLUE::read(void)
{
	return this->_serial->read();
}

void ESPBLUE::flush(void)
{
	this->_serial->flush();
}

size_t ESPBLUE::write(uint8_t c)
{
	return this->_serial->write(c);
}

//========================================================================
// 描述: 串口写数据，最后带回车与换行.
// 参数: str:数据.
// 返回: none.
//========================================================================
void ESPBLUE::writeData(String str) 
{
    // taskENTER_CRITICAL();
    Serial2.println(str);
    // taskEXIT_CRITICAL();
}

/************************************************************************
  * @brief  读取数据,如果接收到数据或者超时则返回.
  * @param  timeout 超时时间(6ms为1单位)
  * @return 读取到的字符串.
  **********************************************************************/
String ESPBLUE::readData(uint32_t timeout) 
{
    String str = "";
    do{
        if(this->_serial->available() > 0){
            str = this->_serial->readString();
            return str;
        }
        delay(6);
    }while(timeout--);
    return "";
}

/************************************************************************
  * @brief  读取数据,如果接收到数据且以字符串s结尾则返回或者超时返回.
  * @param  timeout 超时时间(6ms为1单位)
  * @param  s       结尾字符串
  * @return 读取到的字符串,如果错误则返回空字符串.
  **********************************************************************/
String ESPBLUE::readData(uint32_t timeout, String s) 
{
    String str = "";
    do{
        if(this->_serial->available() > 0){
            str =str + this->_serial->readString();
            if(str.substring(str.length() - s.length()) == s){
                return str;
            }
        }
        delay(6);
    }while(timeout--);
    return "";
}





#endif
