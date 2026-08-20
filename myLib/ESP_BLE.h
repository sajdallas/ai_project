/**
 * @file    ESP_BLE.h
 * @brief   AT指令驱动esp32的BLE头文件
 *              该版本暂只支持作为服务器端,
 *              不支持自定义添加属性和特征，如果需要自定义请自行查阅esp32的相关文档。
 * @version 0.1
 * @date    2022-11-25
 * @author  hhdd
 * @copyright Copyright (c) 2021 TWen51 Technology Co., Ltd.
 */
#ifndef _ESP_BLE_H_
#define _ESP_BLE_H_

#include "asr.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "HardwareSerial.h"

#define	ESPBLE_IDLE			    0   //模块处于空闲模式
#define BLE_CONNECTED	        1	//有客户端连接上
#define BLE_NEW_MESSAGE	        2	//接收到一帧数据
#define	BLE_NOTIFY_OPEN			3   //NOTIFY开启
#define BLE_NOTIFY_CLOSE	    4	//NOTIFY关闭
#define BLE_INDICATE_OPEN	    5	//INDICATE开启
#define BLE_INDICATE_CLOSE	    6	//INDICATE关闭

#define  ESPBLE_CANCE              '0'
#define  ESPBLE_CLIENT             '1'
#define  ESPBLE_SERVER             '2'

class ESPBLE : public Stream
{
public:
	ESPBLE(HardwareSerial *serialx);
	void begin();  	//初始化

    /* 基础AT命令 */
	bool check();  	//确认esp设备是否存在
    bool reset();   //复位
	bool restore();	//恢复出厂设置
	String getGMR();  //查询版本信息
	
    /* BLE命令 */
    bool setMode(char mode);	//设置BLE模式
    bool set_name(String n);    //设置设备名称
    bool set_adv_data(String n); //设置广播数据
    bool adv_start(); //开始广播
    bool adv_stop(); //停止广播
    bool GATTS_start(); //创建并开启服务
    bool GATTS_set_ATTR(uint8_t srv_index, uint8_t char_index, String str);  //设置特征值
    bool GATTS_set_ATTRdesc(uint8_t srv_index, uint8_t char_index, uint8_t desc_index, String str); //设置特征描述符值
    bool GATTS_notify(String str); //发送notify值
    bool GATTS_indicate(String str); //发送indicate值

    /* HID命令 */
    bool HIDInit();  	//HID初始化
    bool HIDSendMus(int buttons, int x, int y, int wheel);  	//HID发送鼠标信息
    bool HIDSendKB(uint8_t Modifier_keys, uint8_t key1, uint8_t key2, uint8_t key3, uint8_t key4, uint8_t key5, uint8_t key6);  	//HID发送键盘信息

    /* 获取消息 */
    int checkMessage();     //获取消息并返回消息类型
	String getMessage();	//获取消息值
	uint8_t getServerID();	//获取服务序号
	uint8_t getCharID();	//获取特征序号
	String getClientAddr();	//获取客户端设备地址

    /* 进入SPP透传模式 */
    bool setSPPMode(uint8_t tx_service_index, uint8_t tx_char_index, uint8_t rx_service_index, uint8_t rx_char_index);
    bool exitSPPMode();

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

	uint8_t ServerID;  //保存服务号id
	uint8_t CharID;  //保存特征id
	String message;		//接收到的消息数据
	String client_addr;		//客户端地址

	String str_t;	//临时保存串口接收到的数据
};

ESPBLE::ESPBLE(HardwareSerial *serialx) 
{
    _serial = serialx;
}

/************************************************************************
  * @brief  初始化.
  * @param  serialx 串口.
  * @return none
  **********************************************************************/
void ESPBLE::begin()
{ 
    this->_serial->begin(115200);
}

/************************************************************************
  * @brief  确认esp模块是否存在.
  * @param  none
  * @return ture,检测到ESP模块; false,没有检测到模块
  **********************************************************************/
bool ESPBLE::check() 
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
bool ESPBLE::reset() 
{
    str_t = "";
	this->_serial->flush();
	writeData("AT+RST");
    str_t = readData(300);
    if(str_t.indexOf("OK") > 0){
        return true;
    }
	return false;	
}

/************************************************************************
  * @brief  恢复出厂设置并复位.
  * @param  none
  * @return ture,复位成功; false,复位失败
  **********************************************************************/
bool ESPBLE::restore() 
{
    str_t = "";
	this->_serial->flush();
	writeData("AT+RESTORE");
    if(readData(1000, "OK\r\n") != ""){
        delay(5000); //等待重启完成
        return true;
    }
    delay(5000); //等待重启完成
    return false;
}

/************************************************************************
  * @brief  设置BLE初始化角色.
  * @param  mode    ESPBLE_CANCE       注销
  *                 ESPBLE_CLIENT      客户端角色
  *                 ESPBLE_SERVER      服务器角色
  * @return ture,复位成功; false,复位失败
  **********************************************************************/
bool ESPBLE::setMode(char mode)
{
    str_t = "";
	this->_serial->flush();
	writeData("AT+BLEINIT="+String(mode));
    if(readData(500, "OK\r\n") != ""){
        return true;
    }
    return false;
}

/************************************************************************
  * @brief  设置ble设备名称.
  * @param  n - 设备名称（最大长度32字节）
  * @return ture,复位成功; false,复位失败
  **********************************************************************/
bool ESPBLE::set_name(String n)
{
	this->_serial->flush();
	writeData("AT+BLENAME=\"" + n + "\"");
    if(readData(300, "OK\r\n") != ""){
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
bool ESPBLE::set_adv_data(String n)
{
    this->_serial->flush();
    writeData("AT+BLEADVDATA=\"" + n + "\"");
    if(readData(300, "OK\r\n") != ""){
        return true;
    }
    return false;
}

/************************************************************************
  * @brief  开始ble广播.
  * @param  none
  * @return ture,复位成功; false,复位失败
  **********************************************************************/
bool ESPBLE::adv_start()
{
    this->_serial->flush();
    writeData("AT+BLEADVSTART");
    if(readData(300, "OK\r\n") != ""){
        return true;
    }
    return false;
}

/************************************************************************
  * @brief  停止ble广播.
  * @param  none
  * @return ture,复位成功; false,复位失败
  **********************************************************************/
bool ESPBLE::adv_stop()
{
    this->_serial->flush();
    writeData("AT+BLEADVSTOP");
    if(readData(300, "OK\r\n") != ""){
        return true;
    }
    return false;
}

/************************************************************************
  * @brief  GATTS创建并开启服务.
  * @param  none
  * @return ture,复位成功; false,复位失败
  **********************************************************************/
bool ESPBLE::GATTS_start()
{
    this->_serial->flush();
    writeData("AT+BLEGATTSSRVCRE");
    if(readData(300, "OK\r\n") == ""){
        return false;
    }
    this->_serial->flush();
    writeData("AT+BLEGATTSSRVSTART");
    if(readData(300, "OK\r\n") != ""){
        return true;
    }
    return false;
}

/************************************************************************
  * @brief  设置特征值.
  * @param  srv_index    服务序号
  * @param  char_index   服务特征的序号
  * @param  str          数据
  * @return ture,成功; false,失败
  **********************************************************************/
bool ESPBLE::GATTS_set_ATTR(uint8_t srv_index, uint8_t char_index, String str)
{
    this->_serial->flush();
    writeData("AT+BLEGATTSSETATTR="+ String(srv_index) +","+ String(char_index) +",," + String(str.length()));
    if(readData(100, ">") == ""){
        return false;
    }
    this->_serial->flush();
    writeData(str);
    if(readData(300, "OK\r\n") != ""){
        return true;
    }
    return false;
}

/************************************************************************
  * @brief  设置描述符值.
  * @param  srv_index    服务序号
  * @param  char_index   服务特征的序号
  * @param  desc_index   特征描述符序号
  * @param  length       数据长度
  * @return ture,成功; false,失败
  **********************************************************************/
bool ESPBLE::GATTS_set_ATTRdesc(uint8_t srv_index, uint8_t char_index, uint8_t desc_index, String str)
{
    this->_serial->flush();
    writeData("AT+BLEGATTSSETATTR="+ String(srv_index) +","+ String(char_index) +","+ String(desc_index)+"," + String(str.length()));
    if(readData(300, ">") == ""){
        return false;
    }
    this->_serial->flush();
    writeData(str);
    if(readData(300, "OK\r\n") != ""){
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
bool ESPBLE::GATTS_notify(String str)
{
    this->_serial->flush();
    writeData("AT+BLEGATTSNTFY=0,1,6," + String(str.length()));
    if(readData(500, ">") == ""){
        return false;
    }
    this->_serial->flush();
    writeData(str);
    if(readData(500, "OK") != ""){
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
bool ESPBLE::GATTS_indicate(String str)
{
    this->_serial->flush();
    writeData("AT+BLEGATTSIND=0,1,7," + String(str.length()));
    if(readData(500, ">") == ""){
        return false;
    }
    this->_serial->flush();
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
  * 	参数说明	 <conn_index>:连接号，范围0-2.
  * 			    <srv_index>:服务序号
  * 				<char_index>:服务特征的序号
  * 				<>:
  * 				<length>: 数据长度
  **********************************************************************/
int ESPBLE::checkMessage() 
{
	str_t = "";
    if(this->_serial->available() > 0){
        str_t = this->_serial->readString();
    }
	if (str_t != "") {
		if (str_t.substring(0, 9) == "+BLECONN:") {   //有客户端连接
            int index_s = str_t.indexOf("\"");
            int index_e = str_t.indexOf("\"", index_s+1);
			this->client_addr = str_t.substring(index_s+1, index_e);
            return BLE_CONNECTED;

		} else if (str_t.substring(0,6) == "+WRITE") {    //服务端接收到客户端发送的数据
            this->ServerID = str_t.substring(9, 10).toInt();
			this->CharID = str_t.substring(11, 12).toInt();
            this->message = str_t.substring(str_t.indexOf(",", 15)+1, str_t.length()-2);  
            if((this->ServerID == 1)&&(this->CharID == 6))
            {
                if(((uint8_t)this->message[0]==1)&&((uint8_t)this->message[1]==0)){
                    return BLE_NOTIFY_OPEN;
                }else if(((uint8_t)this->message[0]==0)&&((uint8_t)this->message[1]==0)){
                    return BLE_NOTIFY_CLOSE;
                }

            }else if((this->ServerID == 1)&&(this->CharID == 7))
            {
                if(((uint8_t)this->message[0]==2)&&((uint8_t)this->message[1]==0)){
                    return BLE_INDICATE_OPEN;
                }else if(((uint8_t)this->message[0]==0)&&((uint8_t)this->message[1]==0)){
                    return BLE_INDICATE_CLOSE;
                }
            }
            return BLE_NEW_MESSAGE;
		
        }else if (this->str_t.substring(0,11) == "+BLEHIDCONN"){   //HID客户端连接上
            return  BLE_CONNECTED;
        
        }else {
			return ESPBLE_IDLE;
		}
	}
	return ESPBLE_IDLE;
}

/************************************************************************
  * @brief  获取客户端地址.
  * @param  none
  * @return 客户端地址字符串.
  **********************************************************************/
String ESPBLE::getClientAddr()
{
	return this->client_addr;
}

/************************************************************************
  * @brief  获取消息.
  * @param  none
  * @return 客户端地址字符串.
  **********************************************************************/
String ESPBLE::getMessage()
{
	return this->message;
}

/************************************************************************
  * @brief  获取服务序号.
  * @param  none
  * @return 客户端地址字符串.
  **********************************************************************/
uint8_t ESPBLE::getServerID()
{
	return this->ServerID;
}

/************************************************************************
  * @brief  获取特征序号.
  * @param  none
  * @return 客户端地址字符串.
  **********************************************************************/
uint8_t ESPBLE::getCharID()
{
	return this->CharID;
}

/************************************************************************
  * @brief  进入SPP模式.
  * @param  tx_service_index    tx服务序号
  * @param  tx_char_index       tx服务特征序号
  * @param  rx_service_index    rx服务序号
  * @param  rx_char_index       rx服务特征序号
  * @return true,成功; false,失败.
  **********************************************************************/
bool ESPBLE::setSPPMode(uint8_t tx_service_index, uint8_t tx_char_index, uint8_t rx_service_index, uint8_t rx_char_index) 
{
    //重置SPP参数
    this->_serial->flush();
	writeData("AT+BLESPPCFG=0");

    if(readData(500, "OK\r\n") == ""){
        return false;
    }    
    //设置SPP参数
    this->_serial->flush();
    writeData("AT+BLESPPCFG=1,"+String(tx_service_index)+","+String(tx_char_index)+","+String(rx_service_index)+","+String(rx_char_index));    //设置SPP参数
    if(readData(500, "OK\r\n") == ""){
        return false;
    }       
    //进入SPP模式
    this->_serial->flush();
    writeData("AT+BLESPP");  
    if(readData(300, ">") != ""){
        return true;
    }
	return false;
}

/************************************************************************
  * @brief  退出SPP模式.
  * @param  noen
  * @return true,成功; false,失败.
  **********************************************************************/
bool ESPBLE::exitSPPMode() 
{
    this->_serial->print("+++");
	delay(200);
	return check();
}


/************************************************************************
  * @brief  HID初始化.
  * @param  noen
  * @return true,成功; false,失败.
  **********************************************************************/
bool ESPBLE::HIDInit()
{
    this->_serial->flush();
    writeData("AT+BLEHIDINIT=1");
    if(readData(300, "OK\r\n") != ""){
        return true;
    }
    return false;
}

/************************************************************************
  * @brief  HID发送鼠标信息.
  * @param  buttons     鼠标按键
  * @param  x           X位移
  * @param  y           Y位移
  * @param  wheel       滚轮
  * @return true,成功; false,失败.
  **********************************************************************/
bool ESPBLE::HIDSendMus(int buttons, int x, int y, int wheel)
{
    this->_serial->flush();
    writeData("AT+BLEHIDMUS="+ String(buttons)+","+String(x)+","+String(y)+","+String(wheel));
    if(readData(300, "OK\r\n") != ""){
        return true;
    }
    return false;
}

/************************************************************************
  * @brief  HID发送键盘信息.
  * @param  buttons     鼠标按键
  * @param  x           X位移
  * @param  y           Y位移
  * @param  wheel       滚轮
  * @return true,成功; false,失败.
  **********************************************************************/
bool ESPBLE::HIDSendKB(uint8_t Modifier_keys, uint8_t key1, uint8_t key2, uint8_t key3, uint8_t key4, uint8_t key5, uint8_t key6)
{
    this->_serial->flush();
    writeData("AT+BLEHIDKB="+ String(Modifier_keys)+","+String(key1)+","+String(key2)+","+String(key3)+String(key4)+","+String(key5)+","+String(key6));
    if(readData(300, "OK\r\n") != ""){
        return true;
    }
    return false;
}

/************************************************************************
  * @brief  串口读写函数.
  * @param  none
  * @return true,成功; false,失败.
  **********************************************************************/
int ESPBLE::available(void)
{
	return this->_serial->available();
}

int ESPBLE::peek(void)
{
	return this->_serial->peek();
}

int ESPBLE::read(void)
{
	return this->_serial->read();
}

void ESPBLE::flush(void)
{
	this->_serial->flush();
}

size_t ESPBLE::write(uint8_t c)
{
	return this->_serial->write(c);
}

//========================================================================
// 描述: 串口写数据，最后带回车与换行.
// 参数: str:数据.
// 返回: none.
//========================================================================
void ESPBLE::writeData(String str) 
{
    // taskENTER_CRITICAL();
    this->_serial->println(str);
    // taskEXIT_CRITICAL();
}

/************************************************************************
  * @brief  读取数据,如果接收到数据或者超时则返回.
  * @param  timeout 超时时间(ms)
  * @return 读取到的字符串.
  **********************************************************************/
String ESPBLE::readData(uint32_t timeout) 
{
    str_t = "";
    uint32_t _startMillis = millis();
    do{
        if(this->_serial->available() > 0){
            _startMillis = millis();
            str_t += (char)this->_serial->read();
        }else{
            delay(2);
        }   
    }while( (millis()-_startMillis) < timeout );
    return str_t;
}

/************************************************************************
  * @brief  读取数据,如果接收到数据且包含字符串s时则返回或者超时返回.
  * @param  timeout 超时时间(ms)
  * @param  s       结尾字符串
  * @return 读取到的字符串,如果错误则返回空字符串.
  **********************************************************************/
String ESPBLE::readData(uint32_t timeout, String s) 
{
    str_t = "";
    uint32_t _startMillis = millis();
    do{
        if(this->_serial->available() > 0){
            _startMillis = millis();
            str_t += (char)this->_serial->read();
            if(str_t.indexOf(s) > 0){
                return str_t;
            }
        }else{
            delay(2);
        }
    }while( (millis() - _startMillis) < timeout );
    return "";
}

#endif
