
//--------------APB2使能时钟寄存器------------------------
#define RCC_AP2BENR	*((unsigned volatile int*)0x40021018)
	//----------------GPIOA配置寄存器 ------------------------
#define GPIOA_CRL	*((unsigned volatile int*)0x40010800)
#define	GPIOA_ODR	*((unsigned volatile int*)0x4001080C)
//----------------GPIOB配置寄存器 ------------------------
#define GPIOB_CRH	*((unsigned volatile int*)0x40010C04)
#define	GPIOB_ODR	*((unsigned volatile int*)0x40010C0C)
//----------------GPIOC配置寄存器 ------------------------
#define GPIOC_CRH	*((unsigned volatile int*)0x40011004)
#define	GPIOC_ODR	*((unsigned volatile int*)0x4001100C)
//-------------------简单的延时函数-----------------------
void SystemInit(void);
void Delay_ms(volatile  unsigned  int);
void A_LED_LIGHT(void);
void B_LED_LIGHT(void);
void C_LED_LIGHT(void);
void  Delay_ms( volatile  unsigned  int  t)
{
     unsigned  int  i;
     while(t--)
         for (i=0;i<800;i++);
}
void A_LED_LIGHT(){
	GPIOA_ODR=0x0<<5;		//PA5低电平
	GPIOB_ODR=0x1<<9;		//PB9高电平
	GPIOC_ODR=0x1<<14;		//PC14高电平
}
void B_LED_LIGHT(){
	GPIOA_ODR=0x1<<5;		//PA5高电平
	GPIOB_ODR=0x0<<9;		//PB9低电平
	GPIOC_ODR=0x1<<14;		//PC14高电平
}
void C_LED_LIGHT(){
	GPIOA_ODR=0x1<<5;		//PA5高电平
	GPIOB_ODR=0x1<<9;		//PB9高电平
	GPIOC_ODR=0x0<<14;		//PC14低电平	
}
//------------------------主函数--------------------------
int main()
{
	int j=100;
	RCC_AP2BENR|=1<<2;			//APB2-GPIOA外设时钟使能
	RCC_AP2BENR|=1<<3;			//APB2-GPIOB外设时钟使能	
	RCC_AP2BENR|=1<<4;			//APB2-GPIOC外设时钟使能
	//这两行代码可以合为 RCC_APB2ENR|=1<<3|1<<4;
	GPIOA_CRL&=0xFF0FFFFF;		//设置位 清零	
	GPIOA_CRL|=0X00200000;		//PA5推挽输出
	GPIOA_ODR|=1<<5;			//设置PA5初始灯为灭
	
	GPIOB_CRH&=0xFFFFFF0F;		//设置位 清零	
	GPIOB_CRH|=0x00000020;		//PB9推挽输出
	GPIOB_ODR|=0x1<<9;			//设置初始灯为灭
	
	GPIOC_CRH&=0xF0FFFFFF;		//设置位 清零
	GPIOC_CRH|=0x02000000;   	//PC14推挽输出
	GPIOC_ODR|=0x1<<14;			//设置初始灯为灭	
	while(j)
	{	
		A_LED_LIGHT();	
		Delay_ms(1000000);
		B_LED_LIGHT();
		Delay_ms(1000000);
		C_LED_LIGHT();
		Delay_ms(1000000);
	}
}
void SystemInit(){
	
}


