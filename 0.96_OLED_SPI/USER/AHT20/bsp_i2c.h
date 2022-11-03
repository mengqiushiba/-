#ifndef __BSP_I2C_H
#define __BSP_I2C_H

#include "AHT20_sys.h"
 
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
//CRL = 0000 1111 1111 1111 1111 1111 1111 1111
//8<<28 = 1000 1111 1111 1111 1111 1111 1111 1111
//CRL = 1000 1111 1111 1111 1111 1111 1111 1111 = 0x8fffffff 表示 SDA 输入
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}
//CRL = 0x3fffffff 表示 SDA 输出


#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	 
#define READ_SDA   PBin(7)  //SDA 数据读取 7 管脚


void IIC_Init(void);
void  read_AHT20_once(void);
void  reset_AHT20(void);
void  init_AHT20(void);	
void  startMeasure_AHT20(void);
void  read_AHT20(void);
uint8_t  Receive_ACK(void);
void  Send_ACK(void);
void  SendNot_Ack(void);
void I2C_WriteByte(uint8_t  input);
uint8_t I2C_ReadByte(void);	
void  set_AHT20sendOutData(void);
void  I2C_Start(void);
void  I2C_Stop(void);
void Show_OLED(void);

#endif

