#include "stm32f10x.h"                  // Device header
#include "Delay.h"

#include "Serial.h"

#include "string.h"
int flag=1;
int main(void)
{

	Serial_Init();
	
	
	
	while (1)
	{
		
		if (Serial_RxFlag == 1)
		{
			
			
			if (strcmp(Serial_RxPacket, "stop stm32!") == 0)
			{
				flag=0;
				printf("stop!");
			}
			else if (strcmp(Serial_RxPacket, "go stm32!") == 0)
			{
				flag=1;
				printf("go");
			}
			
			printf("over");
			Serial_RxFlag = 0;
		}

         if(flag==1)
					 
				 {printf("hello window!");
					 Delay_ms(1000);
				 }


	}
}
