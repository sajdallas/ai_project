/********************************************************************************
* @File name: asr_dhtxx.h
* @Author: hhdd
* @Version: 1.1
* @Date: 2022-6-21
* @Description: DHTxx驱动库。
********************************************************************************/
#ifndef _ASR_DHTxx_H_
#define _ASR_DHTxx_H_

#include "asr.h"
#ifndef TW_ASR_PRO
#include "asr_delay.h"
#endif

#define  DHTxx_Debug            0
/*********************************************************
 * PRO：DHT11串口打印读出的counter值如下
 * 0-47-93-57-25-57-26-57-75-57-75-57-25-58-74-57-75-58-75-58-25-57-26-57-25-58-25-57-26-
 * 57-25-57-26-57-27-57-25-57-26-57-26-57-75-57-75-57-75-57-25-58-75-58-25-57-26-57-25-57-
 * 26-57-26-57-75-57-25-57-76-58-25-57-75-57-26-57-75-57-75-57-25-57-26-57-76-57-255
 * dht11的0码值：57-26
 * dht11的1码值：57-75
 *********************************************************/
/*********************************************************
 * ONE：DHT11串口打印读出的counter值如下
 * 0-30-57-34-16-35-15-35-46-34-46-35-45-35-46-34-46-35-46-35-15-35-16-35-15-35-15-35-16-34-
 * 16-35-15-35-16-35-16-34-16-35-15-35-46-34-46-35-45-35-46-35-16-35-15-35-15-35-16-35-15-35-
 * 15-35-46-35-15-35-16-35-15-35-46-35-45-35-16-34-16-35-15-35-16-34-46-35-165
 * dht11的0码值：35-15
 * dht11的1码值：35-46
 *********************************************************/
#if defined(TW_ASR_PRO)
#define COUNT 45  //"1"码和"0"码区分的阈值
#else
#define COUNT 30  //"1"码和"0"码区分的阈值
#endif

#define MAXTIMINGS 85   //数据长度

#define DHT11 11
#define DHT22 22
#define DHT21 21
#define AM2301 21

class DHTxx
{
public:
    uint8_t data[6];
    DHTxx(uint8_t pin);
    DHTxx(uint8_t pin, uint8_t type, uint8_t count=COUNT);  
    void init(void);
    float read_temperature(bool S=false);  //读取温度函数
    float convertCtoF(float); //摄氏度转华氏度
    float readHumidity(void); //读取湿度
private:
    uint8_t _pin, _type, _count;
    unsigned long _lastreadtime;
    bool firstreading;
    bool read(void);
};

//========================================================================
// 描述: 初始化函数.
// 参数: pin：引脚.
// 返回: none.
//========================================================================
DHTxx::DHTxx(uint8_t pin)
{
    _pin = pin;
    _type = DHT11;
    _count = COUNT;
    firstreading = true;
    _lastreadtime = 0;
}

//========================================================================
// 描述: 初始化函数.
// 参数: pin：引脚; type:器件类型; count:"1"码和"0"码区分的阈值.
// 返回: none.
//========================================================================
DHTxx::DHTxx(uint8_t pin, uint8_t type, uint8_t count)
{
    _pin = pin;
    _type = type;
    _count = count;
    firstreading = true;
    _lastreadtime = 0;
}

//========================================================================
// 描述: 引脚初始化函数.
// 参数: none.
// 返回: none.
//========================================================================
void DHTxx::init(void)
{
    // set up the pins!
    pinMode(_pin,input);
#if defined(TW_ASR_PRO)
    set_pin_to_gpio_mode(_pin);
#else
    if((_pin<=8) && (_pin >=5))
    {
        setPinFun(_pin,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(_pin,FIRST_FUNCTION);
    }
#endif
    digitalWrite(_pin,1);
}

//========================================================================
// 描述: 读取温度.
// 参数: S:True,华氏度,False,摄氏度.
// 返回: none.
//========================================================================
float DHTxx::read_temperature(bool S) {
    float f;
    if (read()) 
    {
      switch (_type) 
      {
        case DHT11:
          f = data[2];
          if(S){
            f = convertCtoF(f);
          }
          return f;
        case DHT22:
        case DHT21:
          f = data[2] & 0x7F;
          f *= 256;
          f += data[3];
          f /= 10;
          if (data[2] & 0x80){
            f *= -1;
          }
          if(S){
            f = convertCtoF(f);
          }
          return f;
      }
    }
    return NAN;
}

//========================================================================
// 描述: 摄氏度转华氏度.
// 参数: c:摄氏度.
// 返回: 华氏度.
//========================================================================
float DHTxx::convertCtoF(float c) {
    return c * 9 / 5 + 32;
}

//========================================================================
// 描述: 读取湿度.
// 参数: none.
// 返回: 湿度值.
//========================================================================
float DHTxx::readHumidity(void) {
    float f;
    if(read())
    {
      switch (_type) {
        case DHT11:
          f = data[0];
          return f;
        case DHT22:
        case DHT21:
          f = data[0];
          f *= 256;
          f += data[1];
          f /= 10;
          return f;
      }
    }
    return NAN;
}

//========================================================================
// 描述: 读取原始数据（内部调用）.
// 参数: none.
// 返回: 湿度值.
//========================================================================
bool DHTxx::read(void) {
    uint8_t laststate = 1;
    uint8_t counter = 0;
    uint8_t j = 0, i;
    unsigned long currenttime;

    digitalWrite(_pin, 1);
    delayMicroseconds(40);

    currenttime = millis();
    if (currenttime < _lastreadtime) {
        _lastreadtime = 0;
    }
    if (!firstreading && ((currenttime - _lastreadtime) < 2000)) 
    {
        return true;  //使用上次读取的值
    }
    firstreading = false;
    _lastreadtime = millis();

    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    // now pull it low for ~20 milliseconds
    pinMode(_pin,output);
    digitalWrite(_pin, 0);
    if(_type == DHT11){
        delay(20);
    }else{
        delayMicroseconds(1100);
    }
    
    taskENTER_CRITICAL();   //进入临界区
    digitalWrite(_pin, 1);
    delayMicroseconds(40);
    pinMode(_pin,input);

#if(DHTxx_Debug==1)
    uint8_t testArray[MAXTIMINGS];
#endif 
    // read in timings
    for ( i = 0; i< MAXTIMINGS; i++) {
        counter = 0;
        while (digitalRead(_pin) == laststate) {
            counter++;
            delay1us();
            if (counter == 255) {
                break;
            }
        }
        laststate = digitalRead(_pin);
        if (counter == 255) break;
#if(DHTxx_Debug==1)
        testArray[i] = counter;
#endif
        //忽略前3个转换
        if ((i >= 4) && (i%2 == 0)) {
            // shove each bit into the storage bytes
            data[j/8] <<= 1;
            if (counter > _count)
                data[j/8] |= 1;
            j++;
        }
    }
#if(DHTxx_Debug==1)
    for ( i = 0; i< MAXTIMINGS; i++) {
        Serial.print(String(testArray[i]) + '-');
    }
    Serial.println(" ");
#endif

    //如果数据大于等于40个且校验成功
    if ((j >= 40) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) ) {
        taskEXIT_CRITICAL();    //退出临界区
        return true;
    }
    taskEXIT_CRITICAL();    //退出临界区
    return false;
}
#endif
