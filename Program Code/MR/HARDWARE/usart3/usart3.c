#include "usart3.h"
#include "delay.h"

extern int res;

float pos_x=0;
float pos_y=0;
float zangle=0;
float xangle=0;
float yangle=0;
float w_z=0;

#if EN_USART3_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART3_RX_BUF[USART3_REC_LEN];     //���ջ���,���USART3_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART3_RX_STA=0;       //����״̬���	

u8 aRxBuffer3[RXBUFFERSIZE3];//HAL��ʹ�õĴ��ڽ��ջ���
UART_HandleTypeDef UART3_Handler; //UART���

//��ʼ��IO ����1 
//bound:������
void uart3_init(u32 bound)
{	
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();			//ʹ��GPIOAʱ��
	__HAL_RCC_USART3_CLK_ENABLE();			//ʹ��USART3ʱ��	
	//UART ��ʼ������
	UART3_Handler.Instance=USART3;					    //USART3
	UART3_Handler.Init.BaudRate=bound;				    //������
	UART3_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART3_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART3_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART3_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART3_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART3_Handler);					    //HAL_UART_Init()��ʹ��UART3

	GPIO_Initure.Pin=GPIO_PIN_10;			//PB10-- USART3_Tx
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
	GPIO_Initure.Pull=GPIO_PULLUP;			//����
	GPIO_Initure.Speed=GPIO_SPEED_FAST;		//����
	GPIO_Initure.Alternate=GPIO_AF7_USART3;	//����ΪUSART3
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//��ʼ��PA9

	GPIO_Initure.Pin=GPIO_PIN_11;			//PB11--USART3_Rx
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//��ʼ��PA10
	
#if EN_USART3_RX
	HAL_NVIC_EnableIRQ(USART3_IRQn);		//ʹ��USART3�ж�ͨ��
	HAL_NVIC_SetPriority(USART3_IRQn,3,2);	//��ռ���ȼ�3�������ȼ�3
#endif		
	HAL_UART_Receive_IT(&UART3_Handler, (u8 *)aRxBuffer3, RXBUFFERSIZE3);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
  
}

//����1�жϷ������
void USART3_IRQHandler(void)                	
{ 

	static uint8_t ch;
	static union 
	{
		uint8_t data[24];
		float ActVal[6];
	}posture;
	static uint8_t count=0;
	static uint8_t i=0;	
	if((__HAL_UART_GET_FLAG(&UART3_Handler,UART_FLAG_RXNE)!=RESET))  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		ch=USART3->DR; 
		res=ch;
		//printf("res:%d",res);
		switch(count)
		{
			case 0:
				if(ch==0x0d)
					count++;
				else 
					count=0;
				break;
			case 1:
				if(ch==0x0a)
				{
					i=0;
					count++;
				}
				else if(ch==0x0d);
				else 
					count=0;
				break;
			case 2:
				posture.data[i]=ch;
			i++;
			if(i>=24)
			{
				i=0;
				count++;
			}
			break;
			case 3:
				if(ch==0x0a)
					count++;
				else
					count=0;
				break;
			case 4:
				if(ch==0x0d)
				{
//					printf("strat!\r\n");
					zangle=posture.ActVal[0];
					xangle=posture.ActVal[1];
					yangle=posture.ActVal[2];
					pos_x=posture.ActVal[3];
					pos_y=posture.ActVal[4];
					w_z=posture.ActVal[5];
				}
				count=0;
				break;
			default:
				count=0;
			break;
					
		}
	
	}
	HAL_UART_IRQHandler(&UART3_Handler);	
} 

//UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
//�˺����ᱻHAL_UART_Init()����
//huart:���ھ��

//void HAL_UART_MspInit(UART_HandleTypeDef *huart)
//{
//    //GPIO�˿�����
//	GPIO_InitTypeDef GPIO_Initure;
//	
//	if(huart->Instance==USART3)//����Ǵ���1�����д���1 MSP��ʼ��
//	{
//		__HAL_RCC_GPIOB_CLK_ENABLE();			//ʹ��GPIOAʱ��
//		__HAL_RCC_USART3_CLK_ENABLE();			//ʹ��USART3ʱ��
//	
//		GPIO_Initure.Pin=GPIO_PIN_10;			//PB10-- USART3_Tx
//		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
//		GPIO_Initure.Pull=GPIO_PULLUP;			//����
//		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//����
//		GPIO_Initure.Alternate=GPIO_AF7_USART3;	//����ΪUSART3
//		HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//��ʼ��PA9

//		GPIO_Initure.Pin=GPIO_PIN_11;			//PB11--USART3_Rx
//		HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//��ʼ��PA10
//		
//#if EN_USART3_RX
//		HAL_NVIC_EnableIRQ(USART3_IRQn);		//ʹ��USART3�ж�ͨ��
//		HAL_NVIC_SetPriority(USART3_IRQn,3,3);	//��ռ���ȼ�3�������ȼ�3
//#endif	
//	}

//}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	if(huart->Instance==USART3)//����Ǵ���1
//	{
//		if((USART3_RX_STA&0x8000)==0)//����δ���
//		{
//			if(USART3_RX_STA&0x4000)//���յ���0x0d
//			{
//				if(aRxBuffer3[0]!=0x0a)USART3_RX_STA=0;//���մ���,���¿�ʼ
//				else USART3_RX_STA|=0x8000;	//��������� 
//			}
//			else //��û�յ�0X0D
//			{	
//				if(aRxBuffer3[0]==0x0d)USART3_RX_STA|=0x4000;
//				else
//				{
//					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=aRxBuffer3[0] ;
//					USART3_RX_STA++;
//					if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
//				}		 
//			}
//		}

//	}
//}
// 
////����1�жϷ������
//void USART3_IRQHandler(void)                	
//{ 
//	u32 timeout=0;
//#if SYSTEM_SUPPORT_OS	 	//ʹ��OS
//	OSIntEnter();    
//#endif
//	
//	HAL_UART_IRQHandler(&UART3_Handler);	//����HAL���жϴ����ú���
//	
//	timeout=0;
//    while (HAL_UART_GetState(&UART3_Handler) != HAL_UART_STATE_READY)//�ȴ�����
//	{
//	 timeout++;////��ʱ����
//     if(timeout>HAL_MAX_DELAY) break;		
//	}
//     
//	timeout=0;
//	while(HAL_UART_Receive_IT(&UART3_Handler, (u8 *)aRxBuffer3, RXBUFFERSIZE3) != HAL_OK)//һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
//	{
//	 timeout++; //��ʱ����
//	 if(timeout>HAL_MAX_DELAY) break;	
//	}
//#if SYSTEM_SUPPORT_OS	 	//ʹ��OS
//	OSIntExit();  											 
//#endif
//} 
#endif	


/*�����������ֱ�Ӱ��жϿ����߼�д���жϷ������ڲ���
 ˵��������HAL�⴦���߼���Ч�ʲ��ߡ�*/
/*


//����1�жϷ������
void USART3_IRQHandler(void)                	
{ 
	u8 Res;
#if SYSTEM_SUPPORT_OS	 	//ʹ��OS
	OSIntEnter();    
#endif
	if((__HAL_UART_GET_FLAG(&UART3_Handler,UART_FLAG_RXNE)!=RESET))  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
        HAL_UART_Receive(&UART3_Handler,&Res,1,1000); 
		if((USART3_RX_STA&0x8000)==0)//����δ���
		{
			if(USART3_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)USART3_RX_STA=0;//���մ���,���¿�ʼ
				else USART3_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)USART3_RX_STA|=0x4000;
				else
				{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
					USART3_RX_STA++;
					if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   		 
	}
	HAL_UART_IRQHandler(&UART3_Handler);	
#if SYSTEM_SUPPORT_OS	 	//ʹ��OS
	OSIntExit();  											 
#endif
} 
#endif	
*/
 




