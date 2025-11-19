#include "esp8266.h"
//b10_T
uint8_t WIFI_R_Flag=0;
void WIFI_Init(void){
	//使能外设
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	//初始化USART3的GPIO口b10,b11
	GPIO_InitTypeDef WIFI_Usart_GPIO_InitStruct;
	WIFI_Usart_GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	WIFI_Usart_GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
	WIFI_Usart_GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&WIFI_Usart_GPIO_InitStruct);//复用推挽输出
	
	WIFI_Usart_GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	WIFI_Usart_GPIO_InitStruct.GPIO_Pin=GPIO_Pin_11;
	WIFI_Usart_GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&WIFI_Usart_GPIO_InitStruct);//上拉输入
	
	//初始化USART1
	USART_InitTypeDef USART3_InitStruct;
	USART3_InitStruct.USART_BaudRate=WIFI_BaudRate;
	USART3_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART3_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART3_InitStruct.USART_Parity=USART_Parity_No;
	USART3_InitStruct.USART_StopBits=USART_StopBits_1;
	USART3_InitStruct.USART_WordLength=USART_WordLength_8b;
	
	USART_Init(USART3,&USART3_InitStruct);
	//中断使能
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);   //使能串口接收中断 
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);   //使能串口总线空闲中断 	
	
	NVIC_InitTypeDef USART3_NVIC_InitStructure;
	USART3_NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	USART3_NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	USART3_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	USART3_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&USART3_NVIC_InitStructure);
	
	USART_Cmd(USART3, ENABLE);
	Delay_ms(100);
	WIFI_Check();
}
char WIFI_RxPacket[RxPacket_Length];

void USART3_IRQHandler(void)
{
	uint8_t Data;
	static uint32_t index=0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		if(WIFI_R_Flag==WIFI_RX_ON&&index<(RxPacket_Length-1))
		{
			if(index==0)
			{
				for(int i=0;i<RxPacket_Length;i++)
				{
							WIFI_RxPacket[i]=0;
				}
			}
			Data  = USART_ReceiveData( USART3 );
			WIFI_RxPacket[index++]=Data;
		}
		
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
	if(USART_GetITStatus(USART3, USART_IT_IDLE) == SET)//读取状态寄存器(USART_SR)
	{
		if(WIFI_R_Flag==WIFI_RX_ON)
		{
		WIFI_R_Flag=WIFI_RXNE;
			WIFI_RxPacket[index]='\0';
		index=0;
		}
		USART_ReceiveData(USART3);//读取数据寄存器(USART_DR)
		//软件序列清除标志位(先读USART_SR，然后读USART_DR)
	}
}

void WIFI_SendByte(uint8_t Byte){

	USART_SendData(USART3,Byte);
	while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)!=SET);
}

void WIFI_SendString(char * string){
	uint8_t i;
	for(i=0;string[i]!='\0';i++){
	WIFI_SendByte(string[i]);
	}
}

void WIFI_printf(char* format,...){
	char str[String_MaxLength];
	va_list arg; 
	va_start(arg,format);
	vsprintf(str,format,arg);
	va_end(arg);
	WIFI_SendString(str);
}

/*
 * 函数名：ESP8266_Cmd
 * 描述  ：对WF-ESP8266模块发送AT指令
 * 输入  ：cmd，待发送的指令
 *         reply1，reply2，期待的响应，为NULL表不需响应，两者为或逻辑关系
 *         waittime，等待响应的时间
 * 返回  : 1，指令发送成功
 *         0，指令发送失败
 * 调用  ：被外部调用
 */
Cmd_Rceive_Flag ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
		WIFI_R_Flag=WIFI_RX_ON;	//允许接收
		WIFI_printf( "%s\r\n", cmd );
	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
		return CMD_triggered;
	
	Delay_ms(waittime);
	if(WIFI_R_Flag==WIFI_RX_ON)
	return TIME_OUT;
	
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
	{
		if ( ( bool ) strstr ( WIFI_RxPacket, reply1 ) || 
				 ( bool ) strstr (WIFI_RxPacket , reply2 ) )
		{
			return CMD_triggered;
		}else{
			return CMD_ERROR;
		}
	}
	else if ( reply1 != 0 )
	{

		if( ( bool ) strstr (WIFI_RxPacket , reply1 ) )
			{
			return CMD_triggered;
		}else{
			return CMD_ERROR;
		}
	}
	
	else
	{
	if( ( bool ) strstr ( WIFI_RxPacket, reply2 ) )
	{
			return CMD_triggered;
		}else{
			return CMD_ERROR;
		}
	}
	
}

//*****************************操作******************/
void WIFI_Check(void){
Cmd_Rceive_Flag res = ESP8266_Cmd("AT", "OK", NULL, 1000);
if(res == CMD_triggered) Serial_printf("WIFI ON\r\n");
else if(res == TIME_OUT) Serial_printf("WIFI TIME_OUT\r\n");
else Serial_printf("WIFI ERROR\r\n");
}
//AT \r\n \r\n OK \r\n

void WIFI_Interaction(void)
{
	if(WIFI_R_Flag)
	{
		Serial_printf("%s",WIFI_RxPacket);
	WIFI_R_Flag=0;
	}
}

