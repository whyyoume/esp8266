#include "stm32f10x.h"                  // Device header
#include <stdarg.h>
#include <stdio.h> 
#include "CH_04.h"
#include <math.h>
#include <string.h>  // 用于 strtok分割、strcmp比对
#include <stdlib.h>  // 用于 atoi转换为int、atof转换为float

//9t,10r

uint8_t Usart1_RxFlag=0;
char RxPacket[RxPacket_Length];
void LanYa_Init(void){
	//使能外设
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	//初始化USART1的GPIO口
	GPIO_InitTypeDef LanYa_Usart_GPIO_InitStruct;
	LanYa_Usart_GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	LanYa_Usart_GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;
	LanYa_Usart_GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&LanYa_Usart_GPIO_InitStruct);//复用推挽输出
	
	LanYa_Usart_GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	LanYa_Usart_GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
	LanYa_Usart_GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&LanYa_Usart_GPIO_InitStruct);//上拉输入
	
	//初始化USART1
	USART_InitTypeDef USART1_InitStruct;
	USART1_InitStruct.USART_BaudRate=9600;
	USART1_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART1_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART1_InitStruct.USART_Parity=USART_Parity_No;
	USART1_InitStruct.USART_StopBits=USART_StopBits_1;
	USART1_InitStruct.USART_WordLength=USART_WordLength_8b;
	
	USART_Init(USART1,&USART1_InitStruct);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_InitTypeDef USART1_NVIC_InitStructure;
	USART1_NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	USART1_NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	USART1_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	USART1_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&USART1_NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
	
}
void Test(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	//初始化USART1的GPIO口
	GPIO_InitTypeDef TEST_GPIO_InitStruct;
	TEST_GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	TEST_GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
	TEST_GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&TEST_GPIO_InitStruct);//P0测试

}
void USART1_IRQHandler(void){
if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
{	
	uint8_t RxData = USART_ReceiveData(USART1);
	
	static uint8_t Rx_status=0;
	static uint8_t Index=0;
	
	if(Rx_status==0)//等待模式
	{
		if(RxData=='['&&Usart1_RxFlag==0)//收到开始包头，又接收允许：则开始接收
		{
			Index=0;
			Rx_status=1;
		}
	}
	else if(Rx_status==1)//接收模式-既-等待结束标志
	{
		if(RxData==']')
		{
			 RxPacket[Index]='\0';
			Rx_status=0;//退出回到等待模式
			Usart1_RxFlag=1;//收到完整数据包，置起标志位
		}
		else 
		{
			RxPacket[Index]=RxData;
			Index++;
		}
	
	}
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}

}


void Serial_SendByte(uint8_t Byte){

	USART_SendData(USART1,Byte);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)!=SET);
}

void Serial_SendString(char * string){
	uint8_t i;
	for(i=0;string[i]!='\0';i++){
	Serial_SendByte(string[i]);
	}
}

void Serial_printf(char* format,...){
	char str[String_MaxLength];
	va_list arg; 
	va_start(arg,format);
	vsprintf(str,format,arg);
	va_end(arg);
	Serial_SendString(str);
}

void Display(uint8_t x,uint8_t y,char* string,uint8_t fontsize){
Serial_printf("[display,%d,%d,%s,%d]",x,y,string,fontsize);
}
void Display_Clear(void){
Serial_printf("[display-clear]");

}

void Plot(uint8_t size,...){
	
	va_list args;
	va_start(args,size);
	
	Serial_printf("[plot,");
	for(uint8_t i=0;i<size;i++)
	{
		
		double val=va_arg(args,double);
		if(i==0)
		{
			Serial_printf("%f",val);
		}
		else
		{
			Serial_printf(",%f",val);
		}
	}
	Serial_printf("]");
	va_end(args);
}

void Plot_Clear(void){
	Serial_printf("[plot-clear]");
}






void LanYa_Interaction(void){
if(Usart1_RxFlag==1)
		{
			char* tag=strtok(RxPacket,",");
			if(strcmp(tag,"key")==0)
			{
				char *Name = strtok(NULL, ",");
				char *Action = strtok(NULL, ",");
				if (strcmp(Name, "1") == 0 && strcmp(Action, "down") == 0)
				{
					GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET);
				}
				else if(strcmp(Name, "1") == 0 && strcmp(Action, "up") == 0)
				{
					GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);
				}
			}
			else if(strcmp(tag,"slider")==0)
			{
				char *Name = strtok(NULL, ",");
				char *Value = strtok(NULL, ",");
				uint8_t IntValue=atoi(Value);
				float FloatValue=atof(Value);
				
				if (strcmp(Name, "1") == 0 )
				{
					Plot(1,FloatValue);
				}
			}
			else if(strcmp(tag,"joystick")==0)
			{
				
				float LH = atof(strtok(NULL, ","));
				float LV = atof(strtok(NULL, ","));
				float RH = atof(strtok(NULL, ","));
				float RV = atof(strtok(NULL, ","));
					Plot(4,LH,LV,RH,RV);
				
			}
			
			Usart1_RxFlag=0;
		}
}









