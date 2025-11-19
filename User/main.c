#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "CH_04.h"
#include "esp8266.h"

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//（2主，2从）：全局配置中断分组
	LanYa_Init();
	WIFI_Init();
	
	while (1)
	{

	}
}


