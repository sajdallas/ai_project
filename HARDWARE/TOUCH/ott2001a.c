#include "ott2001a.h"
#include "touch.h"
#include "ctiic.h"
#include "usart.h"
#include "delay.h" 
//////////////////////////////////////////////////////////////////////////////////	 

//4.3����ݴ�����-OTT2001A ��������	   
//STM32F4����ģ��-�⺯���汾
								  
////////////////////////////////////////////////////////////////////////////////// 


//��OTT2001Aд��һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�������
//len:д���ݳ���
//����ֵ:0,�ɹ�;1,ʧ��.
u8 OTT2001A_WR_Reg(u16 reg,u8 *buf,u8 len)
{
	u8 i;
	u8 ret=0;
	CT_IIC_Start();	
 	CT_IIC_Send_Byte(OTT_CMD_WR);   //����д���� 	 
	CT_IIC_Wait_Ack();
	CT_IIC_Send_Byte(reg>>8);   	//���͸�8λ��ַ
	CT_IIC_Wait_Ack(); 	 										  		   
	CT_IIC_Send_Byte(reg&0XFF);   	//���͵�8λ��ַ
	CT_IIC_Wait_Ack();  
	for(i=0;i<len;i++)
	{	   
    	CT_IIC_Send_Byte(buf[i]);  	//������
		ret=CT_IIC_Wait_Ack();
		if(ret)break;  
	}
    CT_IIC_Stop();					//����һ��ֹͣ����	    
	return ret; 
}
//��OTT2001A����һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�������
//len:�����ݳ���			  
void OTT2001A_RD_Reg(u16 reg,u8 *buf,u8 len)
{
	u8 i; 
 	CT_IIC_Start();	
 	CT_IIC_Send_Byte(OTT_CMD_WR);   //����д���� 	 
	CT_IIC_Wait_Ack();
 	CT_IIC_Send_Byte(reg>>8);   	//���͸�8λ��ַ
	CT_IIC_Wait_Ack(); 	 										  		   
 	CT_IIC_Send_Byte(reg&0XFF);   	//���͵�8λ��ַ
	CT_IIC_Wait_Ack();  
 	CT_IIC_Start();  	 	   
	CT_IIC_Send_Byte(OTT_CMD_RD);   //���Ͷ�����		   
	CT_IIC_Wait_Ack();	   
	for(i=0;i<len;i++)
	{	   
    	buf[i]=CT_IIC_Read_Byte(i==(len-1)?0:1); //������	  
	} 
    CT_IIC_Stop();//����һ��ֹͣ����    
}
//��������/�رղ���
//cmd:1,�򿪴�����;0,�رմ�����
void OTT2001A_SensorControl(u8 cmd)
{
	u8 regval=0X00;
	if(cmd)regval=0X80;
	OTT2001A_WR_Reg(OTT_CTRL_REG,&regval,1); 
} 
//��ʼ��������
//����ֵ:0,��ʼ���ɹ�;1,��ʼ��ʧ�� 
// 初始化触摸屏
// 返回值:0,初始化成功;1,初始化失败 
u8 OTT2001A_Init(void)
{
	u8 regval=0; 
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
    /* ====== HAL 库版本：时钟使能 ====== */
	__HAL_RCC_GPIOB_CLK_ENABLE();   // 使能 PB 时钟 (INT)
	__HAL_RCC_GPIOC_CLK_ENABLE();   // 使能 PC 时钟 (RST)

    /* ====== HAL 库版本：PB1 (OTT INT) 配置为输入 ====== */
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;        // 输入模式
	GPIO_InitStruct.Pull = GPIO_PULLUP;            // 上拉
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
    /* ====== HAL 库版本：PC13 (OTT RST) 配置为输出 ====== */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;     // 推挽输出
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	CT_IIC_Init();      	// 初始化电容触摸屏I2C总线  
	OTT_RST=0;				// 复位
	delay_ms(100);
	OTT_RST=1;				// 释放复位		    
	delay_ms(100); 
	OTT2001A_SensorControl(1);	// 打开传感器 
	OTT2001A_RD_Reg(OTT_CTRL_REG,&regval,1);//读取传感器控制寄存器值,判断I2C通信是否正常.
	// printf("CTP ID:%x\r\n",regval);  // 已注释，避免依赖 usart.h
	if(regval==0x80)return 0;
	return 1;
}

const u16 OTT_TPX_TBL[5]={OTT_TP1_REG,OTT_TP2_REG,OTT_TP3_REG,OTT_TP4_REG,OTT_TP5_REG};
//ɨ�败����(���ò�ѯ��ʽ)
//mode:0,����ɨ��.
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
u8 OTT2001A_Scan(u8 mode)
{
	u8 buf[4];
	u8 i=0;
	u8 res=0;
	static u8 t=0;//���Ʋ�ѯ���,�Ӷ�����CPUռ����   
	t++;
	if((t%10)==0||t<10)//����ʱ,ÿ����10��CTP_Scan�����ż��1��,�Ӷ���ʡCPUʹ����
	{
		OTT2001A_RD_Reg(OTT_GSTID_REG,&mode,1);//��ȡ�������״̬  
		if(mode&0X1F)
		{
			tp_dev.sta=(mode&0X1F)|TP_PRES_DOWN|TP_CATH_PRES;
			for(i=0;i<5;i++)
			{
				if(tp_dev.sta&(1<<i))	//������Ч?
				{
					OTT2001A_RD_Reg(OTT_TPX_TBL[i],buf,4);	//��ȡXY����ֵ
					if(tp_dev.touchtype&0X01)//����
					{
						tp_dev.y[i]=(((u16)buf[2]<<8)+buf[3])*OTT_SCAL_Y;
						tp_dev.x[i]=800-((((u16)buf[0]<<8)+buf[1])*OTT_SCAL_X);
					}else
					{
						tp_dev.x[i]=(((u16)buf[2]<<8)+buf[3])*OTT_SCAL_Y;
						tp_dev.y[i]=(((u16)buf[0]<<8)+buf[1])*OTT_SCAL_X;
					}  
					//printf("x[%d]:%d,y[%d]:%d\r\n",i,tp_dev.x[i],i,tp_dev.y[i]);
				}			
			} 
			res=1;
			if(tp_dev.x[0]==0 && tp_dev.y[0]==0)mode=0;	//���������ݶ���0,����Դ˴�����
			t=0;		//����һ��,��������������10��,�Ӷ����������
		}
	}
	if((mode&0X1F)==0)//�޴����㰴��
	{ 
		if(tp_dev.sta&TP_PRES_DOWN)	//֮ǰ�Ǳ����µ�
		{
			tp_dev.sta&=~(1<<7);	//��ǰ����ɿ�
		}else						//֮ǰ��û�б�����
		{ 
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
			tp_dev.sta&=0XE0;	//�������Ч���	
		}	 
	} 	
	if(t>240)t=10;//���´�10��ʼ����
	return res;
}
 































 





























