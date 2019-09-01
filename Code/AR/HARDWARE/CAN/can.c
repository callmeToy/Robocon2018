#include "can.h"
#include "usart.h"
#include "delay.h"
#include "led.h"





CAN_HandleTypeDef   CAN1_Handler;   //CAN1���
CanTxMsgTypeDef     TxMessage;      //������Ϣ
CanRxMsgTypeDef     RxMessage;      //������Ϣ

unsigned int CAN_Time_Out = 0;
unsigned char can_tx_success_flag = 0;


short Real_Current_Value[4] = {0};
short Real_Velocity_Value[4] = {0};
long Real_Position_Value[4] = {0};
char Real_Online[4] = {0};
char Real_Ctl1_Value[4] = {0};
char Real_Ctl2_Value[4] = {0};

static void CAN_Delay_Us(unsigned int t)
{
	int i;
	for(i=0;i<t;i++)
	{
		int a=40;
		while(a--);
	}
}

////CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1TQ~CAN_SJW_4TQ
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1TQ~CAN_BS2_8TQ;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1TQ~CAN_BS1_16TQ
//brp :�����ʷ�Ƶ��.��Χ:1~1024; tq=(brp)*tpclk1
//������=Fpclk1/((tbs1+tbs2+1)*brp); ����tbs1��tbs2����ֻ�ù�ע��ʶ���ϱ�־����ţ�����CAN_BS2_1TQ�����Ǿ���Ϊtbs2=1�����㼴�ɡ�
//mode:CAN_MODE_NORMAL,��ͨģʽ;CAN_MODE_LOOPBACK,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ45M,�������CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_8tq,6,CAN_MODE_LOOPBACK);
//������Ϊ:45M/((6+8+1)*6)=500Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��; 

u8 CAN1_Mode_Init(u32 tsjw,u32 tbs2,u32 tbs1,u16 brp,u32 mode)
{
    CAN_FilterConfTypeDef  CAN1_FilerConf;
    
    CAN1_Handler.Instance=CAN1; 
    CAN1_Handler.pTxMsg=&TxMessage;     //������Ϣ
    CAN1_Handler.pRxMsg=&RxMessage;     //������Ϣ
    CAN1_Handler.Init.Prescaler=brp;    //��Ƶϵ��(Fdiv)Ϊbrp+1
    CAN1_Handler.Init.Mode=mode;        //ģʽ���� 
    CAN1_Handler.Init.SJW=tsjw;         //����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1TQ~CAN_SJW_4TQ
    CAN1_Handler.Init.BS1=tbs1;         //tbs1��ΧCAN_BS1_1TQ~CAN_BS1_16TQ
    CAN1_Handler.Init.BS2=tbs2;         //tbs2��ΧCAN_BS2_1TQ~CAN_BS2_8TQ
    CAN1_Handler.Init.TTCM=DISABLE;     //��ʱ�䴥��ͨ��ģʽ 
    CAN1_Handler.Init.ABOM=DISABLE;     //����Զ����߹���
    CAN1_Handler.Init.AWUM=DISABLE;     //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
    CAN1_Handler.Init.NART=DISABLE;      //��ֹ�����Զ����� 
    CAN1_Handler.Init.RFLM=DISABLE;     //���Ĳ�����,�µĸ��Ǿɵ� 
    CAN1_Handler.Init.TXFP=ENABLE;     //���ȼ��ɱ��ı�ʶ������ 
	
    if(HAL_CAN_Init(&CAN1_Handler)!=HAL_OK) return 1;   //��ʼ��
    
    CAN1_FilerConf.FilterIdHigh=0X0000;     //32λID
    CAN1_FilerConf.FilterIdLow=0X0000;
    CAN1_FilerConf.FilterMaskIdHigh=0X0000; //32λMASK
    CAN1_FilerConf.FilterMaskIdLow=0X0000;  
    CAN1_FilerConf.FilterFIFOAssignment=CAN_FILTER_FIFO0;//������0������FIFO0
    CAN1_FilerConf.FilterNumber=0;          //������0
    CAN1_FilerConf.FilterMode=CAN_FILTERMODE_IDMASK;
    CAN1_FilerConf.FilterScale=CAN_FILTERSCALE_32BIT;
    CAN1_FilerConf.FilterActivation=ENABLE; //�����˲���0
    CAN1_FilerConf.BankNumber=14;
	
    if(HAL_CAN_ConfigFilter(&CAN1_Handler,&CAN1_FilerConf)!=HAL_OK) return 2;//�˲�����ʼ��
	
    return 0;
}

//CAN�ײ��������������ã�ʱ�����ã��ж�����
//�˺����ᱻHAL_CAN_Init()����
//hcan:CAN���
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_CAN1_CLK_ENABLE();                //ʹ��CAN1ʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();			    //����GPIOAʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_11|GPIO_PIN_12;   //PA11,12
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //���츴��
    GPIO_Initure.Pull=GPIO_PULLUP;              //����
    GPIO_Initure.Speed=GPIO_SPEED_FAST;         //����
    GPIO_Initure.Alternate=GPIO_AF9_CAN1;       //����ΪCAN1
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //��ʼ��
    
#if CAN1_RX0_INT_ENABLE
    __HAL_CAN_ENABLE_IT(&CAN1_Handler,CAN_IT_FMP0);//FIFO0��Ϣ�����ж�����.	  
    //CAN1->IER|=1<<1;		//FIFO0��Ϣ�����ж�����.	
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn,1,2);    //��ռ���ȼ�1�������ȼ�2
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);          //ʹ���ж�
#endif	
}

/*************************************************************************
                          CAN1_TX_IRQHandler
������CAN1�ķ����жϺ���
*************************************************************************/
void CAN1_TX_IRQHandler(void)
{
  if(__HAL_CAN_GET_IT_SOURCE(&CAN1_Handler,CAN_IT_TME)!= RESET) 
	{
	   __HAL_CAN_CLEAR_FLAG(&CAN1_Handler,CAN_IT_TME);
      can_tx_success_flag=1;
   }
}

/****************************************************************************************
                                       ��λָ��
Group   ȡֵ��Χ 0-7
Number  ȡֵ��Χ 0-15������Number==0ʱ��Ϊ�㲥����
*****************************************************************************************/
void CAN_RoboModule_DRV_Reset(unsigned char Group,unsigned char Number)
{
//	printf("enter reset!!!\r\n");
	u8  res;
  u8 resetbuffer[8] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
  unsigned short can_id = 0x000;  
  
    if((Group<=7)&&(Number<=15))
    {
        can_id |= Group<<8;
        can_id |= Number<<4;
    }
    else
    {
        return;
    }
  can_tx_success_flag = 0;
	res = CAN1_ID_Send_Msg(can_id,resetbuffer,8);
//		printf("res:%d\r\n",res);
	CAN_Time_Out = 0;
	while(can_tx_success_flag == 0)
	{
			CAN_Delay_Us(1);
			CAN_Time_Out++;
			if(CAN_Time_Out>100)
			{
//				  printf("fuwei chaoshi tuichu!!!\r\n");
					break;
			}
	}
}
/****************************************************************************************
                                     ģʽѡ��ָ��
Group   ȡֵ��Χ 0-7
Number  ȡֵ��Χ 0-15������Number==0ʱ��Ϊ�㲥����

Mode    ȡֵ��Χ

OpenLoop_Mode                       0x01
Current_Mode                        0x02
Velocity_Mode                       0x03
Position_Mode                       0x04
Velocity_Position_Mode              0x05
Current_Velocity_Mode               0x06
Current_Position_Mode               0x07
Current_Velocity_Position_Mode      0x08
*****************************************************************************************/
void CAN_RoboModule_DRV_Mode_Choice(unsigned char Group,unsigned char Number,unsigned char Mode)
{
    unsigned short can_id = 0x001;
    u8 buffer[8] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
    if((Group<=7)&&(Number<=15))
    {
        can_id |= Group<<8;
        can_id |= Number<<4;
    }
    else
    {
        return;
    }
    buffer[0] = Mode;
		can_tx_success_flag = 0;
    CAN1_ID_Send_Msg(can_id,buffer,8);
    
    CAN_Time_Out = 0;
    while(can_tx_success_flag == 0)
    {
        CAN_Delay_Us(1);
        CAN_Time_Out++;
        if(CAN_Time_Out>100)
        {
            break;
        }
    }
}

/****************************************************************************************
                                   �ٶ�ģʽ�µ�����ָ��
Group   ȡֵ��Χ 0-7

Number  ȡֵ��Χ 0-15������Number==0ʱ��Ϊ�㲥����

temp_pwm��ȡֵ��Χ���£�
0 ~ +5000����ֵ5000������temp_pwm = 5000ʱ����������ѹΪ��Դ��ѹ

temp_velocity��ȡֵ��Χ���£�
-32768 ~ +32767����λRPM

*****************************************************************************************/
void CAN_RoboModule_DRV_Velocity_Mode(unsigned char Group,unsigned char Number,short Temp_PWM,short Temp_Velocity)
{
    unsigned short can_id = 0x004;
		u8 buffer[8] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
    
    if((Group<=7)&&(Number<=15))
    {
        can_id |= Group<<8;
        can_id |= Number<<4;
    }
    else
    {
        return;
    }
    

    if(Temp_PWM > 5000)
    {
        Temp_PWM = 5000;
    }
    else if(Temp_PWM < -5000)
    {
        Temp_PWM = -5000;
    }
    
    if(Temp_PWM < 0)
    {
        Temp_PWM = abs(Temp_PWM);
    }
    
    buffer[0] = (unsigned char)((Temp_PWM>>8)&0xff);
    buffer[1] = (unsigned char)(Temp_PWM&0xff);
    buffer[2] = (unsigned char)((Temp_Velocity>>8)&0xff);
    buffer[3] = (unsigned char)(Temp_Velocity&0xff);
    
    can_tx_success_flag = 0;
    CAN1_ID_Send_Msg(can_id,buffer,8);
    
    CAN_Time_Out = 0;
    while(can_tx_success_flag == 0)
    {
        CAN_Delay_Us(1);
        CAN_Time_Out++;
        if(CAN_Time_Out>100)
        {
            break;
        }
    }
}

/****************************************************************************************
                                   ����ģʽ�µ�����ָ��
Group   ȡֵ��Χ 0-7

Number  ȡֵ��Χ 0-15������Number==0ʱ��Ϊ�㲥����

temp_pwm��ȡֵ��Χ���£�
0 ~ +5000����ֵ5000������temp_pwm = ��5000ʱ����������ѹΪ��Դ��ѹ

*****************************************************************************************/
void CAN_RoboModule_DRV_OpenLoop_Mode(unsigned char Group,unsigned char Number,short Temp_PWM)
{
    unsigned short can_id = 0x002;
		u8 buffer[8] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
    
    
    if((Group<=7)&&(Number<=15))
    {
        can_id |= Group<<8;
        can_id |= Number<<4;
    }
    else
    {
        return;
    }
    

    if(Temp_PWM > 5000)
    {
        Temp_PWM = 5000;
    }
    else if(Temp_PWM < -5000)
    {
        Temp_PWM = -5000;
    }
    
    buffer[0] = (unsigned char)((Temp_PWM>>8)&0xff);
    buffer[1] = (unsigned char)(Temp_PWM&0xff);
		
    can_tx_success_flag = 0;
    CAN1_ID_Send_Msg(can_id,buffer,8);
    
    CAN_Time_Out = 0;
    while(can_tx_success_flag == 0)
    {
        CAN_Delay_Us(1);
        CAN_Time_Out++;
        if(CAN_Time_Out>100)
        {
//					  printf("kaihuan chaoshi tuichu\r\n");
            break;
        }
    }
}

/****************************************************************************************
                                      ����ָ��
Temp_Time��ȡֵ��Χ: 0 ~ 255��Ϊ0ʱ��Ϊ�رյ����ٶ�λ�÷�������
Ctl1_Ctl2��ȡֵ��Χ��0 or 1 ������Ϊ0 or 1������Ϊ��0��Ϊ�ر�������λ��⹦��
�ر���ʾ��Ctl1��Ctl2�Ĺ��ܽ�������102 301������汾��������Ctl1_Ctl2 = 0 ����
*****************************************************************************************/
void CAN_RoboModule_DRV_Config(unsigned char Group,unsigned char Number,unsigned char Temp_Time,unsigned char Ctl1_Ctl2)
{
    unsigned short can_id = 0x00A;
    u8 buffer[8] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
    
    if((Group<=7)&&(Number<=15))
    {
        can_id |= Group<<8;
        can_id |= Number<<4;
    }
    else
    {
        return;
    }
    
    if((Ctl1_Ctl2 != 0x00)&&(Ctl1_Ctl2 != 0x01))
    {
        Ctl1_Ctl2 = 0x00;
    }
    
    
    buffer[0] = Temp_Time;
    buffer[1] = Ctl1_Ctl2;
    
    can_tx_success_flag = 0;
    CAN1_ID_Send_Msg(can_id,buffer,8);
    
    CAN_Time_Out = 0;
    while(can_tx_success_flag == 0)
    {
        CAN_Delay_Us(1);
        CAN_Time_Out++;
        if(CAN_Time_Out>100)
        {
//					  printf("peizhi chaoshi tuichu\r\n");
            break;
        }
    }
}

/****************************************************************************************
                                      ���߼��
*****************************************************************************************/
void CAN_RoboModule_DRV_Online_Check(unsigned char Group,unsigned char Number)
{
    unsigned short can_id = 0x00F;
		u8 buffer[8] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
    
    if((Group<=7)&&(Number<=15))
    {
        can_id |= Group<<8;
        can_id |= Number<<4;
    }
    else
    {
        return;
    }
    
    can_tx_success_flag = 0;
    CAN1_ID_Send_Msg(can_id,buffer,8);
    
    CAN_Time_Out = 0;
    while(can_tx_success_flag == 0)
    {
        CAN_Delay_Us(1);
        CAN_Time_Out++;
        if(CAN_Time_Out>100)
        {
            break;
        }
    }
}


//���������ݵĺ�����Ĭ��Ϊ4����������������0�飬���Ϊ1��2��3��4
/*************************************************************************
                          CAN1_RX0_IRQHandler
������CAN1�Ľ����жϺ���
*************************************************************************/
void CAN1_RX0_IRQHandler(void)
{
//    CanRxMsgTypeDef rx_message;
	  uint32_t stdidbuf[1];
		  u8      buf[8];
//  if(__HAL_CAN_GET_IT_SOURCE(&CAN1_Handler,CAN_IT_TME)!= RESET) 
//	{
//	   __HAL_CAN_CLEAR_FLAG(&CAN1_Handler,CAN_IT_TME);
//      can_tx_success_flag=1;
//   }    
    if (__HAL_CAN_GET_IT_SOURCE(&CAN1_Handler,CAN_IT_FMP0)!= RESET)
	{
        __HAL_CAN_CLEAR_FLAG(&CAN1_Handler,CAN_IT_FMP0);
//    if(HAL_CAN_Receive(&CAN1_Handler,CAN_FIFO0,0)!=HAL_OK) return 0;//�������ݣ���ʱʱ������Ϊ0	
        CAN1_ID_Receive_Msg( buf, stdidbuf);

				if(stdidbuf == (u32 *)0x1B)
				{
						Real_Current_Value[0] = (buf[0]<<8)|(buf[1]);
						Real_Velocity_Value[0] = (buf[2]<<8)|(buf[3]);
						Real_Position_Value[0] = ((buf[4]<<24)|(buf[5]<<16)|(buf[6]<<8)|(buf[7]));
				}
				else if(stdidbuf== (u32 *)0x2B)
				{
						Real_Current_Value[1] = (buf[0]<<8)|(buf[1]);
						Real_Velocity_Value[1] = (buf[2]<<8)|(buf[3]);
						Real_Position_Value[1] = ((buf[4]<<24)|(buf[5]<<16)|(buf[6]<<8)|(buf[7]));
				}
				else if(stdidbuf== (u32 *)0x3B)
				{
						Real_Current_Value[2] = (buf[0]<<8)|(buf[1]);
						Real_Velocity_Value[2] = (buf[2]<<8)|(buf[3]);
						Real_Position_Value[2] = ((buf[4]<<24)|(buf[5]<<16)|(buf[6]<<8)|(buf[7]));
				}
				else if(stdidbuf== (u32 *)0x4B)
				{
						Real_Current_Value[3] = (buf[0]<<8)|(buf[1]);
						Real_Velocity_Value[3] = (buf[2]<<8)|(buf[3]);
						Real_Position_Value[3] = ((buf[4]<<24)|(buf[5]<<16)|(buf[6]<<8)|(buf[7]));
				}
				else if(stdidbuf== (u32 *)0x1F)
				{
						Real_Online[0] = 1;
				}
				else if(stdidbuf== (u32 *)0x2F)
				{
						Real_Online[1] = 1;
				}
				else if(stdidbuf== (u32 *)0x3F)
				{
						Real_Online[2] = 1;
				}
				else if(stdidbuf== (u32 *)0x4F)
				{
						Real_Online[3] = 1;
				}
				else if(stdidbuf== (u32 *)0x1C)
				{
						Real_Ctl1_Value[0] = buf[0];
						Real_Ctl2_Value[0] = buf[1];
				}
				else if(stdidbuf== (u32 *)0x2C)
				{
						Real_Ctl1_Value[1] = buf[0];
						Real_Ctl2_Value[1] = buf[1];
				}
				else if(stdidbuf== (u32 *)0x3C)
				{
						Real_Ctl1_Value[2] = buf[0];
						Real_Ctl2_Value[2] = buf[1];
				}
				else if(stdidbuf== (u32 *)0x4C)
				{
						Real_Ctl1_Value[3] = buf[0];
						Real_Ctl2_Value[3] = buf[1];
				}

        
                
    }
}
//#if CAN1_RX0_INT_ENABLE                         //ʹ��RX0�ж�
////CAN�жϷ�����
//void CAN1_RX0_IRQHandler(void)
//{
//    HAL_CAN_IRQHandler(&CAN1_Handler);//�˺��������CAN_Receive_IT()��������
//}

////CAN�жϴ������
////�˺����ᱻCAN_Receive_IT()����
////hcan:CAN���
//void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
//{
//    int i=0;
//    //CAN_Receive_IT()������ر�FIFO0��Ϣ�Һ��жϣ����������Ҫ���´�
//    __HAL_CAN_ENABLE_IT(&CAN1_Handler,CAN_IT_FMP0);//���¿���FIF00��Ϣ�Һ��ж�
//    printf("id:%d\r\n",CAN1_Handler.pRxMsg->StdId);
//    printf("ide:%d\r\n",CAN1_Handler.pRxMsg->IDE);
//    printf("rtr:%d\r\n",CAN1_Handler.pRxMsg->RTR);
//    printf("len:%d\r\n",CAN1_Handler.pRxMsg->DLC);
//    for(i=0;i<8;i++)
//    printf("rxbuf[%d]:%d\r\n",i,CAN1_Handler.pRxMsg->Data[i]);
//}
//#endif	

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 CAN1_Send_Msg(u8* msg,u8 len)
{	
    u16 i=0;
    CAN1_Handler.pTxMsg->StdId=0X12;        //��׼��ʶ��
    CAN1_Handler.pTxMsg->ExtId=0x12;        //��չ��ʶ��(29λ)
    CAN1_Handler.pTxMsg->IDE=CAN_ID_STD;    //ʹ�ñ�׼֡
    CAN1_Handler.pTxMsg->RTR=CAN_RTR_DATA;  //����֡
    CAN1_Handler.pTxMsg->DLC=len;                
    for(i=0;i<len;i++)
    CAN1_Handler.pTxMsg->Data[i]=msg[i];
    if(HAL_CAN_Transmit(&CAN1_Handler,10)!=HAL_OK) return 1;     //����
    return 0;		
}
//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 CAN1_ID_Send_Msg(uint32_t id,u8* msg,u8 len)
{	
    u16 i=0;
	  HAL_StatusTypeDef  res;
    CAN1_Handler.pTxMsg->StdId=id;        //��׼��ʶ��
    CAN1_Handler.pTxMsg->ExtId=0x12;        //��չ��ʶ��(29λ)
    CAN1_Handler.pTxMsg->IDE=CAN_ID_STD;    //ʹ�ñ�׼֡
    CAN1_Handler.pTxMsg->RTR=CAN_RTR_DATA;  //����֡
    CAN1_Handler.pTxMsg->DLC=len;                
    for(i=0;i<len;i++)
    CAN1_Handler.pTxMsg->Data[i]=msg[i];
		res = HAL_CAN_Transmit(&CAN1_Handler,10);
//	  printf("send res:%x\r\n",res);
	  if(res != HAL_OK) return 1;     //����
//    if(HAL_CAN_Transmit(&CAN1_Handler,10)!=HAL_OK) return 1;     //����
    return 0;		
}
//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//		 ����,���յ����ݳ���;
u8 CAN1_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
    if(HAL_CAN_Receive(&CAN1_Handler,CAN_FIFO0,0)!=HAL_OK) return 0;//�������ݣ���ʱʱ������Ϊ0	
    for(i=0;i<CAN1_Handler.pRxMsg->DLC;i++)
    buf[i]=CAN1_Handler.pRxMsg->Data[i];
	return CAN1_Handler.pRxMsg->DLC;	
}
//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//		 ����,���յ����ݳ���;
u8 CAN1_ID_Receive_Msg(u8* buf,uint32_t* stdidbuf)
{		   		   
 	u32 i;
    if(HAL_CAN_Receive(&CAN1_Handler,CAN_FIFO0,0)!=HAL_OK) return 0;//�������ݣ���ʱʱ������Ϊ0	
    for(i=0;i<CAN1_Handler.pRxMsg->DLC;i++)
    buf[i]=CAN1_Handler.pRxMsg->Data[i];
		stdidbuf[0] = CAN1_Handler.pRxMsg->StdId;
	return CAN1_Handler.pRxMsg->DLC;	
}

