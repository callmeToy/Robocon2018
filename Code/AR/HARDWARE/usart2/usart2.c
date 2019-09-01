#include "usart2.h"
#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
extern int res;

float pos_x=0;
float pos_y=0;
float zangle=0;
float xangle=0;
float yangle=0;
float w_z=0;


#if EN_USART2_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART2_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART2_RX_STA=0;       //����״̬���	

u8 aRxBuffer2[RXBUFFERSIZE2];//HAL��ʹ�õĴ��ڽ��ջ���
UART_HandleTypeDef UART2_Handler; //UART���

//��ʼ��IO ����1 
//bound:������
void uart2_init(u32 bound)
{	
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
	__HAL_RCC_USART2_CLK_ENABLE();			//ʹ��USART2ʱ��
	//UART ��ʼ������
	UART2_Handler.Instance=USART2;					    //USART2
	UART2_Handler.Init.BaudRate=bound;				    //������
	UART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART2_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART2_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART2_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART2_Handler);					    //HAL_UART_Init()��ʹ��UART2


	
	GPIO_Initure.Pin=GPIO_PIN_2;			//PA2-- USART2_Tx
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
	GPIO_Initure.Pull=GPIO_PULLUP;			//����
	GPIO_Initure.Speed=GPIO_SPEED_FAST;		//����
	GPIO_Initure.Alternate=GPIO_AF7_USART2;	//����ΪUSART2
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA9

	GPIO_Initure.Pin=GPIO_PIN_3;			//PA3--USART2_Rx
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA10
	
#if EN_USART2_RX
	HAL_NVIC_EnableIRQ(USART2_IRQn);		//ʹ��USART2�ж�ͨ��
	HAL_NVIC_SetPriority(USART2_IRQn,3,3);	//��ռ���ȼ�3�������ȼ�3
#endif		
	HAL_UART_Receive_IT(&UART2_Handler, (u8 *)aRxBuffer2, RXBUFFERSIZE2);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
  
}


//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	if(huart->Instance==USART2)//����Ǵ���1
//	{
//		if((USART2_RX_STA&0x8000)==0)//����δ���
//		{
//			if(USART2_RX_STA&0x4000)//���յ���0x0d
//			{
//				if(aRxBuffer2[0]!=0x0a)USART2_RX_STA=0;//���մ���,���¿�ʼ
//				else USART2_RX_STA|=0x8000;	//��������� 
//			}
//			else //��û�յ�0X0D
//			{	
//				if(aRxBuffer2[0]==0x0d)USART2_RX_STA|=0x4000;
//				else
//				{
//					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=aRxBuffer2[0] ;
//					USART2_RX_STA++;
//					if(USART2_RX_STA>(USART2_REC_LEN-1))USART2_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
//				}		 
//			}
//		}

//	}
//}
// 
////����1�жϷ������
//void USART2_IRQHandler(void)                	
//{ 
//	u32 timeout=0;
//#if SYSTEM_SUPPORT_OS	 	//ʹ��OS
//	OSIntEnter();    
//#endif
//	
//	HAL_UART_IRQHandler(&UART2_Handler);	//����HAL���жϴ����ú���
//	
//	timeout=0;
//    while (HAL_UART_GetState(&UART2_Handler) != HAL_UART_STATE_READY)//�ȴ�����
//	{
//	 timeout++;////��ʱ����
//     if(timeout>HAL_MAX_DELAY) break;		
//	}
//     
//	timeout=0;
//	while(HAL_UART_Receive_IT(&UART2_Handler, (u8 *)aRxBuffer2, RXBUFFERSIZE2) != HAL_OK)//һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
//	{
//	 timeout++; //��ʱ����
//	 if(timeout>HAL_MAX_DELAY) break;	
//	}
//#if SYSTEM_SUPPORT_OS	 	//ʹ��OS
//	OSIntExit();  											 
//#endif
//} 
//#endif	


/*�����������ֱ�Ӱ��жϿ����߼�д���жϷ������ڲ���
 ˵��������HAL�⴦���߼���Ч�ʲ��ߡ�*/



//����1�жϷ������
void USART2_IRQHandler(void)                	
{ 

	static uint8_t ch;
	static union 
	{
		uint8_t data[24];
		float ActVal[6];
	}posture;
	static uint8_t count=0;
	static uint8_t i=0;	
	if((__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_RXNE)!=RESET))  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		ch=USART2->DR; 
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
					printf("strat!\r\n");
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
	HAL_UART_IRQHandler(&UART2_Handler);	
} 
#endif	

 




