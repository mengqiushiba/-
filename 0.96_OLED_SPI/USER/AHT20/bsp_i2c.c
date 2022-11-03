#include "bsp_i2c.h"
#include "delay.h"
#include "string.h"
#include "gui.h"
#include <stdio.h>

uint8_t   ack_status=0;
uint8_t   readByte[6];

uint32_t  H1=0;  //Humility
uint32_t  T1=0;  //Temperature
uint8_t  t;
u8 *strTemp1;
u8 *strTemp2;
u8 *strTemp3;
u8 *strHumi1;
u8 *strHumi2;
u8 *strHumi3;

uint8_t  AHT20_OutData[4];

/****************
 *��ʼ�� I2C ����
 ****************/
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//���ø��� APB (APB2) ��Χʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );	
	
	//GPIO ����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//��ʼ�� SCL��Pin6���ߵ�ƽ
	IIC_SCL=1;
	//��ʼ�� SDA��Pin7���ߵ�ƽ
	IIC_SDA=1;
}

/*********************
 *AHT20 ���ݲ����ܺ���
 *********************/
void read_AHT20_once(void)
{

	//��ʱ 10 ΢��
	delay_ms(10);
	
  //��������ǰ������������������λ
	reset_AHT20();
	delay_ms(10);
  
	//�鿴ʹ��λ
	init_AHT20();
	delay_ms(10);
	
  //��������
	startMeasure_AHT20();
	delay_ms(80);
  
	//������
	read_AHT20();
	delay_ms(5);
}


void reset_AHT20(void)
{
	//���ݴ��俪ʼ�ź�
	I2C_Start();
	
	//��������
	I2C_WriteByte(0x70);
	//���� ACK �ź�
	ack_status = Receive_ACK();

	//������λ�������������ϵͳ��
	I2C_WriteByte(0xBA);
	//���� ACK �ź�
	ack_status = Receive_ACK();

	//ֹͣ I2C Э��
	I2C_Stop();
}

//0x70 ��> 0111 0000 ǰ��λ��ʾ I2C ��ַ���ڰ�λΪ0����ʾ write
//0xE1 ��> ��״̬�ֵ�У׼ʹ��λBit[3]�Ƿ�Ϊ 1
//0x08 0x00 ��> 0xBE ������������������ AHT20 �ο��ֲ�
void init_AHT20(void)
{
	//���俪ʼ
	I2C_Start();

	//д�� 0x70 ����
	I2C_WriteByte(0x70);
	//���� ACK �ź�
	ack_status = Receive_ACK();

	//д�� 0xE1 ����
	I2C_WriteByte(0xE1);
	ack_status = Receive_ACK();

	//д�� 0x08 ����
	I2C_WriteByte(0x08);
	ack_status = Receive_ACK();

	//д�� 0x00 ����
	I2C_WriteByte(0x00);
	ack_status = Receive_ACK();

	//ֹͣ I2C Э��
	I2C_Stop();
}

//0x70 ��> 0111 0000 ǰ��λ��ʾ I2C ��ַ���ڰ�λΪ0����ʾ write
//0xAC ��> ��������
//0x33 0x00 ��> 0xAC ������������������ AHT20 �ο��ֲ�
void startMeasure_AHT20(void)
{
	//���� I2C Э��
	I2C_Start();
	
	I2C_WriteByte(0x70);
	ack_status = Receive_ACK();

	I2C_WriteByte(0xAC);
	ack_status = Receive_ACK();

	I2C_WriteByte(0x33);
	ack_status = Receive_ACK();

	I2C_WriteByte(0x00);
	ack_status = Receive_ACK();

	I2C_Stop();
}


void read_AHT20(void)
{
	uint8_t i;

	//��ʼ�� readByte ����
	for(i=0; i<6; i++)
	{
		readByte[i]=0;
	}

	I2C_Start();

	//ͨ������ 0x71 ���Ի�ȡһ���ֽڵ�״̬��
	I2C_WriteByte(0x71);
	ack_status = Receive_ACK();
	
	//���� 6 �� 8 bit������
	readByte[0]= I2C_ReadByte();
	//���� ACK �ź�
	Send_ACK();

	readByte[1]= I2C_ReadByte();
	Send_ACK();

	readByte[2]= I2C_ReadByte();
	Send_ACK();

	readByte[3]= I2C_ReadByte();
	Send_ACK();

	readByte[4]= I2C_ReadByte();
	Send_ACK();

	readByte[5]= I2C_ReadByte();
	//���� NACK �ź�
	SendNot_Ack();

	I2C_Stop();

	//��ʪ�ȵĶ��������ݴ���
	//0x68 = 0110 1000
  //0x08 = 0000 1000	
	if( (readByte[0] & 0x68) == 0x08 )
	{
		H1 = readByte[1];
		//H1 ���� 8 λ���� readByte[2] ��� 
		H1 = (H1<<8) | readByte[2];
		H1 = (H1<<8) | readByte[3];
		//H1 ���� 4 λ
		H1 = H1>>4;

		H1 = (H1*1000)/1024/1024;

		T1 = readByte[3];
		//������
		T1 = T1 & 0x0000000F;
		T1 = (T1<<8) | readByte[4];
		T1 = (T1<<8) | readByte[5];

		T1 = (T1*2000)/1024/1024 - 500;

		AHT20_OutData[0] = (H1>>8) & 0x000000FF;
		AHT20_OutData[1] = H1 & 0x000000FF;

		AHT20_OutData[2] = (T1>>8) & 0x000000FF;
		AHT20_OutData[3] = T1 & 0x000000FF;
	}
	else
	{
		AHT20_OutData[0] = 0xFF;
		AHT20_OutData[1] = 0xFF;

		AHT20_OutData[2] = 0xFF;
		AHT20_OutData[3] = 0xFF;

	}
	/*
	printf("��ɣ�\n");
	printf("----�¶�:%d%d.%d ��C\n",T1/100,(T1/10)%10,T1%10);
	printf("----ʪ��:%d%d.%d %%",H1/100,(H1/10)%10,H1%10);
	printf("\n\n");
	*/
	Show_OLED();
}

//ת���ַ�������� OLED ��
void Show_OLED(void)
{
	t = T1/100;
	switch(t)
	{
		case 0:break;
		case 1:strTemp1 = "1";break;
		case 2:strTemp1 = "2";break;
		case 3:strTemp1 = "3";break;
		case 4:strTemp1 = "4";break;
		case 5:strTemp1 = "5";break;
		case 6:strTemp1 = "6";break;
		case 7:strTemp1 = "7";break;
		case 8:strTemp1 = "8";break;
		case 9:strTemp1 = "9";break;
	}
	t = (T1/10)%10;
	switch(t)
	{
		case 0:strTemp2 = "0";break;
		case 1:strTemp2 = "1";break;
		case 2:strTemp2 = "2";break;
		case 3:strTemp2 = "3";break;
		case 4:strTemp2 = "4";break;
		case 5:strTemp2 = "5";break;
		case 6:strTemp2 = "6";break;
		case 7:strTemp2 = "7";break;
		case 8:strTemp2 = "8";break;
		case 9:strTemp2 = "9";break;
	}
	t = T1%10;
	switch(t)
	{
		case 0:strTemp3 = "0";break;
		case 1:strTemp3 = "1";break;
		case 2:strTemp3 = "2";break;
		case 3:strTemp3 = "3";break;
		case 4:strTemp3 = "4";break;
		case 5:strTemp3 = "5";break;
		case 6:strTemp3 = "6";break;
		case 7:strTemp3 = "7";break;
		case 8:strTemp3 = "8";break;
		case 9:strTemp3 = "9";break;
	}
	t = H1/100;
	switch(t)
	{
		case 0:break;
		case 1:strHumi1 = "1";break;
		case 2:strHumi1 = "2";break;
		case 3:strHumi1 = "3";break;
		case 4:strHumi1 = "4";break;
		case 5:strHumi1 = "5";break;
		case 6:strHumi1 = "6";break;
		case 7:strHumi1 = "7";break;
		case 8:strHumi1 = "8";break;
		case 9:strHumi1 = "9";break;
	}
	t = H1/100;
	switch(t)
	{
		case 0:strHumi2 = "0";break;
		case 1:strHumi2 = "1";break;
		case 2:strHumi2 = "2";break;
		case 3:strHumi2 = "3";break;
		case 4:strHumi2 = "4";break;
		case 5:strHumi2 = "5";break;
		case 6:strHumi2 = "6";break;
		case 7:strHumi2 = "7";break;
		case 8:strHumi2 = "8";break;
		case 9:strHumi2 = "9";break;
	}
	t = H1/100;
	switch(t)
	{
		case 0:strHumi3 = "0";break;
		case 1:strHumi3 = "1";break;
		case 2:strHumi3 = "2";break;
		case 3:strHumi3 = "3";break;
		case 4:strHumi3 = "4";break;
		case 5:strHumi3 = "5";break;
		case 6:strHumi3 = "6";break;
		case 7:strHumi3 = "7";break;
		case 8:strHumi3 = "8";break;
		case 9:strHumi3 = "9";break;
	}
	GUI_ShowString(40,32,"    ",16,1);
	GUI_ShowString(40,48,"    ",16,1);
  GUI_ShowCHinese(0,32,16,"�¶ȣ�",1);
	GUI_ShowString(40,32,strTemp1,16,1);
	GUI_ShowString(48,32,strTemp2,16,1);
	GUI_ShowString(56,32,".",16,1);
	GUI_ShowString(64,32,strTemp3,16,1);
	GUI_ShowCHinese(72,32,16,"��",1);
  GUI_ShowCHinese(0,48,16,"ʪ�ȣ�",1);
	GUI_ShowString(40,48,strHumi1,16,1);
	GUI_ShowString(48,48,strHumi2,16,1);
	GUI_ShowString(56,48,".",16,1);
	GUI_ShowString(64,48,strHumi3,16,1);
	GUI_ShowCHinese(72,48,16,"��",1);
}

//���� ACK �ź�
uint8_t Receive_ACK(void)
{
	uint8_t result=0;
	uint8_t cnt=0;

	//�� SCL �͵�ƽ
	IIC_SCL = 0;
	//���� SDA Ϊ��ȡ����ģʽ
	SDA_IN();
	delay_us(4);

	//�� SCL �ߵ�ƽ
	IIC_SCL = 1;
	delay_us(4);

	//�ȴ��ӻ����� ACK �źţ��ȴ�ʱ��Ϊ 100 ��ѭ��
	while(READ_SDA && (cnt<100))
	{
		cnt++;
	}

	IIC_SCL = 0;
	delay_us(4);

	//����ڵȴ�ʱ���ڣ�����Ϊ 1
	if(cnt<100)
	{
		result=1;
	}
	
	return result;
}

//���� ACK �ź�
void Send_ACK(void)
{
	//���� SDA Ϊд����ģʽ
	SDA_OUT();
	IIC_SCL = 0;
	delay_us(4);

	//�� SDA Ϊ�͵�ƽ
	IIC_SDA = 0;
	delay_us(4);

	IIC_SCL = 1;
	delay_us(4);
	IIC_SCL = 0;
	delay_us(4);

	SDA_IN();
}

//���� NACK �ź�
void SendNot_Ack(void)
{
	//���� SDA Ϊд����ģʽ
	SDA_OUT();
	
	IIC_SCL = 0;
	delay_us(4);

	IIC_SDA = 1;
	delay_us(4);

	IIC_SCL = 1;
	delay_us(4);

	IIC_SCL = 0;
	delay_us(4);

	IIC_SDA = 0;
	delay_us(4);
}

//����һ���ֽ�����
void I2C_WriteByte(uint8_t  input)
{
	uint8_t  i;
	//���� SDA Ϊд����ģʽ
	SDA_OUT();
	
	//ѭ�����Ʒ��� 8 bit����
	for(i=0; i<8; i++)
	{
		IIC_SCL = 0;
		delay_ms(5);

		if(input & 0x80)
		{
			IIC_SDA = 1;
		}
		else
		{
			IIC_SDA = 0;
		}

		IIC_SCL = 1;
		delay_ms(5);

		input = (input<<1);
	}

	IIC_SCL = 0;
	delay_us(4);

	SDA_IN();
	delay_us(4);
}	

//ѭ����� SDA �ĵ�ƽ״̬���洢����
uint8_t I2C_ReadByte(void)
{
	uint8_t  resultByte=0;
	uint8_t  i=0, a=0;

	IIC_SCL = 0;
	SDA_IN();
	delay_ms(4);

	//ѭ�����
	for(i=0; i<8; i++)
	{
		IIC_SCL = 1;
		delay_ms(3);

		a=0;
		if(READ_SDA)
		{
			a=1;
		}
		else
		{
			a=0;
		}

		resultByte = (resultByte << 1) | a;

		IIC_SCL = 0;
		delay_ms(3);
	}

	SDA_IN();
	delay_ms(10);

	return   resultByte;
}

//���� I2C Э�鿪ʼ
void I2C_Start(void)
{
	SDA_OUT();
	
	IIC_SCL = 1;
	delay_ms(4);

	//SDA �� 1 ����Ϊ 0 ���������
	//��ʾ��ʼ�ź�
	IIC_SDA = 1;
	delay_ms(4);
	IIC_SDA = 0;
	delay_ms(4);

	//SCL ��Ϊ 0
	//��ʾ SDA ������Ч����ʱ SDA ���Խ��е�ƽ�л�
	IIC_SCL = 0;
	delay_ms(4);
}

//���� I2C Э��ֹͣ
void I2C_Stop(void)
{
	SDA_OUT();
	
	//SCL �ߵ�ƽ��SDA �ߵ�ƽ
	//ֹͣʱ��
	IIC_SDA = 0;
	delay_ms(4);
	IIC_SCL = 1;
	delay_ms(4);

	//SDA �л����ߵ�ƽ
	IIC_SDA = 1;
	delay_ms(4);
}

