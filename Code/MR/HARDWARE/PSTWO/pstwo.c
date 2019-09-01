#include "pstwo.h"
#include "stdbool.h"
/*********************************************************
opyright (C), 2015-2025, YFRobot.
www.yfrobot.com
File��PS2��������
Author��pinggai    Version:1.1     Data:2015/10/20
Description: PS2��������
             ���ӹ��ܣ�
			 1��������á����ģʽ�������̵�ģʽ�������������á����桱��ͨ���ֱ���ģʽ�������޷��ı�
			 2�������ֱ��𶯣�ͨ����ֵ�����ã��ı������𶯵����Ƶ�ʡ�
			                  ͨ����ֵ�����ã������Ҳ�С�𶯵����
History:  
V1.0: 	2015/05/16
1���ֱ����룬ʶ�𰴼�ֵ����ȡģ��ֵ��       
**********************************************************/	 
#define DELAY_TIME  delay_us(5 ); 
u16 Handkey;	// ����ֵ��ȡ����ʱ�洢��
u8 Comd[2]={0x01,0x42};	//��ʼ�����������
u8 Data[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //���ݴ洢����

double Handle_Addup = 0;
//����Ϊ1-16
u16 MASK[]={
    PSB_SELECT,
    PSB_L3,
    PSB_R3 ,
    PSB_START,
    PSB_PAD_UP,
    PSB_PAD_RIGHT,
    PSB_PAD_DOWN,
    PSB_PAD_LEFT,
    PSB_L2,
    PSB_R2,
    PSB_L1,
    PSB_R1 ,
    PSB_GREEN,
    PSB_RED,
    PSB_BLUE,
    PSB_PINK
	};	//����ֵ�밴����

	
	
void PS2_Init(void)
{
	    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();           //����GPIOBʱ��
//    __HAL_RCC_GPIOA_CLK_ENABLE();           //����GPIOBʱ��
    __HAL_RCC_GPIOD_CLK_ENABLE();           //����GPIOBʱ��	
	
    GPIO_Initure.Pin=GPIO_PIN_11; //PB1,0  |GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  //�������
    GPIO_Initure.Pull=GPIO_PULLDOWN;          //����
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //����
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);
		
	
    GPIO_Initure.Pin=GPIO_PIN_13|GPIO_PIN_12|GPIO_PIN_14; //PB1,0
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
//    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);	//PB0��1 
  //  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);	//PB1��1  
//		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6,GPIO_PIN_RESET);	//��0
		
		//HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_SET);	//PB1��1  	
		//HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4|GPIO_PIN_5,GPIO_PIN_SET);	//PB1��1  
//	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2,GPIO_PIN_RESET);	//PH2=0
//	HAL_GPIO_WritePin(GPIOH,GPIO_PIN_3,GPIO_PIN_RESET);	//PH3=0
}
	
	
	
//�ֱ��ӿڳ�ʼ��    ����  DAT(DI)->PD11 
//                  ���  DO(CMD)->PB12    CS->PB13  CLK->PB14
//void PS2_Init(void)
//{
//	    
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);    //ʹ��PORTBʱ��
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);    //ʹ��PORTBʱ��
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;//����  DI->PD11
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����
//  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIO


//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;//PB12��PB13��PB14 �������   	
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
//  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO

//}

//���ֱ���������
void PS2_Cmd(u8 CMD)
{
	volatile u16 ref=0x01;
	Data[1] = 0;
	for(ref=0x01;ref<0x0100;ref<<=1)
	{
		if(ref&CMD)
		{
			DO_H;                   //���һλ����λ
		}
		else DO_L;

		CLK_H;                        //ʱ������
		DELAY_TIME;
		CLK_L;
		DELAY_TIME;
		CLK_H;
		if(DI)
			Data[1] = ref|Data[1];
	}
	delay_us(16);
}
//�ж��Ƿ�Ϊ���ģʽ,0x41=ģ���̵ƣ�0x73=ģ����
//����ֵ��0�����ģʽ
//		  ����������ģʽ
u8 PS2_RedLight(void)
{
	CS_L;
	PS2_Cmd(Comd[0]);  //��ʼ����
	PS2_Cmd(Comd[1]);  //��������
	CS_H;
	if( Data[1] == 0X73)   return 0 ;
	else return 1;

}
//��ȡ�ֱ�����
void PS2_ReadData(void)
{
	volatile u8 byte=0;
	volatile u16 ref=0x01;
	CS_L;
	PS2_Cmd(Comd[0]);  //��ʼ����
	PS2_Cmd(Comd[1]);  //��������
	for(byte=2;byte<9;byte++)          //��ʼ��������
	{
		for(ref=0x01;ref<0x100;ref<<=1)
		{
			CLK_H;
			DELAY_TIME;
			CLK_L;
			DELAY_TIME;
			CLK_H;
		      if(DI)
		      Data[byte] = ref|Data[byte];
		}
        delay_us(16);
	}
	CS_H;
}

//�Զ�������PS2�����ݽ��д���,ֻ����������  
//ֻ��һ����������ʱ����Ϊ0�� δ����Ϊ1

u8 PS2_DataKey_Once(){
	static u8 lastkey = 0;
	u8 key = 0;
	u8 result = 0;
	key = PS2_DataKey();
	if(key!=0 && lastkey!=key){
		result = key;
	}
	lastkey = key;
	return result;
}

u16 LastHandkey = 0xFFFF;

u8 PS2_DataKey()
{
	u8 index;

	PS2_ClearData();
	PS2_ReadData();

	Handkey=(Data[4]<<8)|Data[3];     //����16������  ����Ϊ0�� δ����Ϊ1
	for(index=0;index<16;index++)
	{	    
		if((Handkey&(1<<(MASK[index]-1)))==0)
		return index+1;
	}
	return 0;          //û���κΰ�������
}

bool PS2_NewButtonState( u16 button )
{
  button = 0x0001u << ( button - 1 );  //�����button��ֵ�� �ð���������������bit��ֵ+1�� ���� PSB_SELECT ���ֵ �� 1�� �������е�λ��0λ�� ������ƣ�
  return ( ( ( LastHandkey ^ Handkey ) & button ) > 0 );  //���ϴεİ������ݺ���εİ������ݽ���������㣬����������β�ͬ�Ĳ��ֻ���1���͵õ���״̬�����˱仯�İ���
	                                                    //Ȼ������������Ҫ���İ������������㣬���������������˱仯����ô�������1�� ����0�����Է��ؾ���true
}

bool PS2_Button( u16 button )
{
  button = 0x0001u << ( button - 1 );  //�����button��ֵ�� �ð���������������bit��ֵ+1�� ���� PSB_SELECT ���ֵ �� 1�� �������е�λ��0λ�� ������ƣ�
  return ( ( (~Handkey) & button ) > 0 );  //�����������ӦλΪ0��û����Ϊ1�� ����������ȡ��֮�󣬾ͱ���˰���Ϊ1��û����Ϊ0
	                                         //����������Ҫ���İ����������㣬��������������£���Ӧλ����1��û���¾���0��������0�ȽϵĽ����
}

bool PS2_ButtonPressed( u16 button )
{
  return (PS2_NewButtonState( button ) && PS2_Button( button ));  //��������ס����������ǰ�����һ���µ�״̬����ô���ǰ����ձ�����
}

bool PS2_ButtonReleased( u16 button )
{
  return ( PS2_NewButtonState( button ) && !PS2_Button( button )); //����û����ס����������ǰ�����һ���µ�״̬����ô���ǰ����ձ��ɿ�
}

//�õ�һ��ҡ�˵�ģ����	 ��Χ0~256
u8 PS2_AnologData(u8 button)
{
	return Data[button];
}

//������ݻ�����
void PS2_ClearData()
{
	u8 a;
	for(a=0;a<9;a++)
		Data[a]=0x00;
}
/******************************************************
Function:    void PS2_Vibration(u8 motor1, u8 motor2)
Description: �ֱ��𶯺�����
Calls:		 void PS2_Cmd(u8 CMD);
Input: motor1:�Ҳ�С�𶯵�� 0x00�أ�������
	   motor2:�����𶯵�� 0x40~0xFF �������ֵԽ�� ��Խ��
******************************************************/
void PS2_Vibration(u8 motor1, u8 motor2)
{
	CS_L;
	delay_us(16);
    PS2_Cmd(0x01);  //��ʼ����
	PS2_Cmd(0x42);  //��������
	PS2_Cmd(0X00);
	PS2_Cmd(motor1);
	PS2_Cmd(motor2);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	CS_H;
	delay_us(16);  
}
//short poll
void PS2_ShortPoll(void)
{
	CS_L;
	delay_us(16);
	PS2_Cmd(0x01);  
	PS2_Cmd(0x42);  
	PS2_Cmd(0X00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	CS_H;
	delay_us(16);	
}
//��������
void PS2_EnterConfing(void)
{
    CS_L;
	delay_us(16);
	PS2_Cmd(0x01);  
	PS2_Cmd(0x43);  
	PS2_Cmd(0X00);
	PS2_Cmd(0x01);
	PS2_Cmd(0x00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	CS_H;
	delay_us(16);
}
//����ģʽ����
void PS2_TurnOnAnalogMode(void)
{
	CS_L;
	PS2_Cmd(0x01);  
	PS2_Cmd(0x44);  
	PS2_Cmd(0X00);
	PS2_Cmd(0x01); //analog=0x01;digital=0x00  ������÷���ģʽ
	PS2_Cmd(0xEE); //Ox03�������ã�������ͨ��������MODE������ģʽ��
				   //0xEE������������ã���ͨ��������MODE������ģʽ��
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	CS_H;
	delay_us(16);
}
//������
void PS2_VibrationMode(void)
{
	CS_L;
	delay_us(16);
	PS2_Cmd(0x01);  
	PS2_Cmd(0x4D);  
	PS2_Cmd(0X00);
	PS2_Cmd(0x00);
	PS2_Cmd(0X01);
	CS_H;
	delay_us(16);	
}
//��ɲ���������
void PS2_ExitConfing(void)
{
    CS_L;
	delay_us(16);
	PS2_Cmd(0x01);  
	PS2_Cmd(0x43);  
	PS2_Cmd(0X00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	CS_H;
	delay_us(16);
}
//�ֱ����ó�ʼ��
void PS2_SetInit(void)
{
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_EnterConfing();		//��������ģʽ
	PS2_TurnOnAnalogMode();	//�����̵ơ�����ģʽ����ѡ���Ƿ񱣴�
	PS2_VibrationMode();	//������ģʽ
	PS2_ExitConfing();		//��ɲ���������
}




