#ifndef __CAN_H
#define __CAN_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F746������
//CAN��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//CAN1����RX0�ж�ʹ��
#define CAN1_RX0_INT_ENABLE	0		//0,��ʹ��;1,ʹ��.
#define abs(x) ((x)>0? (x):(-(x)))

#define OpenLoop_Mode                       0x01  //����ģʽ
#define Current_Mode                        0x02  //����ģʽ
#define Velocity_Mode                       0x03  //�ٶ�ģʽ
#define Position_Mode                       0x04  //����ģʽ
#define Velocity_Position_Mode              0x05  //����ģʽ
#define Current_Velocity_Mode               0x06  //����ģʽ
#define Current_Position_Mode               0x07  //����ģʽ
#define Current_Velocity_Position_Mode      0x08  //����ģʽ

u8 CAN1_Mode_Init(u32 tsjw,u32 tbs2,u32 tbs1,u16 brp,u32 mode);//CAN��ʼ��
u8 CAN1_Send_Msg(u8* msg,u8 len);						//��������
u8 CAN1_Receive_Msg(u8 *buf);							//��������
u8 CAN1_ID_Send_Msg(uint32_t id,u8* msg,u8 len);//���ͺ��� ��д��id
u8 CAN1_ID_Receive_Msg(u8* buf,uint32_t* stdidbuf);


void CAN_RoboModule_DRV_Reset(unsigned char Group,unsigned char Number);
void CAN_RoboModule_DRV_Mode_Choice(unsigned char Group,unsigned char Number,unsigned char Mode);
void CAN_RoboModule_DRV_Velocity_Mode(unsigned char Group,unsigned char Number,short Temp_PWM,short Temp_Velocity);
void CAN_RoboModule_DRV_Config(unsigned char Group,unsigned char Number,unsigned char Temp_Time,unsigned char Ctl1_Ctl2);
void CAN_RoboModule_DRV_Online_Check(unsigned char Group,unsigned char Number);
void CAN_RoboModule_DRV_OpenLoop_Mode(unsigned char Group,unsigned char Number,short Temp_PWM);

#endif
