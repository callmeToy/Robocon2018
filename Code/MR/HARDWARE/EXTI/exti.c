#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"

u8  PE2flag = 0;   //0:��ʾ��ִ�� 1����ʾҪִ��
u8  PE4flag = 0;   //0:��ʾ��ִ�� 1����ʾҪִ��
u8  PE5flag = 0;   //0:��ʾ��ִ�� 1����ʾҪִ��
u8  PG3flag = 0;   //0:��ʾ��ִ�� 1����ʾҪִ��
u8  PG13flag = 0;   //0:��ʾ��ִ�� 1����ʾҪִ��
//�ⲿ�жϳ�ʼ��



void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOE_CLK_ENABLE();               //����GPIOEʱ��
    __HAL_RCC_GPIOG_CLK_ENABLE();               //����GPIOCʱ��
//    __HAL_RCC_GPIOH_CLK_ENABLE();               //����GPIOHʱ��
    
//    GPIO_Initure.Pin=GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;                //PA0
//    GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      //�����ش���
//    GPIO_Initure.Pull=GPIO_PULLDOWN;
//    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 ;               //PC13
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //�½��ش���
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_13 | GPIO_PIN_3;     //PH2,3
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);
    
    //�ж���4
    HAL_NVIC_SetPriority(EXTI4_IRQn,2,0);       //��ռ���ȼ�Ϊ2�������ȼ�Ϊ0
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);             //ʹ���ж���0
    
    //�ж���2
    HAL_NVIC_SetPriority(EXTI2_IRQn,2,1);       //��ռ���ȼ�Ϊ2�������ȼ�Ϊ1
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);             //ʹ���ж���2
    
    //�ж���3
    HAL_NVIC_SetPriority(EXTI3_IRQn,2,2);       //��ռ���ȼ�Ϊ2�������ȼ�Ϊ2
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);             //ʹ���ж���2
    
    //�ж���5 
    HAL_NVIC_SetPriority(EXTI9_5_IRQn,2,3);   //��ռ���ȼ�Ϊ2�������ȼ�Ϊ3
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);         //ʹ���ж���13  
		
    //�ж���13 
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,3,0);   //��ռ���ȼ�Ϊ2�������ȼ�Ϊ3
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         //ʹ���ж���13  		
}


//�жϷ�����
void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);//�����жϴ����ú���
}

void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);//�����жϴ����ú���
}

void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);//�����жϴ����ú���
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);//�����жϴ����ú���
}
void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);//�����жϴ����ú���
}
//�жϷ����������Ҫ��������
//��HAL�������е��ⲿ�жϷ�����������ô˺���
//GPIO_Pin:�ж����ź�
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
		////  delay_ms(100);      //����
    switch(GPIO_Pin)
    {
        case GPIO_PIN_2:
            if(0 == HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2)) 
            {
							PE2flag = 1;   //0:��ʾ��ִ�� 1����ʾҪִ��
//				LED1=!LED1;//����LED0,LED1�������
//				LED0=!LED1;
            }
            break;
        case GPIO_PIN_3:
            if(0 == HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_3))  //LED1��ת
            {
				 
							PG3flag = 1;   //0:��ʾ��ִ�� 1����ʾҪִ��
              //  LED1=!LED1;  
							//	LED0=!LED0;							
            }
            break;
        case GPIO_PIN_4:
            if(0 == HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4))  //ͬʱ����LED0,LED1��ת 
            {
							PE4flag = 1;   //0:��ʾ��ִ�� 1����ʾҪִ��
              //  LED0=!LED0;
							//  LED1=!LED1;
            }
            break;						
        case GPIO_PIN_5:
            if(0 == HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_5))  //ͬʱ����LED0,LED1��ת 
            {
							PE5flag = 1;   //0:��ʾ��ִ�� 1����ʾҪִ��
              //  LED0=!LED0;
							//	LED1=!LED1;
            }
            break;
        case GPIO_PIN_13:
            if(0 == HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_13))  
            {
							PG13flag = 1;   //0:��ʾ��ִ�� 1����ʾҪִ��
							//	LED0=!LED0;//����LED0��ת
							//  printf("led\r\n");
            }
            break;
    }
}
