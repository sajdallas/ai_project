#ifndef __TEXT_H__
#define __TEXT_H__	 
#include <stm32f4xx.h>
#include "fontupd.h"
//////////////////////////////////////////////////////////////////////////////////	 
 
//������ʾ ��������	   
//STM32F4����ģ��-�⺯���汾
//�Ա����̣�http://mcudev.taobao.com									  
////////////////////////////////////////////////////////////////////////////////// 	 
 					     
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size);			//�õ����ֵĵ�����
void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode);					//��ָ��λ����ʾһ������
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode);	//��ָ��λ����ʾһ���ַ��� 
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len);
void Show_Str_UTF8(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode);
#endif
