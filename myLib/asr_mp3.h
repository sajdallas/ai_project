#ifndef _mp3_
#define _mp3_ 

#include "asr.h"
#include "HardwareSerial.h"

uint8_t mingling[8]={0x7E,0xFF,0x06,0x00,0x01,0x00,0x00,0xEF};
uint8_t rxbuf[10];
uint8_t Count,Value,RecFlag;
uint8_t MP3_wie_cmd(uint8_t CMD, uint8_t DEL, int16_t dat);
uint8_t MP3_wie1_cmd(uint8_t CMD, uint8_t DEL, uint8_t wjj, int16_t dat);
uint8_t UART_BUF();


//**************************************************************//
//功能：向mp3写入数据
//参数：1命令(cmd)2返回(DEL)3数据(dat)
//**************************************************************//
uint8_t MP3_wie_cmd(uint8_t mun,uint8_t CMD,uint8_t DEL,int16_t dat){
  mingling[(int)(3)] = CMD;
  mingling[(int)(4)] = DEL;
  mingling[(int)(5)] = (dat>>8);
  mingling[(int)(6)] = dat;
  return(mingling[mun]);
}
//**************************************************************//
//功能：mp3播放指定文件夹指定歌曲
//参数：1命令(cmd)2返回(DEL)3数据(dat)
//**************************************************************//
uint8_t MP3_wie1_cmd(uint8_t mun,uint8_t CMD, uint8_t DEL, uint8_t wjj, int16_t dat){
  mingling[(int)(3)] = CMD;
  mingling[(int)(4)] = DEL;
  mingling[(int)(5)] = ((wjj<<4)|(dat>>8));
  mingling[(int)(6)] = dat;
  return(mingling[mun]);                                                              
}
//**************************************************************//
//功能：mp3接收返回命令
//参数：接收完成返回1
//**************************************************************//
uint8_t UART_BUF(uint8_t available,uint8_t uartrx){
  if(available > 0){
    if(RecFlag == 0){
      Value = uartrx;
      if(Count == 0){
        if(Value == 0x7E){
          rxbuf[(int)(Count)] = Value;
          Count = Count + 1;
        }
        else{
          Count = 0;
        }
      }
      else{
        rxbuf[(int)(Count)] = Value;
        Count = Count + 1;
        if(Count > 9){
          Count = 0;
          RecFlag = 1;
        }
      }
    }
  }
  return RecFlag;
}
void UART_QC(){
	RecFlag = 0;
}
uint8_t UART_DQ(uint8_t mun){
	RecFlag = 0;
	return rxbuf[mun];
}

#endif