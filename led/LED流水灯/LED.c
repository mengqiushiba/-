
//--------------APB2ʹ��ʱ�ӼĴ���------------------------
#define RCC_AP2BENR	*((unsigned volatile int*)0x40021018)
	//----------------GPIOA���üĴ��� ------------------------
#define GPIOA_CRL	*((unsigned volatile int*)0x40010800)
#define	GPIOA_ODR	*((unsigned volatile int*)0x4001080C)
//----------------GPIOB���üĴ��� ------------------------
#define GPIOB_CRH	*((unsigned volatile int*)0x40010C04)
#define	GPIOB_ODR	*((unsigned volatile int*)0x40010C0C)
//----------------GPIOC���üĴ��� ------------------------
#define GPIOC_CRH	*((unsigned volatile int*)0x40011004)
#define	GPIOC_ODR	*((unsigned volatile int*)0x4001100C)
//-------------------�򵥵���ʱ����-----------------------
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
	GPIOA_ODR=0x0<<5;		//PA5�͵�ƽ
	GPIOB_ODR=0x1<<9;		//PB9�ߵ�ƽ
	GPIOC_ODR=0x1<<14;		//PC14�ߵ�ƽ
}
void B_LED_LIGHT(){
	GPIOA_ODR=0x1<<5;		//PA5�ߵ�ƽ
	GPIOB_ODR=0x0<<9;		//PB9�͵�ƽ
	GPIOC_ODR=0x1<<14;		//PC14�ߵ�ƽ
}
void C_LED_LIGHT(){
	GPIOA_ODR=0x1<<5;		//PA5�ߵ�ƽ
	GPIOB_ODR=0x1<<9;		//PB9�ߵ�ƽ
	GPIOC_ODR=0x0<<14;		//PC14�͵�ƽ	
}
//------------------------������--------------------------
int main()
{
	int j=100;
	RCC_AP2BENR|=1<<2;			//APB2-GPIOA����ʱ��ʹ��
	RCC_AP2BENR|=1<<3;			//APB2-GPIOB����ʱ��ʹ��	
	RCC_AP2BENR|=1<<4;			//APB2-GPIOC����ʱ��ʹ��
	//�����д�����Ժ�Ϊ RCC_APB2ENR|=1<<3|1<<4;
	GPIOA_CRL&=0xFF0FFFFF;		//����λ ����	
	GPIOA_CRL|=0X00200000;		//PA5�������
	GPIOA_ODR|=1<<5;			//����PA5��ʼ��Ϊ��
	
	GPIOB_CRH&=0xFFFFFF0F;		//����λ ����	
	GPIOB_CRH|=0x00000020;		//PB9�������
	GPIOB_ODR|=0x1<<9;			//���ó�ʼ��Ϊ��
	
	GPIOC_CRH&=0xF0FFFFFF;		//����λ ����
	GPIOC_CRH|=0x02000000;   	//PC14�������
	GPIOC_ODR|=0x1<<14;			//���ó�ʼ��Ϊ��	
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


