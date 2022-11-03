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
 *初始化 I2C 函数
 ****************/
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//启用高速 APB (APB2) 外围时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );	
	
	//GPIO 定义
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//初始化 SCL（Pin6）高电平
	IIC_SCL=1;
	//初始化 SDA（Pin7）高电平
	IIC_SDA=1;
}

/*********************
 *AHT20 数据操作总函数
 *********************/
void read_AHT20_once(void)
{

	//延时 10 微妙
	delay_ms(10);
	
  //传输数据前进行启动传感器和软复位
	reset_AHT20();
	delay_ms(10);
  
	//查看使能位
	init_AHT20();
	delay_ms(10);
	
  //触发测量
	startMeasure_AHT20();
	delay_ms(80);
  
	//读数据
	read_AHT20();
	delay_ms(5);
}


void reset_AHT20(void)
{
	//数据传输开始信号
	I2C_Start();
	
	//发送数据
	I2C_WriteByte(0x70);
	//接收 ACK 信号
	ack_status = Receive_ACK();

	//发送软复位命令（重启传感器系统）
	I2C_WriteByte(0xBA);
	//接收 ACK 信号
	ack_status = Receive_ACK();

	//停止 I2C 协议
	I2C_Stop();
}

//0x70 —> 0111 0000 前七位表示 I2C 地址，第八位为0，表示 write
//0xE1 —> 看状态字的校准使能位Bit[3]是否为 1
//0x08 0x00 —> 0xBE 命令的两个参数，详见 AHT20 参考手册
void init_AHT20(void)
{
	//传输开始
	I2C_Start();

	//写入 0x70 数据
	I2C_WriteByte(0x70);
	//接收 ACK 信号
	ack_status = Receive_ACK();

	//写入 0xE1 数据
	I2C_WriteByte(0xE1);
	ack_status = Receive_ACK();

	//写入 0x08 数据
	I2C_WriteByte(0x08);
	ack_status = Receive_ACK();

	//写入 0x00 数据
	I2C_WriteByte(0x00);
	ack_status = Receive_ACK();

	//停止 I2C 协议
	I2C_Stop();
}

//0x70 —> 0111 0000 前七位表示 I2C 地址，第八位为0，表示 write
//0xAC —> 触发测量
//0x33 0x00 —> 0xAC 命令的两个参数，详见 AHT20 参考手册
void startMeasure_AHT20(void)
{
	//启动 I2C 协议
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

	//初始化 readByte 数组
	for(i=0; i<6; i++)
	{
		readByte[i]=0;
	}

	I2C_Start();

	//通过发送 0x71 可以获取一个字节的状态字
	I2C_WriteByte(0x71);
	ack_status = Receive_ACK();
	
	//接收 6 个 8 bit的数据
	readByte[0]= I2C_ReadByte();
	//发送 ACK 信号
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
	//发送 NACK 信号
	SendNot_Ack();

	I2C_Stop();

	//温湿度的二进制数据处理
	//0x68 = 0110 1000
  //0x08 = 0000 1000	
	if( (readByte[0] & 0x68) == 0x08 )
	{
		H1 = readByte[1];
		//H1 左移 8 位并与 readByte[2] 相或 
		H1 = (H1<<8) | readByte[2];
		H1 = (H1<<8) | readByte[3];
		//H1 右移 4 位
		H1 = H1>>4;

		H1 = (H1*1000)/1024/1024;

		T1 = readByte[3];
		//与运算
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
	printf("完成！\n");
	printf("----温度:%d%d.%d °C\n",T1/100,(T1/10)%10,T1%10);
	printf("----湿度:%d%d.%d %%",H1/100,(H1/10)%10,H1%10);
	printf("\n\n");
	*/
	Show_OLED();
}

//转化字符串输出到 OLED 上
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
  GUI_ShowCHinese(0,32,16,"温度：",1);
	GUI_ShowString(40,32,strTemp1,16,1);
	GUI_ShowString(48,32,strTemp2,16,1);
	GUI_ShowString(56,32,".",16,1);
	GUI_ShowString(64,32,strTemp3,16,1);
	GUI_ShowCHinese(72,32,16,"℃",1);
  GUI_ShowCHinese(0,48,16,"湿度：",1);
	GUI_ShowString(40,48,strHumi1,16,1);
	GUI_ShowString(48,48,strHumi2,16,1);
	GUI_ShowString(56,48,".",16,1);
	GUI_ShowString(64,48,strHumi3,16,1);
	GUI_ShowCHinese(72,48,16,"％",1);
}

//接收 ACK 信号
uint8_t Receive_ACK(void)
{
	uint8_t result=0;
	uint8_t cnt=0;

	//置 SCL 低电平
	IIC_SCL = 0;
	//设置 SDA 为读取数据模式
	SDA_IN();
	delay_us(4);

	//置 SCL 高电平
	IIC_SCL = 1;
	delay_us(4);

	//等待从机发送 ACK 信号，等待时间为 100 个循环
	while(READ_SDA && (cnt<100))
	{
		cnt++;
	}

	IIC_SCL = 0;
	delay_us(4);

	//如果在等待时间内，则结果为 1
	if(cnt<100)
	{
		result=1;
	}
	
	return result;
}

//发送 ACK 信号
void Send_ACK(void)
{
	//设置 SDA 为写数据模式
	SDA_OUT();
	IIC_SCL = 0;
	delay_us(4);

	//置 SDA 为低电平
	IIC_SDA = 0;
	delay_us(4);

	IIC_SCL = 1;
	delay_us(4);
	IIC_SCL = 0;
	delay_us(4);

	SDA_IN();
}

//发送 NACK 信号
void SendNot_Ack(void)
{
	//设置 SDA 为写数据模式
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

//发送一个字节数据
void I2C_WriteByte(uint8_t  input)
{
	uint8_t  i;
	//设置 SDA 为写数据模式
	SDA_OUT();
	
	//循环左移发送 8 bit数据
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

//循环检测 SDA 的电平状态并存储起来
uint8_t I2C_ReadByte(void)
{
	uint8_t  resultByte=0;
	uint8_t  i=0, a=0;

	IIC_SCL = 0;
	SDA_IN();
	delay_ms(4);

	//循环检测
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

//设置 I2C 协议开始
void I2C_Start(void)
{
	SDA_OUT();
	
	IIC_SCL = 1;
	delay_ms(4);

	//SDA 从 1 跳变为 0 的这个过程
	//表示起始信号
	IIC_SDA = 1;
	delay_ms(4);
	IIC_SDA = 0;
	delay_ms(4);

	//SCL 变为 0
	//表示 SDA 数据无效，此时 SDA 可以进行电平切换
	IIC_SCL = 0;
	delay_ms(4);
}

//设置 I2C 协议停止
void I2C_Stop(void)
{
	SDA_OUT();
	
	//SCL 高电平，SDA 高电平
	//停止时序
	IIC_SDA = 0;
	delay_ms(4);
	IIC_SCL = 1;
	delay_ms(4);

	//SDA 切换到高电平
	IIC_SDA = 1;
	delay_ms(4);
}

