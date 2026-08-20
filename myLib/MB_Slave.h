#ifndef _MB_Slave_H
#define _MB_Slave_H

#include "asr.h"
#include "HardwareSerial.h"
#include "CRC_16.h"

#define MB_SLAVE0

#ifndef S_COIL_NCOILS
#define S_COIL_NCOILS   2000 //线圈数量
#endif

#ifndef S_DISCRETE_INPUT_NDISCRETES
#define S_DISCRETE_INPUT_NDISCRETES   16 //离散量输入数量
#endif

#ifndef S_REG_INPUT_NREGS
#define S_REG_INPUT_NREGS 100 //输入寄存器数量
#endif

#ifndef S_REG_HOLDING_NREGS
#define S_REG_HOLDING_NREGS 100 //保持寄存器数量
#endif

#ifndef  S_COIL_START
#define  S_COIL_START 0  //线圈初始地址
#endif 

#ifndef S_DISCRETE_INPUT_START
#define S_DISCRETE_INPUT_START 0 //离散量初始地址
#endif 


#ifndef  S_REG_INPUT_START
#define  S_REG_INPUT_START 0  //输入寄存器初始地址
#endif 

#ifndef  S_REG_HOLDING_START
#define  S_REG_HOLDING_START 0   //保持寄存器初始地址
#endif 

class MBSlave{
public:
    MBSlave(uint16_t id,HardwareSerial *serialx,uint32_t baud, UART_Parity eParity);
    MBSlave(uint16_t id,HardwareSerial *serialx,uint32_t baud, UART_Parity eParity,int8_t rs485_pin,uint8_t level);
    void Init(); //modbus从机初始化
    void Poll(); //modbus从机轮询函数
    int8_t GetCoil(uint16_t addr); //获取线圈值
    int8_t SetCoil(uint16_t addr,uint8_t vol); //设置线圈值
    int8_t GetDisc(uint16_t addr); //获取离散量输入值
    int8_t SetDisc(uint16_t addr,uint8_t vol); //设置离散量输入值
    uint16_t GetInputReg(uint16_t addr); //获取输入寄存器值
    int8_t SetInputReg(uint16_t addr,uint16_t vol); //设置输入寄存器值
    uint16_t GetHoldReg(uint16_t addr); //获取保持寄存器值
    int8_t SetHoldReg(uint16_t addr,uint16_t vol); //设置保持寄存器值

private:
    uint16_t usSRegInBuf[S_REG_INPUT_NREGS]; //输入寄存器缓存
    uint16_t usSRegHoldBuf[S_REG_HOLDING_NREGS];   //保持寄存器缓存
    uint8_t ucSCoilBuf[(S_COIL_NCOILS % 8) ? ((S_COIL_NCOILS / 8) + 1) : (S_COIL_NCOILS / 8)];//线圈缓存
    uint8_t ucSDiscInBuf[(S_DISCRETE_INPUT_NDISCRETES % 8) ? ((S_DISCRETE_INPUT_NDISCRETES / 8) + 1) : (S_DISCRETE_INPUT_NDISCRETES / 8)];//从机离散量

    uint8_t ModBus_data[50];      //校验成功接收到数组
    uint8_t ModBus_Send_data[50]; //发送数组
     
    int8_t _rs485_pin = -1;
    uint8_t _level;
    HardwareSerial *_serial;
    uint32_t _baud;
    UART_Parity _eParity;
    uint16_t mb_s_crc_temp = 0; // 16位CRC临时变量
    uint16_t _id; //从站ID
    uint16_t mb_s_send_len;           //发送长度
    uint8_t mb_s_rec_len = 0;         //接收长度

    void SerialInit(); //串口初始化
    void SerialPutByte(uint8_t ucByte); //串口发送字节
    void SerialPutBuff(uint8_t* p, uint16_t len);   //串口发送数组

    void Send(uint8_t *p, uint16_t len);

    void func_01();
    void func_02();
    void func_03();
    void func_04();
    void func_05();
    void func_06();
    void func_15();
    void func_16();
    void process();
};

MBSlave::MBSlave(uint16_t id,HardwareSerial *serialx,uint32_t baud, UART_Parity eParity)
{
    _id = id;
    _serial = serialx;
    _baud = baud;
    _eParity = eParity;
}

MBSlave::MBSlave(uint16_t id,HardwareSerial *serialx,uint32_t baud, UART_Parity eParity,int8_t rs485_pin,uint8_t level)
{
    _id = id;
    _serial = serialx;
    _baud = baud;
    _eParity = eParity;
    _rs485_pin = rs485_pin;
    _level = level;
}

//========================================================================
// 描述: 获取线圈值.
// 参数: addr:线圈地址.
// 返回: 线圈值（不在范围内返回-1）.
//========================================================================
int8_t MBSlave::GetCoil(uint16_t addr)
{
    if ((addr < S_COIL_START) || (addr > (S_COIL_START + S_COIL_NCOILS)))
    {
        return -1;
    }

    if (ucSCoilBuf[(addr - S_COIL_START) / 8] & (1 << ((addr - S_COIL_START) % 8)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//========================================================================
// 描述: 设置线圈值.
// 参数: addr:线圈地址; vol:值（0/1）.
// 返回: 成功返回1.
//========================================================================
int8_t MBSlave::SetCoil(uint16_t addr,uint8_t vol)
{
    if ((addr < S_COIL_START) || (addr > (S_COIL_START + S_COIL_NCOILS)))
    {
        return -1;
    }
    if(vol)
    {
        ucSCoilBuf[(addr - S_COIL_START) / 8]|=(1 << ((addr - S_COIL_START) % 8));

    }else{
        ucSCoilBuf[(addr - S_COIL_START) / 8]&=~(1 << ((addr - S_COIL_START) % 8));

    }
    return 1;
}

//========================================================================
// 描述: 获取离散量输入的值.
// 参数: addr:离散量地址.
// 返回: 成功返回1.
//========================================================================
int8_t MBSlave::GetDisc(uint16_t addr)
{
    if ((addr < S_DISCRETE_INPUT_START) || (addr > (S_DISCRETE_INPUT_START + S_DISCRETE_INPUT_NDISCRETES)))
    {
        return -1;
    }

    if (ucSDiscInBuf[(addr - S_DISCRETE_INPUT_START) / 8] & (1 << ((addr - S_DISCRETE_INPUT_START) % 8)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//========================================================================
// 描述: 设置离散量输入的值.
// 参数: addr:离散量地址; vol:值（0/1）.
// 返回: 成功返回1.
//========================================================================
int8_t MBSlave::SetDisc(uint16_t addr,uint8_t vol)
{
    if ((addr < S_DISCRETE_INPUT_START) || (addr > (S_DISCRETE_INPUT_START + S_DISCRETE_INPUT_NDISCRETES)))
    {
        return -1;
    }
    if(vol)
    {
        ucSDiscInBuf[(addr - S_DISCRETE_INPUT_START)/8]|=(1 << ((addr - S_DISCRETE_INPUT_START) % 8));

    }else{
        ucSDiscInBuf[(addr - S_DISCRETE_INPUT_START) / 8]&=~(1 << ((addr - S_DISCRETE_INPUT_START) % 8));

    }
    return 1;
}

//========================================================================
// 描述: 获取输入寄存器的值.
// 参数: addr:输入寄存器地址.
// 返回: 成功返回1.
//========================================================================
uint16_t MBSlave::GetInputReg(uint16_t addr)
{
    if ((addr < S_REG_INPUT_START) || (addr > (S_REG_INPUT_START + S_REG_INPUT_NREGS)))
    {
        return 0;
    }
    return usSRegInBuf[addr - S_REG_INPUT_START];
}

//========================================================================
// 描述: 设置输入寄存器的值.
// 参数: addr:输入寄存器地址; vol:值.
// 返回: 成功返回1.
//========================================================================
int8_t MBSlave::SetInputReg(uint16_t addr,uint16_t vol)
{
    if ((addr < S_REG_INPUT_START) || (addr > (S_REG_INPUT_START + S_REG_INPUT_NREGS)))
    {
        return -1;
    }
    usSRegInBuf[addr - S_REG_INPUT_START] = vol;
    return 1;
}

//========================================================================
// 描述: 获取保持寄存器的值.
// 参数: addr:保持寄存器地址.
// 返回: 成功返回1.
//========================================================================
uint16_t MBSlave::GetHoldReg(uint16_t addr)
{
    if ((addr < S_REG_HOLDING_START) || (addr > (S_REG_HOLDING_START + S_REG_HOLDING_NREGS)))
    {
        return 0;
    }
    return usSRegHoldBuf[addr - S_REG_HOLDING_START];
}

//========================================================================
// 描述: 设置保持寄存器的值.
// 参数: addr:保持寄存器地址; vol:值.
// 返回: 成功返回1.
//========================================================================
int8_t MBSlave::SetHoldReg(uint16_t addr,uint16_t vol)
{
    if ((addr < S_REG_HOLDING_START) || (addr > (S_REG_HOLDING_START + S_REG_HOLDING_NREGS)))
    {
        return -1;
    }
    usSRegHoldBuf[addr - S_REG_HOLDING_START] = vol;
    return 1;
}

//========================================================================
// 描述: modbus发送函数
// 参数: p: 发送的数组; len:发送多少字节.
// 返回: none.
//========================================================================
void MBSlave::Send(uint8_t *p, uint16_t len)
{
    uint16_t crc16_temp;
    uint8_t data_temp[50];  
    for (uint8_t i = 0; i < len; i++){
        data_temp[i] = p[i];
    }
    crc16_temp = CRC16(p, len);
    data_temp[len] = crc16_temp;
    data_temp[len + 1] = (crc16_temp >> 8);
    if(_rs485_pin >= 0){
        digitalWrite(_rs485_pin, _level);
    } 
    delay(2);
    SerialPutBuff(data_temp, len + 2); //数组输出
    //等待发送完成
    if(_serial == &Serial){
        UartPollingSenddone(UART0);
    }else if(_serial == &Serial1){
        UartPollingSenddone(UART1);
    }else if(_serial == &Serial2){
        UartPollingSenddone(UART1);
    }
    delay(2);    
    if(_rs485_pin >= 0){
        digitalWrite(_rs485_pin, !(_level));
    }
}

//========================================================================
// 描述: modbus初始化函数
// 参数: uart_n：串口号; uart_rx_pin,uart_tx_pin:引脚; baud:波特率;
//          tim_n:采用的定时器; eParity:校验方式.
// 返回: none.
//========================================================================
void MBSlave::Init()
{
    if(_rs485_pin >= 0)
    {
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
}

//========================================================================
// 描述: modbus读线圈（01功能码）处理函数（内部调用）
// 参数: none.
// 返回: none.
//========================================================================
void MBSlave::func_01()
{
    uint8_t k;
    uint16_t slen, sadder;
    slen = ModBus_data[mb_s_rec_len - 4] * 256 + ModBus_data[mb_s_rec_len - 3];
    if (slen % 8)
    {
        mb_s_send_len = slen / 8 + 1;
    }
    else
    {
        mb_s_send_len = (slen / 8);
    }

    sadder = ModBus_data[2] * 256 + ModBus_data[3];

    if (sadder < S_COIL_START)
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x81;
        ModBus_Send_data[2] = 0x02;
        Send(ModBus_Send_data, 3);
    }
    else if ((sadder + slen) > (S_COIL_START + S_COIL_NCOILS))
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x81;
        ModBus_Send_data[2] = 0x03;
        Send(ModBus_Send_data, 3);
    }
    else
    {

        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x01;
        ModBus_Send_data[2] = mb_s_send_len;
        for (k = 0; k < mb_s_send_len; k++)
        {
            ModBus_Send_data[k + 3] = ucSCoilBuf[(sadder - S_COIL_START) / 8 + k];
        }
        Send(ModBus_Send_data, mb_s_send_len + 3);
    }
}

//========================================================================
// 描述: modbus读离散量输入（02功能码）处理函数（内部调用）
// 参数: none.
// 返回: none.
//========================================================================
void MBSlave::func_02()
{

    uint8_t k;
    uint16_t slen, sadder;
    slen = ModBus_data[mb_s_rec_len - 4] * 256 + ModBus_data[mb_s_rec_len - 3];
    if (slen % 8)
    {
        mb_s_send_len = slen / 8 + 1;
    }
    else
    {
        mb_s_send_len = (slen / 8);
    }

    sadder = ModBus_data[2] * 256 + ModBus_data[3];

    if (sadder < S_DISCRETE_INPUT_START) //地址错误
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x82;
        ModBus_Send_data[2] = 0x02;
        Send(ModBus_Send_data, 3);
    }
    else if ((sadder + slen) > (S_DISCRETE_INPUT_START + S_DISCRETE_INPUT_NDISCRETES)) //数据越界
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x82;
        ModBus_Send_data[2] = 0x03;
        Send(ModBus_Send_data, 3);
    }
    else
    {

        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x02;
        ModBus_Send_data[2] = mb_s_send_len;
        for (k = 0; k < mb_s_send_len; k++)
        {
            ModBus_Send_data[k + 3] = ucSDiscInBuf[(sadder - S_DISCRETE_INPUT_START) / 8 + (sadder - S_DISCRETE_INPUT_START) % 8 + k];
        }
        Send(ModBus_Send_data, mb_s_send_len + 3);
    }
}
//========================================================================
// 描述: modbus读保持寄存器（03功能码）处理函数（内部调用）
// 参数: none.
// 返回: none.
//========================================================================
void MBSlave::func_03()
{
    uint8_t k;
    uint16_t sadder;
    mb_s_send_len = ModBus_data[mb_s_rec_len - 3] * 2;
    sadder = ModBus_data[2] * 256 + ModBus_data[3];
    if (sadder < S_REG_HOLDING_START)
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x83;
        ModBus_Send_data[2] = 0x02;
        Send(ModBus_Send_data, 3);
    }
    else if ((sadder + mb_s_send_len) > (S_REG_HOLDING_START + (S_REG_HOLDING_NREGS * 2)))
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x83;
        ModBus_Send_data[2] = 0x03;
        Send(ModBus_Send_data, 3);
    }
    else
    {

        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x03;
        ModBus_Send_data[2] = mb_s_send_len;
        for (k = 0; k < (mb_s_send_len / 2); k++)
        {

            ModBus_Send_data[2 * k + 4] = (uint8_t)usSRegHoldBuf[(sadder - S_REG_HOLDING_START) + k];
            ModBus_Send_data[2 * k + 3] = usSRegHoldBuf[(sadder - S_REG_HOLDING_START) + k] >> 8;
        }
        Send(ModBus_Send_data, mb_s_send_len + 3);
    }
}

//========================================================================
// 描述: modbus读输入寄存器（04功能码）处理函数（内部调用）
// 参数: none.
// 返回: none.
//========================================================================
void MBSlave::func_04()
{
    uint8_t k;
    uint16_t sadder;
    mb_s_send_len = ModBus_data[mb_s_rec_len - 3] * 2;
    sadder = ModBus_data[2] * 256 + ModBus_data[3];
    if (sadder < S_REG_HOLDING_START)
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x84;
        ModBus_Send_data[2] = 0x02;
        Send(ModBus_Send_data, 3);
    }
    else if ((sadder + mb_s_send_len) > (S_REG_INPUT_START + (S_REG_INPUT_NREGS * 2)))
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x84;
        ModBus_Send_data[2] = 0x03;
        Send(ModBus_Send_data, 3);
    }
    else
    {

        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x04;
        ModBus_Send_data[2] = mb_s_send_len;
        for (k = 0; k < (mb_s_send_len / 2); k++)
        {

            ModBus_Send_data[2 * k + 4] = (uint8_t)usSRegInBuf[(sadder - S_REG_INPUT_START) + k];
            ModBus_Send_data[2 * k + 3] = usSRegInBuf[(sadder - S_REG_INPUT_START) + k] >> 8;
        }
        Send(ModBus_Send_data, mb_s_send_len + 3);
    }
}

//========================================================================
// 描述: modbus写单个线圈（05功能码）处理函数（内部调用）
// 参数: none.
// 返回: none.
//========================================================================
void MBSlave::func_05()
{
    uint8_t k;
    uint16_t sadder = ModBus_data[2] * 256 + ModBus_data[3];
    if (sadder < S_COIL_START || sadder > (S_COIL_START + S_COIL_NCOILS))
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x85;
        ModBus_Send_data[2] = 0x02;
        Send(ModBus_Send_data, 3);
    }
    else
    {
        sadder = (sadder - S_COIL_START);
        if (ModBus_data[4])
        {
            ucSCoilBuf[sadder / 8] |= (1 << (sadder % 8));
        }
        else
        {
            ucSCoilBuf[sadder / 8] &= ~(1 << (sadder % 8));
        }

        for (k = 0; k < 8; k++)
            ModBus_Send_data[k] = ModBus_data[k];
        Send(ModBus_Send_data, 6);
    }
}

//========================================================================
// 描述: modbus写单个保持寄存器（06功能码）处理函数（内部调用）
// 参数: none.
// 返回: none.
//========================================================================
void MBSlave::func_06()
{
    uint8_t k;
    uint16_t sadder = ModBus_data[2] * 256 + ModBus_data[3];
    if (sadder < S_REG_HOLDING_START || sadder > (S_REG_HOLDING_START + S_REG_HOLDING_NREGS))
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x86;
        ModBus_Send_data[2] = 0x02;
        Send(ModBus_Send_data, 3);
    }
    else
    {
        sadder = (sadder - S_REG_HOLDING_START);
        usSRegHoldBuf[sadder] = ModBus_data[4] * 256 + ModBus_data[5];

        for (k = 0; k < 8; k++)
            ModBus_Send_data[k] = ModBus_data[k];
        Send(ModBus_Send_data, 6);
    }
}

//========================================================================
// 描述: modbus写多个线圈（15功能码）处理函数（内部调用）
// 参数: none.
// 返回: none.
//========================================================================
void MBSlave::func_15()
{
    uint8_t k;
    uint16_t slen, sadder;
    slen = ModBus_data[4] * 256 + ModBus_data[5];
    if (slen % 8)
    {
        mb_s_send_len = slen / 8 + 1;
    }
    else
    {
        mb_s_send_len = (slen / 8);
    }

    sadder = ModBus_data[2] * 256 + ModBus_data[3];

    if (sadder < S_COIL_START)
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x8F;
        ModBus_Send_data[2] = 0x02;
        Send(ModBus_Send_data, 3);
    }
    else if ((sadder + slen) > (S_COIL_START + S_COIL_NCOILS))
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x8F;
        ModBus_Send_data[2] = 0x03;
        Send(ModBus_Send_data, 3);
    }
    else
    {
        for (k = 0; k < ModBus_data[6]; k++)

            ucSCoilBuf[((sadder - S_COIL_START) / 8) + k] = ModBus_data[7 + k]; //只能以8的倍数为起始地址发送

        for (k = 0; k < 8; k++)
            ModBus_Send_data[k] = ModBus_data[k];
        Send(ModBus_Send_data, 6);
    }
}

//========================================================================
// 描述: modbus写多个保持寄存器（16功能码）处理函数（内部调用）
// 参数: none.
// 返回: none.
//========================================================================
void MBSlave::func_16()
{
    uint8_t k;
    uint16_t sadder;
    mb_s_send_len = ModBus_data[6];
    sadder = ModBus_data[2] * 256 + ModBus_data[3];
    if (sadder < S_REG_HOLDING_START)
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x90;
        ModBus_Send_data[2] = 0x02;
        Send(ModBus_Send_data, 3);
    }
    else if ((sadder + mb_s_send_len) > (S_REG_HOLDING_START + (S_REG_HOLDING_NREGS * 2)))
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x90;
        ModBus_Send_data[2] = 0x03;
        Send(ModBus_Send_data, 3);
    }
    else
    {
        for (k = 0; k < 8; k++)
            ModBus_Send_data[k] = ModBus_data[k];
        Send(ModBus_Send_data, 6);
    }
    for (k = 0; k < (mb_s_send_len / 2); k++)
    {
        usSRegHoldBuf[sadder - S_REG_HOLDING_START + k] = ModBus_data[7 + 2 * k] * 256 + ModBus_data[8 + 2 * k];
    }
}

void MBSlave::process()
{
    if (ModBus_data[1] == 1)
    {
        func_01();
    }
    if (ModBus_data[1] == 2)
    {
        func_02();
    }
    if (ModBus_data[1] == 3)
    {
        func_03();
    }
    if (ModBus_data[1] == 4)
    {
        func_04();
    }
    if (ModBus_data[1] == 5)
    {
        func_05();
    }
    if (ModBus_data[1] == 6)
    {
        func_06();
    }
    if (ModBus_data[1] == 15)
    {
        func_15();
    }
    if (ModBus_data[1] == 16)
    {
        func_16();
    }
}


//========================================================================
// 描述: modbus轮询（在主函数中轮询）
// 参数: none.
// 返回: none.
//========================================================================
void MBSlave::Poll()
{
    if(this->_serial->available() > 0)
    {
        String str = this->_serial->readString();
        this->_serial->flush();
        mb_s_rec_len = str.length();
        if(mb_s_rec_len != 0){
            for (uint8_t i = 0; i < mb_s_rec_len; i++)
            {
                ModBus_data[i] = str[i];
            }
            mb_s_crc_temp = CRC16(ModBus_data, mb_s_rec_len - 2);
            if ((ModBus_data[mb_s_rec_len - 1] == (mb_s_crc_temp >> 8)) && (ModBus_data[mb_s_rec_len - 2] == (uint8_t)mb_s_crc_temp) && (ModBus_data[0] == _id))
            {
                process();
            }
        }
    }}

//========================================================================
// 描述: 串口初始化，打开串口中断（内部调用）.
// 参数: none.
// 返回: none.
//========================================================================
void MBSlave::SerialInit()
{
    uint8_t timeout = 0;
    this->_serial->begin(_baud);

    if(_serial == &Serial){
        UART_LCRConfig(UART0, UART_WordLength_8b,UART_StopBits_1, _eParity);
    }else if(_serial == &Serial1){
        UART_LCRConfig(UART1, UART_WordLength_8b,UART_StopBits_1, _eParity);
    }else if(_serial == &Serial2){
        UART_LCRConfig(UART2, UART_WordLength_8b,UART_StopBits_1, _eParity);
    }
    timeout = 20000 / _baud;
    if(timeout < 4){
        timeout = 4;
    }
    this->_serial->setTimeout(timeout);
}

//========================================================================
// 描述: 串口写1字节函数（内部调用）.
// 参数: none.
// 返回: none.
//========================================================================
void MBSlave::SerialPutByte(uint8_t ucByte)
{
    this->_serial->write(ucByte);
}

//========================================================================
// 描述: 串口发送数组（内部调用）.
// 参数: none.
// 返回: none.
//========================================================================
void MBSlave::SerialPutBuff(uint8_t* p, uint16_t len)
{
	while (len--){
        SerialPutByte(*p++);
    }	
}

#endif