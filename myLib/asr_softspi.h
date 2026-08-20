/**
 * @file asr_softspi.h
 * @brief 
 * @author LI (com)
 * @version 1.0
 * @date 2024-01-24
 * 
 * @copyright Copyright (c) 2024 haohaodada.com
 * 
 */
#ifndef _ASRPRO_SOFTSPI_H_
#define _ASRPRO_SOFTSPI_H_

#include "asr.h"
#ifndef TW_ASR_PRO
#include "asr_delay.h"
#endif


#define     SOFTSPI_CPHA 0
#define     SOFTSPI_CPOL 0

class SOFTSPI  {
public:
    void begin(uint8_t sckpin, uint8_t mosipin, uint8_t misopin); //SOFTSPI初始化
    uint8_t wr_data(uint8_t data); //读/写函数
    void write_data(uint8_t data); //写函数
    uint8_t read_data(); //读函数
private:

	uint8_t _sckpin;
	uint8_t _mosipin;
	uint8_t _misopin;

	volatile uint32_t * _scl_reg = NULL;
	volatile uint32_t * _mosi_reg = NULL;
	volatile uint32_t * _miso_reg = NULL;   

	uint8_t _SCL_GPIO_PIN;
	uint8_t _MOSI_GPIO_PIN;
	uint8_t _MISO_GPIO_PIN;

    inline void SCL_LOW(void)		
	{
    	*_scl_reg = 0x00;
    }
    inline void SCL_HIGH(void)		
	{
    	*_scl_reg = _SCL_GPIO_PIN;
    }
    inline void MOSI_LOW(void)		
	{
    	*_mosi_reg = 0x00;
    }
    inline void MOSI_HIGH(void)		
	{
    	*_mosi_reg = _MOSI_GPIO_PIN;
    }

    inline uint8_t MISO_STATUS(void)		
	{
    	return ((*_miso_reg) &_MISO_GPIO_PIN);
    }
};
void SOFTSPI::begin(uint8_t sckpin, uint8_t mosipin, uint8_t misopin)
{
	this->_sckpin = sckpin;
	this->_mosipin = mosipin;
	this->_misopin = misopin;

	_SCL_GPIO_PIN = 1<<(_sckpin%8);
	_MOSI_GPIO_PIN = 1<<(_mosipin%8);
	_MISO_GPIO_PIN = 1<<(_misopin%8);

    if(_sckpin<16)
    {
        _scl_reg = (uint32_t*)(HAL_PA_BASE + 0X1000 * (_sckpin/8)) + _SCL_GPIO_PIN;
    }
    else if(_sckpin<24)
    {
        _scl_reg = (uint32_t*)(HAL_PC_BASE )+_SCL_GPIO_PIN ;
    }else
    {
        _scl_reg = (uint32_t*)(HAL_PD_BASE )+_SCL_GPIO_PIN ;
    }

    if(_mosipin<16)
    {
        _mosi_reg = (uint32_t*)(HAL_PA_BASE + 0X1000 * (_mosipin/8)) + _MOSI_GPIO_PIN;
    }
    else if(_mosipin<24)
    {
        _mosi_reg = (uint32_t*)(HAL_PC_BASE )+_MOSI_GPIO_PIN ;
    }else
    {
        _mosi_reg = (uint32_t*)(HAL_PD_BASE )+_MOSI_GPIO_PIN ;
    }

    if(_misopin<16)
    {
        _miso_reg = (uint32_t*)(HAL_PA_BASE + 0X1000 * (_misopin/8)) + _MISO_GPIO_PIN;
    }
    else if(_misopin<24)
    {
        _miso_reg = (uint32_t*)(HAL_PC_BASE )+_MISO_GPIO_PIN ;
    }else
    {
        _miso_reg = (uint32_t*)(HAL_PD_BASE )+_MISO_GPIO_PIN ;
    }

    set_pin_to_gpio_mode(_sckpin);
    set_pin_to_gpio_mode(_mosipin);
    set_pin_to_gpio_mode(_misopin);

    pinMode(_sckpin, output);
    SCL_LOW();
    pinMode(_mosipin, output);
    MOSI_HIGH();
    pinMode(_misopin, input);
    dpmu_set_io_pull(pinToFun[_misopin],DPMU_IO_PULL_UP);//MISO设置为上拉
    
}

//========================================================================
// 描述: 读写函数.
// 参数: sckpin:sck引脚; mosipin: mosi引脚; misopin：miso引脚.
// 返回: 读到的数据.
//========================================================================
uint8_t SOFTSPI::wr_data(uint8_t data)
{
    uint8_t i;
    uint8_t datavalue=0;
    uint8_t wr_data = data;
#if SOFTSPI_CPHA==0 && SOFTSPI_CPOL==0  
    for(i=0;i<8;i++){
        if(wr_data&0x80){                         //慢速2Mhz频率
            digitalWrite(_mosipin,1);  //MOSI拉高
        }else{
            digitalWrite(_mosipin,0);   //MOSI拉低
        }
        wr_data <<=1;
        digitalWrite(_sckpin,1);  //SCK拉高
        datavalue <<=1;
        if( (digitalRead(_misopin)) ){
            datavalue |= 0x01;
        }
        digitalWrite(_sckpin,0);   //SCK拉低
    }
    // for(i=0;i<8;i++){                           //快速6Mhz频率
    //     if(wr_data&0x80){
    //         MOSI_HIGH();  //MOSI拉高
    //     }else{
    //         MOSI_LOW();   //MOSI拉低
    //     }
    //     wr_data <<=1;
    //     SCL_HIGH();  //SCK拉高
    //     datavalue <<=1;
    //     if( (MISO_STATUS()) ){
    //         datavalue |= 0x01;
    //     }
    //     SCL_LOW();  //SCK拉低
    // }
    
#elif SOFTSPI_CPHA==0 && SOFTSPI_CPOL==1
    for(i=0;i<8;i++){
        if(wr_data&0x80){
            MOSI_HIGH();  //MOSI拉高
        }else{
            MOSI_LOW();  //MOSI拉低
        }
        wr_data <<=1;
        SCL_LOW();  //SCK拉低
        datavalue <<=1;
        if( (MISO_STATUS()) ){
            datavalue |= 0x01;
        }
        SCL_HIGH();  //SCK拉高
        //delay(1);
    }  
    
#elif SOFTSPI_CPHA==1 && SOFTSPI_CPOL==0
    for(i=0;i<8;i++){
        SCL_HIGH();  //SCK拉高
        if(wr_data&0x80){
            MOSI_HIGH();  //MOSI拉高
        }else{
            MOSI_LOW();  //MOSI拉低
        }
        wr_data <<=1;
        SCL_LOW();  //SCK拉低
        datavalue <<=1;
        if( (MISO_STATUS()) ){
            datavalue |= 0x01;
        }
        //delay(1);
    } 
    
#elif SOFTSPI_CPHA==1 && SOFTSPI_CPOL==1
    for(i=0;i<8;i++){
        SCL_LOW();  //SCK拉低
        if(wr_data&0x80){
            MOSI_HIGH();  //MOSI拉高
        }else{
            MOSI_LOW();  //MOSI拉低
        }
        wr_data <<=1;
        SCL_HIGH();  //SCK拉高
        datavalue <<=1;
        if( (MISO_STATUS()) ){
            datavalue |= 0x01;
        }
        SCL_HIGH();  //SCK拉高
        // delay(1);
    } 
#endif
    return datavalue;
}

//========================================================================
// 描述: 写函数.
// 参数: data：数据.
// 返回: none.
//========================================================================
void SOFTSPI::write_data(uint8_t data)
{
    wr_data(data);
}

//========================================================================
// 描述: 读函数.
// 参数: none.
// 返回: 读到的数据.
//========================================================================
uint8_t SOFTSPI::read_data()
{
    return wr_data(0xff);
}
#endif