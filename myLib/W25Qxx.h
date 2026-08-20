/**
 * @file W25Qxx.h
 * @brief 
 * @author LI (com)
 * @version 1.0
 * @date 2024-01-19
 * 
 * @copyright Copyright (c) 2024 haohaodada.com
 * 
 */
#ifndef __W25Qxx_H__
#define __W25Qxx_H__

#ifndef W25Qxx_USE_HARDSPI
#define W25Qxx_USE_HARDSPI  1   //1:使用硬件SPI   0：使用软件SPI
#endif

#if (defined(__TW32F0XX_H))
#if (W25Qxx_USE_HARDSPI)
#include "../myLib/TW32f0xx_HARDSPI.h"
//TW32F003默认硬件SPI引脚PA2(SPI1_SCK),PA1(SPI1_MOSI) ， PA0(SPI1_MISO)
#else
#include "../myLib/TW32f0xx_SOFTSPI.h"
#endif
#endif
#if 0       //CH32V003flash及RAM空间不足，如有需要请自行修改库
#if (defined(__CH32V00x_H)) 
#if (W25Qxx_USE_HARDSPI)
#include "myLib/CH32V_HARDSPI.h"
//CH32V003默认硬件SPI引脚PC5(SCK),PC6(MOSI),PC7(MISO)
#else
#include "myLib/CH32V_SOFTSPI.h"
#endif
#endif
#endif
#if (defined(__CH32V10x_H)) 
#if (W25Qxx_USE_HARDSPI)
#include "../myLib/CH32V_HARDSPI.h"
//CH32V103默认硬件SPI引脚PA5(SCK),PA7(MOSI),PA6(MISO)
#else
#include "../myLib/CH32V_SOFTSPI.h"
#endif
#endif
#if (defined(__haodausb_H)) 
#if (W25Qxx_USE_HARDSPI)
#include "../myLib/HDUSB_HARDSPI.h"
//TWEN32默认硬件引脚  PB3(SCK),PB5(MOSI),PB4(MISO)
#else
#include "../myLib/HDUSB_SOFTSPI.h"
#endif
#endif
#if (defined(TW_ASR_PRO)) 
#include "../myLib/asr_softspi.h"
#endif


//W25Qxx ID
#define     W25Q80 	            		0XEF13 	
#define     W25Q16 	            		0XEF14
#define     W25Q32 	            		0XEF15
#define     W25Q64 	            		0XEF16
#define     W25Q128	            		0XEF17

//W25Q64常用操作命令
#define W25Qxx_WriteEnable		0x06    //写使能
#define W25QXX_WRITE_STATUS_REG	0x01    //写状态1寄存器
#define W25Qxx_WriteDisable     0x04    //写禁止
#define W25QXX_POWER_DOWN		0xB9    //掉电模式
#define W25QXX_RELEASE_POWER_DOWN 0xAB  //唤醒
#define W25Qxx_ReadStatusReg1	0x05    //读状态寄存器1指令
#define W25Qxx_SectorErase		0x20    //扇区（4KB）擦除指令
#define W25Qxx_BlockErase	    0xD8    //块（64KB）擦除指令
#define W25Qxx_ChipErase        0xC7    //片擦除指令
#define W25Qxx_PageProgram		0x02    //页（256B）写入指令
#define W25Qxx_ReadData	        0x03    //读数据指令
#define W25Qxx_Powerdown        0xB9    //Powerdown指令
#define W25Qxx_DevId            0x90    //ID


class W25Qxx
{
public:
    W25Qxx(uint8_t cspin);
    W25Qxx(uint8_t cspin,uint8_t sckpin, uint8_t mosipin, uint8_t misopin);
    void begin();
    void select();
    void deselect();
    void Wait_Busy();
    uint8_t readStatusReg1();
    void writeStatusReg1(uint8_t status);
    void powerDown();
    void wakeUp();
    uint16_t readId();
    void enableWrite();
    void disableWrite();
    void read(uint32_t addr, uint8_t * buf, uint32_t size);
    void programPage(uint32_t  addr, uint8_t * buf, uint16_t size);
    void write_nocheck(uint32_t address, uint8_t *data, uint32_t size);
    void w25qxx_write(uint32_t address, uint8_t *data, uint32_t size);
    void eraseSector(uint32_t  NBsector);
    void eraseblock(uint32_t  NBsector);
    void eraseChip();

private:
    uint8_t _cspin;
    uint8_t _sckpin;
    uint8_t _mosipin;
    uint8_t _misopin;
    uint32_t FLASH_BYTE;
    uint16_t _W25Qxx_Id;
#if (W25Qxx_USE_HARDSPI)
    HARDSPI _SPI;
#else
    SOFTSPI _SPI;
#endif
    
    
};
/**
 * @brief Construct a new W25Qxx::W25Qxx object 使用硬件SPI
 * @param  cspin            片选引脚
 */
W25Qxx::W25Qxx(uint8_t cspin)
{
    _cspin = cspin;
}
/**
 * @brief Construct a new W25Qxx::W25Qxx object 使用软件SPI
 * @param  cspin            片选引脚
 * @param  sckpin           时钟引脚
 * @param  mosipin          输出引脚
 * @param  misopin          输入引脚
 */
W25Qxx::W25Qxx(uint8_t cspin,uint8_t sckpin, uint8_t mosipin, uint8_t misopin)
{
    _cspin = cspin;
    _sckpin = sckpin;
    _mosipin = mosipin;
    _misopin = misopin;
}
/**
 * @brief 初始化
 */
void W25Qxx::begin()
{
#if (W25Qxx_USE_HARDSPI)
    _SPI.begin();
#if (!defined(__haodausb_H)) 
    _SPI.set_mode(0,0);//SPI模式0
    _SPI.set_speed(4);//SPI分频
#endif
#else
    _SPI.begin(_sckpin,_mosipin,_misopin);
#endif
#if (defined(TW_ASR_PRO)) 
    pinMode(_cspin, output);
#else
    pinMode(_cspin, GPIO_Mode_Out_PP);
#endif
    deselect();
}
/**
 * @brief 片选选中
 */
void W25Qxx::select()
{
  digitalWrite(_cspin,0);
}
/**
 * @brief 取消片选
 */
void W25Qxx::deselect()
{
  digitalWrite(_cspin,1);
}

/**
 * @brief 读ID
 * @return uint16_t 
 * 
//0XEF13,表示芯片型号为W25Q80  
//0XEF14,表示芯片型号为W25Q16   
//0XEF15,表示芯片型号为W25Q32  
//0XEF16,表示芯片型号为W25Q64
//0XEF17,表示芯片型号为W25Q128
 */
uint16_t W25Qxx::readId()
{
    uint8_t temp;
    Wait_Busy();//等待空闲
    select();//CS有效信号
    _SPI.wr_data((uint8_t)W25Qxx_DevId);
    _SPI.wr_data(0);
    _SPI.wr_data(0);
    _SPI.wr_data(0);
    _W25Qxx_Id |= _SPI.wr_data(0xff)<<8;
    _W25Qxx_Id |= _SPI.wr_data(0xff);
    deselect();//CS无效信号
    switch (_W25Qxx_Id)
    {
    case W25Q80:
        FLASH_BYTE = ((1) * 1024 * 1024);
        break;
    case W25Q16:
        FLASH_BYTE = ((2) * 1024 * 1024);
        break;
    case W25Q32:
        FLASH_BYTE = ((4) * 1024 * 1024);
        break;
    case W25Q64:
        FLASH_BYTE = ((8) * 1024 * 1024);
        break;   
    case W25Q128:
        FLASH_BYTE = ((16) * 1024 * 1024);
        break; 
    default:
        break;
    }
    return _W25Qxx_Id;
}

/**
 * @brief 等待空闲
 */
void W25Qxx::Wait_Busy()
{
    uint8_t res;
	do{			    
		res = readStatusReg1(); 	                    //读取状态寄存器1   
        delay(1); 
	}while((res&0x01)==0x01);                       //等待BUSY位清空，表示空闲
}
/**
 * @brief 写使能
 */
void W25Qxx::enableWrite()
{
    Wait_Busy();//等待空闲
    select();
    _SPI.wr_data((uint8_t)W25Qxx_WriteEnable);
    deselect();
}
/**
 * @brief 写禁止
 */
void W25Qxx::disableWrite()
{
    Wait_Busy();//等待空闲
    select();
    _SPI.wr_data((uint8_t)W25Qxx_WriteDisable);
    deselect();
}
/**
 * @brief 写状态1寄存器
 * @param  status           状态
 */
void W25Qxx::writeStatusReg1(uint8_t status)
{
    Wait_Busy();//等待空闲
    select();
    _SPI.wr_data((uint8_t)W25QXX_WRITE_STATUS_REG);
    _SPI.wr_data(status);
    deselect();
}
/**
 * @brief 读取状态寄存器1
 * @return uint8_t 状态
 */
uint8_t W25Qxx::readStatusReg1()
{
    select();
    _SPI.wr_data((uint8_t)W25Qxx_ReadStatusReg1);
    uint8_t const status_reg_1 = _SPI.wr_data(0x00);
    deselect();
    return status_reg_1;
}
/**
 * @brief 进入掉电模式
 */
void W25Qxx::powerDown()
{
    Wait_Busy();//等待空闲
    select();
    _SPI.wr_data((uint8_t)W25QXX_POWER_DOWN);
    deselect();
    delay(1);//等待TPD
}
/**
 * @brief 唤醒W25Qxx
 */
void W25Qxx::wakeUp()
{
    Wait_Busy();//等待空闲
    select();
    _SPI.wr_data((uint8_t)W25QXX_RELEASE_POWER_DOWN);
    deselect();
    delay(1);//等待TRES1
}
/**
 * @brief 读数据
 * @param  addr             读取地址
 * @param  buf              存数据地址
 * @param  size             读取数量
 */
void W25Qxx::read(uint32_t  addr, uint8_t * buf, uint32_t size)
{
    Wait_Busy();//等待空闲
    select();
    _SPI.wr_data((uint8_t)W25Qxx_ReadData);
    _SPI.wr_data((uint8_t)(addr >> 16));   //发送24位地址 
    _SPI.wr_data((uint8_t)(addr >> 8));
    _SPI.wr_data((uint8_t)(addr >> 0));

    for(uint32_t bytes_read = 0; bytes_read < size; bytes_read++)
    {
        buf[bytes_read] = _SPI.wr_data(0);
    }
    deselect(); 
}
/**
 * @brief 页编程 写入最大256字节的数据. 超过页边界无效
 * @param  addr             页地址
 * @param  buf              数据地址
 * @param  size             大小
 */
void W25Qxx::programPage(uint32_t addr, uint8_t  * buf, uint16_t size)
{
    enableWrite();
    Wait_Busy();//等待空闲
    select();
    _SPI.wr_data((uint8_t)W25Qxx_PageProgram);
    _SPI.wr_data((uint8_t)(addr >> 16));
    _SPI.wr_data((uint8_t)(addr >> 8));
    _SPI.wr_data((uint8_t)(addr >> 0)); 
    for(uint32_t bytes_written = 0; bytes_written < size; bytes_written++)
    {
        _SPI.wr_data(buf[bytes_written]);
    }
    deselect(); 
}
/**
 * @brief 无检验写spiflash.必须确保所写的地址范围内的数据全部为0XFF.确保地址不越界
 * @param  address          开始写入的地址(24bit);
 * @param  data             数据存储区
 * @param  size             写入的字节数
 */
void W25Qxx::write_nocheck(uint32_t address, uint8_t *data, uint32_t size)
{ 
	uint16_t pageremain;	   
	pageremain=256-address%256; //单页剩余的字节数	 	    
	if(size<=pageremain)pageremain=size;//不大于256个字节
	while(1)
	{	   
		programPage(address,data,pageremain);
		if(size==pageremain)break;//写入结束了
	 	else 	
		{
			data+=pageremain;
			address+=pageremain;	
			size-=pageremain;			  //减去已经写入了的字节数
			if(size>256)pageremain=256; //一次可以写入256个字节
			else pageremain=size; 	  //不够256个字节了
		}
	}	
}
uint8_t W25QXX_BUFFER[4096];
/**
 * @brief 在指定地址开始写入指定长度的数据
 * @param  address          开始写入的地址
 * @param  data             数据存储区
 * @param  size             要写入的字节数
 */
void W25Qxx::w25qxx_write(uint32_t address, uint8_t *data, uint32_t size)
{
    uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;
	uint8_t* W25QXX_BUF = NULL; 
  	W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=address/4096;//扇区地址
	secoff=address%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小  
    if(size<=secremain)secremain=size;//不大于4096个字节
	while(1) 
	{	
		read(secpos*4096,W25QXX_BUF,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)
                break;//需要擦除	  
		}
		if(i<secremain)//需要擦除
		{
			eraseSector(secpos);	//擦除整个扇区
			for(i=0;i<secremain;i++)	    //复制
			{
				W25QXX_BUF[i+secoff] = data[i];	  
			}
			write_nocheck(secpos*4096,W25QXX_BUF,4096);//写入整个扇区
		}
        else 
        {
            write_nocheck(address,data,secremain);//写已经擦除了的，直接写入扇区剩余区间			   
        }
            
		if(size==secremain) break;//写入结束了
		else//写入未结束
		{ 
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	data+=secremain;  //地址偏移
			address+=secremain;//写地址偏移	   
		   	size-=secremain;				//字节数递减
			if(size>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=size;			//下一个扇区可以写完了
		}	 
	}   
}
/**
 * @brief W25Qxx擦除一个扇区（4KB）
 * @param  NBsector        第几个扇区 
 */
void W25Qxx::eraseSector(uint32_t  NBsector)
{
    enableWrite();//写使能 
    Wait_Busy();//等待空闲
    select();
    _SPI.wr_data((uint8_t)W25Qxx_SectorErase);
    _SPI.wr_data((uint8_t)((NBsector*4096) >> 16));
    _SPI.wr_data((uint8_t)((NBsector*4096) >> 8));
    _SPI.wr_data((uint8_t)((NBsector*4096) >> 0)); 
    deselect(); 
    Wait_Busy();//等待擦除完成
}
/**
 * @brief W25Qxx擦除一个块（64KB）
 * @param  NBsector         第几个块
 */
void W25Qxx::eraseblock(uint32_t  NBsector)
{
    enableWrite();//写使能 
    Wait_Busy();//等待空闲
    select();
    _SPI.wr_data((uint8_t)W25Qxx_BlockErase);
    _SPI.wr_data((uint8_t)((NBsector*16*4096) >> 16));
    _SPI.wr_data((uint8_t)((NBsector*16*4096) >> 8));
    _SPI.wr_data((uint8_t)((NBsector*16*4096) >> 0)); 
    deselect(); 
    Wait_Busy();//等待擦除完成
}
/**
 * @brief W25Qxx全擦除
 */
void W25Qxx::eraseChip()
{
    enableWrite();//写使能 
    Wait_Busy();//等待空闲
    select();
    _SPI.wr_data((uint8_t)W25Qxx_ChipErase);
    deselect(); 
    Wait_Busy();// 在这种情况下，请在此函数中等待，因为它非常耗时 */
}

#endif