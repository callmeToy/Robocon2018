#include "main.h"

int temp_pwm = 0;
int temp_pwma = 0;
int main(void)
{     
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    CAN1_Configuration();                               //CAN1��ʼ��
    
    delay_ms(500);                                      //�տ�ʼҪ���㹻����ʱ��ȷ���������Ѿ���ʼ����� 
    CAN_RoboModule_DRV_Reset(0,0);                      //��0���������������и�λ 
    
    delay_ms(500);                                      //���͸�λָ������ʱ����Ҫ�У��ȴ��������ٴγ�ʼ�����
    
    CAN_RoboModule_DRV_Config(0,1,100,0);               //1������������Ϊ100ms����һ������
    delay_us(200);                                      //�˴���ʱΪ�˲��ô�������ʱ��4����һ��
    CAN_RoboModule_DRV_Config(0,2,100,0);               //2������������Ϊ100ms����һ������
    delay_us(200);                                      //�˴���ʱΪ�˲��ô�������ʱ��4����һ��
    CAN_RoboModule_DRV_Config(0,3,100,0);               //3������������Ϊ100ms����һ������
    delay_us(200);                                      //�˴���ʱΪ�˲��ô�������ʱ��4����һ��
    CAN_RoboModule_DRV_Config(0,4,100,0);               //3������������Ϊ100ms����һ������

    
    CAN_RoboModule_DRV_Mode_Choice(0,0,Velocity_Mode);  //0������������� �����뿪��ģʽ
    delay_ms(500);                                      //����ģʽѡ��ָ���Ҫ�ȴ�����������ģʽ������������ʱҲ������ȥ����
    
    temp_pwm = 0;
		temp_pwma = 0;
     while(1) //���Լ���
    {
        delay_ms(100);
        
        CAN_RoboModule_DRV_OpenLoop_Mode(0,1,temp_pwm);
        //CAN_RoboModule_DRV_OpenLoop_Mode(0,2,temp_pwm);
        CAN_RoboModule_DRV_OpenLoop_Mode(0,3,temp_pwma);
       // CAN_RoboModule_DRV_OpenLoop_Mode(0,4,temp_pwm);
        temp_pwm += 200;
			temp_pwma -= 200;
        if(temp_pwm > 4500)
        {
            break;
					if(temp_pwma < 4500)
        {
            break;
        }
        }
    }
    
    temp_pwm = 4500;
    while(1) //1���Լ���
    {
        delay_ms(100);
        
        CAN_RoboModule_DRV_OpenLoop_Mode(0,1,temp_pwm);
        //CAN_RoboModule_DRV_OpenLoop_Mode(0,2,temp_pwm);
        CAN_RoboModule_DRV_OpenLoop_Mode(0,3,temp_pwma);
        //CAN_RoboModule_DRV_OpenLoop_Mode(0,4,temp_pwm);
        temp_pwm -= 100;
			temp_pwma += 100;
        if(temp_pwm < 0)
        {
            break;
        }
				if(temp_pwma > 0)
        {
            break;
        }
    }
	
    
    temp_pwm = 0;
		temp_pwma = 0;
    
    CAN_RoboModule_DRV_OpenLoop_Mode(0,0,temp_pwm);
    
    while(1);
}


/*
1.������Ҫ��ǰ��װ��keil MDK���Ƽ��汾V4.74��
2.����������оƬΪSTM32F103C8T6��
3.�����Ҫ����CAN��ͨ�ţ��û����ÿ�����������ذ壬���뺬��CAN�շ�����������ʹ�õ�CAN����ΪPA11 PA12��
4.����оƬ���в�ͬ���������ڹ������޸ģ�CAN�������в�ͬ����������can.c�ļ����޸ġ�
*/
