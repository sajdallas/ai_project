/**
 * @file ASR_RC522.h
 * @brief 
 * @author LI (com)
 * @version 1.0
 * @date 2024-05-10
 * 
 * @copyright Copyright (c) 2024 haohaodada.com
 * 
 */
#ifndef __ASR_RC522_h
#define __ASR_RC522_h

#include "asr_softspi.h"
#include "string.h"
//MF522命令字
#define PCD_IDLE              0x00               //取消当前命令
#define PCD_AUTHENT           0x0E               //验证密钥
#define PCD_RECEIVE           0x08               //接收数据
#define PCD_TRANSMIT          0x04               //发送数据
#define PCD_TRANSCEIVE        0x0C               //发送并接收数据
#define PCD_RESETPHASE        0x0F               //复位
#define PCD_CALCCRC           0x03               //CRC计算

//Mifare_One卡片命令字
#define PICC_REQIDL           0x26               //寻天线区内未进入休眠状态
#define PICC_REQALL           0x52               //寻天线区内全部卡
#define PICC_ANTICOLL1        0x93               //防冲撞
#define PICC_ANTICOLL2        0x95               //防冲撞
#define PICC_AUTHENT1A        0x60               //验证A密钥
#define PICC_AUTHENT1B        0x61               //验证B密钥
#define PICC_READ             0x30               //读块
#define PICC_WRITE            0xA0               //写块
#define PICC_DECREMENT        0xC0               //扣款
#define PICC_INCREMENT        0xC1               //充值
#define PICC_RESTORE          0xC2               //调块数据到缓冲区
#define PICC_TRANSFER         0xB0               //保存缓冲区中数据
#define PICC_HALT             0x50               //休眠

//MF522 FIFO长度定义
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte

//MF522寄存器定义
// PAGE 0
#define     RFU00                 0x00    
#define     CommandReg            0x01    
#define     ComIEnReg             0x02    
#define     DivlEnReg             0x03    
#define     ComIrqReg             0x04    
#define     DivIrqReg             0x05
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08    
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     RFU0F                 0x0F
// PAGE 1     
#define     RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
// PAGE 2    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
// PAGE 3      
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     RFU3C                 0x3C   
#define     RFU3D                 0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F		          0x3F

//和MF522通讯时返回的错误代码
#define     MI_OK                 0
#define     MI_NOTAGERR           (-1)
#define     MI_ERR                (-2)

#define     MAXRLEN               18


class RC522
{
public:
    RC522(uint8_t sdapin,uint8_t sckpin, uint8_t mosipin, uint8_t misopin, uint8_t rstpin);
    void begin();
    void rc522_set_bit_mask(uint8_t reg,uint8_t mask) ;
    void rc522_clear_bit_mask(uint8_t reg,uint8_t mask)  ;
    int8_t rc522_pcd_reset();
    void rc522_pcd_antenna_on();//开启天线
    void rc522_pcd_antenna_off();
    int8_t rc522_pcd_com_mf522(uint8_t Command, uint8_t *pInData, uint8_t InLenByte,uint8_t *pOutData, uint16_t*pOutLenBit);

    int8_t rc522_pcd_request(uint8_t req_code,uint8_t *pTagType);     //寻卡
    int8_t rc522_pcd_anticoll(uint8_t *pSnr);   //防冲撞
    int8_t rc522_pcd_select(uint8_t *pSnr);     //选卡
    int8_t rc522_pcd_auth_state(uint8_t auth_mode,uint8_t addr,uint8_t *pKey,uint8_t *pSnr);  //验证卡密码
    int8_t rc522_pcd_read(uint8_t addr,uint8_t *pData);   //读取一块数据
    int8_t rc522_pcd_write(uint8_t addr,uint8_t *pData);  //写入一块数据
    int8_t rc522_pcd_value(uint8_t dd_mode,uint8_t addr,uint8_t *pValue);   //扣款和充值
    int8_t rc522_pcd_bak_value(uint8_t sourceaddr, uint8_t goaladdr); //备份钱包
    int8_t rc522_pcd_halt();  //进入休眠
    void rc522_calulate_crc(uint8_t *pIndata,uint8_t len,uint8_t *pOutData);  //crc校验
    int8_t rc522_m500_pcd_config_ios_type(uint8_t type);    //设置工作方式

private:
    SOFTSPI softspi;
    uint8_t _sdapin;
    uint8_t _sckpin;
    uint8_t _mosipin;
    uint8_t _misopin;
    uint8_t _rstpin;
    uint8_t rc522_read_raw(uint8_t Address); 
    void rc522_write_raw(uint8_t Address, uint8_t value);
};

/**
 * @brief Construct a new RC522::RC522 object
 */
RC522::RC522(uint8_t sdapin,uint8_t sckpin, uint8_t mosipin, uint8_t misopin, uint8_t rstpin)
{
    _sdapin = sdapin;
    _sckpin = sckpin;
    _mosipin = mosipin;
    _misopin = misopin;
    _rstpin = rstpin;
}
/**
 * @brief RC522初始化
 */
void RC522::begin()
{
    set_pin_to_gpio_mode(_sdapin);
    set_pin_to_gpio_mode(_rstpin);
    pinMode(_sdapin,output);
    pinMode(_rstpin,output);
    softspi.begin(_sckpin,_mosipin,_misopin);

    rc522_pcd_reset();
    rc522_pcd_antenna_off(); 
    rc522_pcd_antenna_on();  
    rc522_m500_pcd_config_ios_type('A');
}
/**
 * @brief 读RC522寄存器
 * @param  Address          地址
 * @return uint8_t 返回字节
 */
uint8_t RC522::rc522_read_raw(uint8_t Address)
{
    uint8_t ucAddr;
    uint8_t ucResult=0;

    digitalWrite(_sdapin, 0);
    ucAddr = ((Address<<1)&0x7E)|0x80;
    softspi.wr_data(ucAddr);
    ucResult = softspi.wr_data(0xff);
    digitalWrite(_sdapin, 1);

    return ucResult;   
}
/**
 * @brief 写RC522寄存器.（spi:CPOL=0,CPHA=0）
 * @param  Address          地址
 * @param  value            数值
 */
void RC522::rc522_write_raw(uint8_t Address, uint8_t value)
{  
    uint8_t ucAddr;
    ucAddr = ((Address<<1)&0x7E);
    digitalWrite(_sdapin, 0);
    softspi.wr_data(ucAddr);
    softspi.wr_data(value);
    digitalWrite(_sdapin, 1);
}
/**
 * @brief 置RC522寄存器bit位.
 * @param  reg              寄存器地址
 * @param  mask             清位值
 */
void RC522::rc522_set_bit_mask(uint8_t reg,uint8_t mask)  
{
    int8_t tmp = 0x0;
    tmp = rc522_read_raw(reg);
    rc522_write_raw(reg,tmp | mask);  // set bit mask
}
/**
 * @brief 清除RC522寄存器bit位.
 * @param  reg              寄存器地址
 * @param  mask             清位值
 */
void RC522::rc522_clear_bit_mask(uint8_t reg,uint8_t mask)
{
    int8_t tmp = 0x0;
    tmp = rc522_read_raw(reg);
    rc522_write_raw(reg, tmp & ~mask);  // clear bit mask
}
/**
 * @brief 复位RC552
 * @return int8_t MI_OK
 */
int8_t RC522::rc522_pcd_reset()
{
    digitalWrite(_rstpin, 1);
    delay(1);
    digitalWrite(_rstpin, 0);
    delay(1);
    digitalWrite(_rstpin, 1);
    delay(1);

    rc522_write_raw(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
    rc522_write_raw(TReloadRegL,30);           
    rc522_write_raw(TReloadRegH,0);
    rc522_write_raw(TModeReg,0x8D);
    rc522_write_raw(TPrescalerReg,0x3E);
    rc522_write_raw(TxAutoReg,0x40);     
    return MI_OK;
}
/**
 * @brief 开启天线（每次启动或关闭天险发射之间应至少有1ms的间隔）
 */
void RC522::rc522_pcd_antenna_on()
{
    uint8_t tem;
    tem = rc522_read_raw(TxControlReg);
    if (!(tem & 0x03))
    {
        rc522_set_bit_mask(TxControlReg, 0x03);
    }
}
/**
 * @brief 关闭天线（每次启动或关闭天险发射之间应至少有1ms的间隔）
 */
void RC522::rc522_pcd_antenna_off()
{
    rc522_clear_bit_mask(TxControlReg, 0x03);
}
/**
 * @brief RC522和ISO14443卡通讯
 * @param  Command          RC522命令字
 * @param  pInData          RC522发送到卡片的数据
 * @param  InLenByte        发送数据的字节长度
 * @param  pOutData         接收到的卡片返回数据
 * @param  pOutLenBit       返回数据的位长度
 * @return int8_t 成功返回MI_OK
 */
int8_t RC522::rc522_pcd_com_mf522(uint8_t Command, uint8_t *pInData, uint8_t InLenByte,uint8_t *pOutData, uint16_t*pOutLenBit)
{
    int8_t status = MI_ERR;
    uint8_t irqEn   = 0x00;
    uint8_t waitFor = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint16_t i;
    switch (Command)
    {
        case PCD_AUTHENT:
            irqEn   = 0x12;
            waitFor = 0x10;
        break;
        case PCD_TRANSCEIVE:
            irqEn   = 0x77;
            waitFor = 0x30;
        break;
        default:
         break;
    }
   
    rc522_write_raw(ComIEnReg,irqEn|0x80);
    rc522_clear_bit_mask(ComIrqReg,0x80);
    rc522_write_raw(CommandReg,PCD_IDLE);
    rc522_set_bit_mask(FIFOLevelReg,0x80);
    
    for (i=0; i<InLenByte; i++)
    {   
        rc522_write_raw(FIFODataReg, pInData[i]);    
    }
    rc522_write_raw(CommandReg, Command);
   
    if (Command == PCD_TRANSCEIVE)
    {    
        rc522_set_bit_mask(BitFramingReg,0x80);  
    }
    
    // i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
    i = 2000;
    do 
    {
        n = rc522_read_raw(ComIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    rc522_clear_bit_mask(BitFramingReg,0x80);    
    if (i!=0)
    {    
        if(!(rc522_read_raw(ErrorReg)&0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {   
                status = MI_NOTAGERR;   
            }
            if (Command == PCD_TRANSCEIVE)
            {
               	n = rc522_read_raw(FIFOLevelReg);
              	lastBits = rc522_read_raw(ControlReg) & 0x07;
                if (lastBits)
                {   
                    *pOutLenBit = (n-1)*8 + lastBits;   
                }
                else
                {   
                    *pOutLenBit = n*8;   
                }
                if (n == 0)
                {  
                    n = 1;    
                }
                if (n > MAXRLEN)
                {   
                    n = MAXRLEN;   
                }
                for (i=0; i<n; i++)
                {   
                    pOutData[i] = rc522_read_raw(FIFODataReg);    
                }
            }
        }
        else
        {   
            status = MI_ERR;   
        }
   }
   rc522_set_bit_mask(ControlReg,0x80);           // stop timer now
   rc522_write_raw(CommandReg,PCD_IDLE); 
   return status;
}
/**
 * @brief 寻卡
 * @param  req_code         寻卡方式 
 *                          0x52 = 寻感应区内所有符合14443A标准的卡
 *                          0x26 = 寻未进入休眠状态的卡
 * @param  pTagType         卡片类型代码(返回)
                            0x4400 = Mifare_UltraLight
                            0x0400 = Mifare_One(S50)
                            0x0200 = Mifare_One(S70)
                            0x0800 = Mifare_Pro(X)
                            0x4403 = Mifare_DESFire
 * @return int8_t 
 */
int8_t RC522::rc522_pcd_request(uint8_t req_code,uint8_t *pTagType)
{
    int8_t status;  
    uint16_t  unLen;
    uint8_t ucComMF522Buf[MAXRLEN]; 

    rc522_clear_bit_mask(Status2Reg,0x08);
    rc522_write_raw(BitFramingReg,0x07);

    rc522_set_bit_mask(TxControlReg,0x03);
 
    ucComMF522Buf[0] = req_code;

    status = rc522_pcd_com_mf522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);

    if ((status == MI_OK) && (unLen == 0x10))
    {    
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
    }
    else
    {   
        status = MI_ERR;   
    }
    return status;   
}
/**
 * @brief 防冲撞
 * @param  pSnr             卡片序列号，4字节
 * @return 成功返回MI_OK 
 */
int8_t RC522::rc522_pcd_anticoll(uint8_t *pSnr)
{
    int8_t status;
    uint8_t i,snr_check=0;
    uint16_t unLen;
    uint8_t ucComMF522Buf[MAXRLEN]; 

    rc522_clear_bit_mask(Status2Reg,0x08);
    rc522_write_raw(BitFramingReg,0x00);
    rc522_clear_bit_mask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = rc522_pcd_com_mf522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	for (i=0; i<4; i++)
        {   
            *(pSnr+i)  = ucComMF522Buf[i];
            snr_check ^= ucComMF522Buf[i];
        }
        if (snr_check != ucComMF522Buf[i])
        {   
            status = MI_ERR;    
        }
    }
    
    rc522_set_bit_mask(CollReg,0x80);
    return status;
}
/** 选定卡片
 * @brief 
 * @param  pSnr             卡片序列号，4字节
 * @return 成功返回MI_OK 
 */
int8_t RC522::rc522_pcd_select(uint8_t *pSnr)
{
    int8_t status;
    uint8_t i;
    uint16_t  unLen;
    uint8_t ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    rc522_calulate_crc(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    rc522_clear_bit_mask(Status2Reg,0x08);

    status = rc522_pcd_com_mf522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   
        status = MI_OK;  
    }
    else
    {   
        status = MI_ERR;    
    }
    return status;  
}
/**
 * @brief 验证卡片密码
 * @param  auth_mode        密码验证模式(0x60 = 验证A密钥 0x61 = 验证B密钥 )
 * @param  addr             块地址(0~63)
 * @param  pKey             密码(6字节)
 * @param  pSnr             卡片序列号，4字节
 * @return int8_t 
 */
int8_t RC522::rc522_pcd_auth_state(uint8_t auth_mode,uint8_t addr,uint8_t *pKey,uint8_t *pSnr)
{
    int8_t status;
    uint16_t  unLen;
    uint8_t i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {   
        ucComMF522Buf[i+2] = *(pKey+i);   
    }
    for (i=0; i<6; i++)
    {    
        ucComMF522Buf[i+8] = *(pSnr+i);   
    }
    // memcpy(&ucComMF522Buf[2], pKey, 6); 
    // memcpy(&ucComMF522Buf[8], pSnr, 4); 
    
    status = rc522_pcd_com_mf522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(rc522_read_raw(Status2Reg) & 0x08)))
    {   
        status = MI_ERR;   
    }
    return status;
}
/**
 * @brief 读取M1卡一块数据
 * @param  addr             块地址(0~63)
 * @param  pData            读出的数据，16字节
 * @return 成功返回MI_OK 
 */
int8_t RC522::rc522_pcd_read(uint8_t addr,uint8_t *pData)
{
    int8_t status;
    uint16_t  unLen;
    uint8_t i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    rc522_calulate_crc(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = rc522_pcd_com_mf522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
    // {   
    //     memcpy(pData, ucComMF522Buf, 16);   
    // }
    {
        for (i=0; i<16; i++)
        {    
            *(pData+i) = ucComMF522Buf[i];   
        }
    }
    else
    {   
        status = MI_ERR;   
    }
    return status;   
}
/**
 * @brief 写数据到M1卡一块
 * @param  addr             块地址(0~63)
 * @param  pData            写入的数据，16字节
 * @return int8_t 
 */
int8_t RC522::rc522_pcd_write(uint8_t addr,uint8_t *pData)
{
    int8_t status;
    uint16_t  unLen;
    uint8_t i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    rc522_calulate_crc(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = rc522_pcd_com_mf522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   
        status = MI_ERR;   
    }
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, pData, 16);
        for (i=0; i<16; i++)
        {    
            ucComMF522Buf[i] = *(pData+i);   
        }
        rc522_calulate_crc(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = rc522_pcd_com_mf522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   
            status = MI_ERR;   
        }
    }
    return status;
}
/**
 * @brief 扣款和充值
 * @param  dd_mode          命令字 0xC0 = 扣款 0xC1 = 充值
 * @param  addr             钱包地址
 * @param  pValue           4字节增(减)值，低位在前
 * @return 成功返回MI_OK 
 */
int8_t RC522::rc522_pcd_value(uint8_t dd_mode,uint8_t addr,uint8_t *pValue)
{
    int8_t status;
    uint16_t  unLen;
    uint8_t ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    rc522_calulate_crc(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = rc522_pcd_com_mf522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   
        status = MI_ERR;   
    }
        
    if (status == MI_OK)
    {
        memcpy(ucComMF522Buf, pValue, 4);
        // for (i=0; i<16; i++)
        // {    
        //     ucComMF522Buf[i] = *(pValue+i);   
        // }
        rc522_calulate_crc(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = rc522_pcd_com_mf522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    
            status = MI_OK;    
        }
    }
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        rc522_calulate_crc(ucComMF522Buf,2,&ucComMF522Buf[2]); 
   
        status = rc522_pcd_com_mf522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   
            status = MI_ERR;   
        }
    }
    return status;
}
/**
 * @brief 备份钱包
 * @param  sourceaddr       源地址
 * @param  goaladdr         目标地址
 * @return 成功返回MI_OK 
 */
int8_t RC522::rc522_pcd_bak_value(uint8_t sourceaddr, uint8_t goaladdr)
{
    int8_t status;
    uint16_t  unLen;
    uint8_t ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    rc522_calulate_crc(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = rc522_pcd_com_mf522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   
        status = MI_ERR;   
    }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        rc522_calulate_crc(ucComMF522Buf,4,&ucComMF522Buf[4]);
 
        status = rc522_pcd_com_mf522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {   
            status = MI_OK;    
        }
    }
    
    if (status != MI_OK)
    {    
        return MI_ERR;   
    }
    
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    rc522_calulate_crc(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = rc522_pcd_com_mf522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   
        status = MI_ERR;   
    }

    return status;
}
/**
 * @brief 命令卡片进入休眠状态
 * @return 成功返回MI_OK 
 */
int8_t RC522::rc522_pcd_halt()
{
    int8_t status;
    uint16_t  unLen;
    uint8_t ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    rc522_calulate_crc(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = rc522_pcd_com_mf522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    return status;
}
/**
 * @brief 用CR522计算CRC16函数
 * @param  pIndata          数据地址
 * @param  len              数据长度
 * @param  pOutData         检验值（两个字节）
 */
void RC522::rc522_calulate_crc(uint8_t *pIndata,uint8_t len,uint8_t *pOutData)
{
    uint8_t i,n;
    rc522_clear_bit_mask(DivIrqReg,0x04);
    rc522_write_raw(CommandReg,PCD_IDLE);
    rc522_set_bit_mask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   
        rc522_write_raw(FIFODataReg, *(pIndata+i));   
    }
    rc522_write_raw(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = rc522_read_raw(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = rc522_read_raw(CRCResultRegL);
    pOutData[1] = rc522_read_raw(CRCResultRegM);  
}
/**
 * @brief 设置RC552的工作方式
 * @param  type             My Param doc
 * @return int8_t 
 */
int8_t RC522::rc522_m500_pcd_config_ios_type(uint8_t type)
{
    if (type == 'A')                     //ISO14443_A
    { 
        rc522_clear_bit_mask(Status2Reg,0x08);

        // rc522_write_raw(CommandReg,0x20);    //as default   
        // rc522_write_raw(ComIEnReg,0x80);     //as default
        // rc522_write_raw(DivlEnReg,0x0);      //as default
	    // rc522_write_raw(ComIrqReg,0x04);     //as default
	    // rc522_write_raw(DivIrqReg,0x0);      //as default
	    // rc522_write_raw(Status2Reg,0x0);//80    //trun off temperature sensor
	    // rc522_write_raw(WaterLevelReg,0x08); //as default
        // rc522_write_raw(ControlReg,0x20);    //as default
	    // rc522_write_raw(CollReg,0x80);    //as default

        rc522_write_raw(ModeReg,0x3D);//3F

        // rc522_write_raw(TxModeReg,0x0);      //as default???
        // rc522_write_raw(RxModeReg,0x0);      //as default???
        // rc522_write_raw(TxControlReg,0x80);  //as default???
        // rc522_write_raw(TxSelReg,0x10);      //as default???

        rc522_write_raw(RxSelReg,0x86);//84

        // rc522_write_raw(RxThresholdReg,0x84);//as default
        // rc522_write_raw(DemodReg,0x4D);      //as default

        // rc522_write_raw(ModWidthReg,0x13);//26
        rc522_write_raw(RFCfgReg,0x7F);   //4F
        // rc522_write_raw(GsNReg,0x88);        //as default???
        // rc522_write_raw(CWGsCfgReg,0x20);    //as default???
        // rc522_write_raw(ModGsCfgReg,0x20);   //as default???

        rc522_write_raw(TReloadRegL,30);  //tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
        rc522_write_raw(TReloadRegH,0);
        rc522_write_raw(TModeReg,0x8D);
        rc522_write_raw(TPrescalerReg,0x3E);
	   

        // PcdSetTmo(106);
	    delay(10);
        rc522_pcd_antenna_on();
    }
    else
    {
        return MI_NOTAGERR; 
    }
   
   return MI_OK;
}
#endif