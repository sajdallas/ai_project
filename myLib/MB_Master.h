#ifndef _MB_Master_H
#define _MB_Master_H

#include "asr.h"
#include "HardwareSerial.h"
#include "CRC_16.h"

class MBMaster{
public:
    MBMaster(HardwareSerial *serialx, uint32_t baud, UART_Parity eParity);
    MBMaster(HardwareSerial *serialx, uint32_t baud, UART_Parity eParity,int8_t rs485_pin,uint8_t level);
    void Init();     //modbus主机初始化
    int8_t ReqReadCoils(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout); //读线圈 
    int8_t ReqWriteCoil(uint8_t MB_id, uint16_t addr,  uint8_t _data, uint16_t timeout); //写线圈
    int8_t ReqWriteCoil_S(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout); //写多个线圈
    int8_t ReqReadDiscreteInputs(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout); //读离散量输入
    int8_t ReqReadInputRegister(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout); //读输入寄存器
    int8_t ReqWriteHoldingRegister(uint8_t MB_id, uint16_t addr, uint16_t  _data, uint16_t timeout); //写单个保持寄存器
    int8_t ReqWriteHoldingRegister_S(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout); //写多个保持寄存器
    int8_t ReqReadHoldingRegister(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout); //读保持寄存器
    int8_t ReqReadWriteHoldingRegister_S(uint8_t MB_id, uint16_t raddr, uint16_t ruslen,uint16_t waddr, uint16_t wuslen,uint8_t *databuf, uint16_t timeout); //读写保持寄存器
    bool rev_process(uint32_t timeout); //串口接收处理函数

private:
    uint8_t mb_m_rec_temp[32]; //保存主站接收到的数据
    uint8_t mb_m_command[32];    //保存主站要发送的数据

    uint8_t mb_m_rec_len = 0;         //接收长度
    uint16_t mb_m_crc_temp = 0; // 16位CRC临时变量

    int8_t _rs485_pin = -1;
    uint8_t _level;

    HardwareSerial *_serial;
    UART_TypeDef *UARTx;
    uint32_t _baud;
    UART_Parity _eParity; //校验位
    uint32_t _timeout;

    void SerialInit(); //串口初始化
    void SerialPutByte(uint8_t ucByte); //串口发送字节
    void SerialPutBuff(uint8_t* p, uint16_t len);   //串口发送数组

    void RecDisable();  //关闭串口接收
    void RecEnble();    //使能串口接收
    void Send(uint8_t *p, uint16_t len);

};

MBMaster::MBMaster(HardwareSerial *serialx, uint32_t baud,  UART_Parity eParity)
{
    _serial = serialx;
    _baud = baud;
    _eParity = eParity;
    if(_serial == &Serial){
        UARTx = UART0;
    }else if(_serial == &Serial1){
        UARTx = UART1;
    }else if(_serial == &Serial2){
        UARTx = UART2;
    }
}

MBMaster::MBMaster(HardwareSerial *serialx, uint32_t baud,  UART_Parity eParity,int8_t rs485_pin,uint8_t level)
{
    _serial = serialx;
    _baud = baud;
    _eParity = eParity;
    _rs485_pin = rs485_pin;
    _level = level;
    if(_serial == &Serial){
        UARTx = UART0;
    }else if(_serial == &Serial1){
        UARTx = UART1;
    }else if(_serial == &Serial2){
        UARTx = UART2;
    }
}

//========================================================================
// 描述: 关闭串口接收.
// 参数: none.
// 返回: none.
//========================================================================
void MBMaster::RecDisable()
{
    // UART_IntClear(UARTx, UART_RXInt);  //清除接收中断标志
    // UART_CRConfig(UARTx, UART_RXE, DISABLE);    //UART禁止接收
}

//========================================================================
// 描述: 使能串口接收.
// 参数: none.
// 返回: none.
//========================================================================
void MBMaster::RecEnble()
{
    // UART_CRConfig(UARTx, UART_RXE, ENABLE);    //UART允许接收
}

//========================================================================
// 描述: modbus发送函数(内部调用)
// 参数: p: 发送的数组; len:发送多少字节.
// 返回: none.
//========================================================================
void MBMaster::Send(uint8_t *p, uint16_t len)
{
    uint16_t crc16_temp;
    uint8_t j;
    uint8_t mb_m_data_temp[32]; //保存串口发送的数据
    for (j = 0; j < len; j++)
        mb_m_data_temp[j] = p[j];
    crc16_temp = CRC16(p, len);
    mb_m_data_temp[len] = crc16_temp;
    mb_m_data_temp[len + 1] = (crc16_temp >> 8);

   RecDisable();
    if(_rs485_pin >= 0){
        digitalWrite(_rs485_pin, _level);
    } 
    SerialPutBuff(mb_m_data_temp, len + 2); //数组输出
    if(_rs485_pin >= 0){
        digitalWrite(_rs485_pin, !(_level));
    }
    RecEnble();
}

//========================================================================
// 描述: modbus读线圈函数
// 参数: MB_id:从站ID; addr:地址; uslen:长度; databuf:读回来的线圈值指针; timeout:超时时间.
// 返回: none.
//========================================================================
int8_t MBMaster::ReqReadCoils(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout)
{
    uint16_t temp;
    uint8_t j;
    
    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x01;//功能号
    temp = addr;
    mb_m_command[2] = (uint8_t)(temp >> 8);//地址高
    mb_m_command[3] = addr;//地址低
    temp = uslen;
    mb_m_command[4] = (uint8_t)(temp >> 8);//寄存器高
    mb_m_command[5] = uslen;//寄存器低
    Send(mb_m_command, 6);//发送
    
    (uslen % 8) ? (uslen = uslen / 8 + 1) : (uslen = uslen / 8);

    if (rev_process(timeout)) //在超时时间内接收到正确的数据
    {

        if (mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x01)//返回正确数据
        {
            for (j = 0; j < uslen; j++)
                databuf[j] = mb_m_rec_temp[3 + j];
            return 1;// 成功
        }else{
            return -1;//不正确类型
        }
    }else{
        return 0;//超时
    }
}

//========================================================================
// 描述: modbus写线圈函数
// 参数: MB_id:从站ID; addr:地址; _databuf:线圈值; timeout:超时时间.
// 返回: none.
//========================================================================
int8_t MBMaster::ReqWriteCoil(uint8_t MB_id, uint16_t addr,  uint8_t _data, uint16_t timeout)
{
    uint16_t temp;

    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x05;//功能号
    temp = addr;
    mb_m_command[2] = (uint8_t)(temp >> 8);//地址高
    mb_m_command[3] = addr;//地址低

    mb_m_command[4] = (_data?0xff:0x00);
    mb_m_command[5] = 0x00;
    Send(mb_m_command, 6);//发送


    if (rev_process(timeout)) //在超时时间内接收到正确的数据
    {
        if (mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x05)//返回正确数据
        {
            return 1;
        }else{
            return -1;//不正确类型
        }
    }else{
        return 0;
    }
}

//========================================================================
// 描述: modbus写多个线圈函数
// 参数: MB_id:从站ID; addr:地址; uslen:长度; _databuf:线圈值; timeout:超时时间.
// 返回: none.
//========================================================================
int8_t MBMaster::ReqWriteCoil_S(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout)
{
    uint16_t temp;
    uint8_t j;

    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x0F;//功能号
    temp = addr;
    mb_m_command[2] = (uint8_t)(temp >> 8);//地址高
    mb_m_command[3] = addr;//地址低
    temp = uslen;
    mb_m_command[4] = (uint8_t)(temp >> 8);//寄存器高
    mb_m_command[5] = uslen;//寄存器低
    (uslen % 8) ? (uslen = uslen / 8 + 1) : (uslen = uslen / 8);
    mb_m_command[6] = uslen;//字节数
    for(j=0;j<uslen;j++)mb_m_command[7+j]=databuf[j];
    
    Send(mb_m_command, 7+uslen);//发送

    if (rev_process(timeout)) //在超时时间内接收到正确的数据
    {
        if (mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x0F)//返回正确数据
        {
            return 1;
        }else{
            return -1;//不正确类型
        } 
    }else{
        return 0;
    }
}

//========================================================================
// 描述: modbus读离散量输入函数
// 参数: MB_id:从站ID; addr:地址; uslen:长度; databuf:离散量值指针; timeout:超时时间.
// 返回: none.
//========================================================================
int8_t MBMaster::ReqReadDiscreteInputs(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout)
{
    uint16_t temp;
    uint8_t j;

    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x02;//功能号
    temp = addr;
    mb_m_command[2] = (uint8_t)(temp >> 8);//地址高
    mb_m_command[3] = addr;//地址低
    temp = uslen;
    mb_m_command[4] = (uint8_t)(temp >> 8);//寄存器数量高
    mb_m_command[5] = uslen;//寄存器数量低
    Send(mb_m_command, 6);//发送

    (uslen % 8) ? (uslen = uslen / 8 + 1) : (uslen = uslen / 8);

    if (rev_process(timeout)) //在超时时间内接收到正确的数据
    {
        if (mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x02)//返回正确数据
        {
            for (j = 0; j < uslen; j++)
                databuf[j] = mb_m_rec_temp[3 + j];
            return 1;
        }else{
            return -1;//不正确类型
        }
    }else{
        return 0;
    }
}

//========================================================================
// 描述: modbus读输入寄存器函数
// 参数: MB_id:从站ID; addr:地址; uslen:长度; databuf:输入寄存器值指针; timeout:超时时间.
// 返回: none.
//========================================================================
int8_t MBMaster::ReqReadInputRegister(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout)
{
    uint16_t temp;
    uint8_t j;

    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x04;//功能号
    temp = addr;
    mb_m_command[2] = (uint8_t)(temp >> 8);//地址高
    mb_m_command[3] = addr;//地址低
    temp = uslen;
    mb_m_command[4] = (uint8_t)(temp >> 8);//寄存器高
    mb_m_command[5] = uslen;//寄存器低
    Send(mb_m_command, 6);//发送

    if (rev_process(timeout)) //在超时时间内接收到正确的数据
    {
        if (mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x04)//返回正确数据
        {
            for (j = 0; j < uslen*2; j++)
                databuf[j] = mb_m_rec_temp[3 + j];
            return 1;
        }else{
            return -1;//不正确类型
        }  
    }else{
        return 0;
    }
}

//========================================================================
// 描述: modbus写单个寄存器函数
// 参数: MB_id:从站ID; addr:地址; _data:要写入的寄存器值; timeout:超时时间.
// 返回: none.
//========================================================================
int8_t MBMaster::ReqWriteHoldingRegister(uint8_t MB_id, uint16_t addr, uint16_t  _data, uint16_t timeout)
{
    uint16_t temp;

    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x06;//功能号
    temp = addr;
    mb_m_command[2] = (uint8_t)(temp >> 8);//地址高
    mb_m_command[3] = addr;//地址低
    temp =_data;
    mb_m_command[4] = (uint8_t)(temp >> 8);//字节高
    mb_m_command[5] = _data;//字节低

    Send(mb_m_command, 6);//发送

    if (rev_process(timeout)) //在超时时间内接收到正确的数据
    {
        if (mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x06)//返回正确数据
        {
            return 1;
        }else{
            return -1;//不正确类型
        }
    }else{
        return 0;
    }
}

//========================================================================
// 描述: modbus写多个寄存器函数
// 参数: MB_id:从站ID; addr:地址; uslen:长度; databuf:要写的寄存器值指针; timeout:超时时间.
// 返回: none.
//========================================================================
int8_t MBMaster::ReqWriteHoldingRegister_S(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout)
{
    uint16_t temp;
    uint8_t j;
  
    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x10;//功能号
    temp = addr;
    mb_m_command[2] = (uint8_t)(temp >> 8);//地址高
    mb_m_command[3] = addr;//地址低
    temp = uslen;
    mb_m_command[4] = (uint8_t)(temp >> 8);//寄存器高
    mb_m_command[5] = uslen;//寄存器低

    mb_m_command[6] = uslen*2;//
    for(j=0;j<uslen*2;j++)mb_m_command[7+j]=databuf[j];
    
    Send(mb_m_command, 7+uslen*2);//发送

    if (rev_process(timeout)) //在超时时间内接收到正确的数据
    {
        if (mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x10)//返回正确数据
        {

            return 1;
        }else{
            return -1;//不正确类型
        }
    }else{
        return 0;
    }
}

//========================================================================
// 描述: modbus读保持寄存器函数
// 参数: MB_id:从站ID; addr:地址; uslen:长度; _data:要写入的寄存器值; timeout:超时时间.
// 返回: none.
//========================================================================
int8_t MBMaster::ReqReadHoldingRegister(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout)
{
    uint16_t temp;
    uint8_t j;

    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x03;//功能号
    temp = addr;
    mb_m_command[2] = (uint8_t)(temp >> 8);//地址高
    mb_m_command[3] = addr;//地址低
    temp = uslen;
    mb_m_command[4] = (uint8_t)(temp >> 8);//寄存器高
    mb_m_command[5] = uslen;//寄存器低
    Send(mb_m_command, 6);//发送

    if (rev_process(timeout)) //在超时时间内接收到正确的数据
    {
        if (mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x03)//返回正确数据
        {
            for (j = 0; j < uslen*2; j++)
                databuf[j] = mb_m_rec_temp[3 + j];
            return 1;
        }else{
            return -1;//不正确类型
        }
    }else{
        return 0;
    }
}

//========================================================================
// 描述: modbus读写多个保持寄存器函数
// 参数: MB_id:从站ID; raddr:读起始地址; ruslen:读的长度; 
//       waddr:写起始地址; wuslen:写长度; databuf:读写指针; timeout:超时时间.
// 返回: none.
//========================================================================
int8_t MBMaster::ReqReadWriteHoldingRegister_S(uint8_t MB_id, uint16_t raddr, uint16_t ruslen,uint16_t waddr, uint16_t wuslen,uint8_t *databuf, uint16_t timeout)
{
    uint16_t temp;
    uint8_t j;
  
    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x17;//功能号
    temp = raddr;
    mb_m_command[2] = (uint8_t)(temp >> 8);//读起始地址高字节
    mb_m_command[3] = raddr;//读起始地址低字节
    temp = ruslen;
    mb_m_command[4] = (uint8_t)(temp >> 8);//读的数量高字节
    mb_m_command[5] = ruslen;//读的数量低字节

    temp = waddr;
    mb_m_command[6] = (uint8_t)(temp >> 8);//写起始地址高字节
    mb_m_command[7] = waddr;//写起始地址低字节
    temp = wuslen;
    mb_m_command[8] = (uint8_t)(temp >> 8);//写的数量高字节
    mb_m_command[9] = wuslen;//写的数量低字节
    mb_m_command[10] = wuslen*2;//
    
    for(j=0;j<wuslen*2;j++)mb_m_command[11+j]=databuf[j];
    
    Send(mb_m_command, 11+wuslen*2);//发送

    if (rev_process(timeout)) //在超时时间内接收到正确的数据
    {
        if (mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x17)//返回正确数据
        {
                for (j = 0; j < ruslen*2; j++)
                    databuf[j] = mb_m_rec_temp[3 + j];
                return 1;
            }else{
                return -1;//不正确类型
            }
    }else{
        return 0;
    }
}

//========================================================================
// 描述: Modbus主站初始化函数
// 参数: none.
// 返回: none.
//========================================================================
void MBMaster::Init()
{
    if(_rs485_pin >= 0){
        pinMode(_rs485_pin, output);
        #if defined(TW_ASR_PRO)
            set_pin_to_gpio_mode(_rs485_pin);
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
    } 
    SerialInit();
    RecDisable();
}

//========================================================================
// 描述: 串口初始化，打开串口中断（内部调用）.
// 参数: uart_n:串口号; uart_rx_pin,uart_tx_pin:引脚; baud:波特率;
//      eParity:校验位
//              USART_Parity_No无校验  USART_Parity_Even：偶校验
//              USART_Parity_Odd：奇校验
// 返回: none.
//========================================================================
void MBMaster::SerialInit()
{
    uint8_t timeout = 0;
    this->_serial->begin(_baud);
    UART_LCRConfig(UARTx, UART_WordLength_8b,UART_StopBits_1, _eParity);
    timeout = 20000 / _baud;
    if(timeout < 5){
        timeout = 5;
    }
    _timeout = timeout;
}

//========================================================================
// 描述: 串口写1字节函数（内部调用）.
// 参数: none.
// 返回: none.
//========================================================================
void MBMaster::SerialPutByte(uint8_t ucByte)
{
    this->_serial->write(ucByte);
    UartPollingSenddone(UARTx);  //等待数据发送完成
}

//========================================================================
// 描述: 串口发送数组（内部调用）.
// 参数: none.
// 返回: none.
//========================================================================
void MBMaster::SerialPutBuff(uint8_t* p, uint16_t len)
{
    taskENTER_CRITICAL();
	while (len--){
        SerialPutByte(*p++);
    }	
    taskEXIT_CRITICAL();
}

//========================================================================
// 描述: modbus主站接收数据函数
// 参数: timeout:串口接收超时时间.
// 返回: none.
//========================================================================
bool MBMaster::rev_process(uint32_t timeout)
{
    volatile uint32_t mb_m_j = 0;
    volatile uint32_t _startMillis = 0;
    uint32_t t = 0;
    uint8_t i = 0;

    _startMillis = millis();
    t = timeout;    //
    do{
        if(this->_serial->available() > 0){
            mb_m_rec_temp[i++] = this->_serial->read();
            _startMillis = millis();
            t = _timeout;
        }else{
            delay(2);
        }
    }while(millis() - _startMillis < t);
    if(i > 0){
        RecDisable();
        this->_serial->flush();
        mb_m_rec_len = i;
        mb_m_crc_temp = CRC16(mb_m_rec_temp, mb_m_rec_len - 2);
        if ((mb_m_rec_temp[mb_m_rec_len - 1] == (mb_m_crc_temp >> 8)) && (mb_m_rec_temp[mb_m_rec_len - 2] == (uint8_t)mb_m_crc_temp))
        {
            return true;
        }
    }
    return false;







    
}

#endif