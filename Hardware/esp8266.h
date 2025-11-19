#ifndef __ESP8266_H__
#define __ESP8266_H__
#include "stm32f10x.h"                  // Device header
#include "CH_04.h"
#include "Delay.h"


#include <stdarg.h>
#include <stdio.h> 
#include <math.h>
#include <string.h>  // 用于 strtok分割、strcmp比对,strstr查找字符串中的
#include <stdbool.h>

#define String_MaxLength 100//Serial_printf发送函数的MAX发送字符串长度
#define RxPacket_Length 100//若大于256则需要修改此处保证索引范围适合：static uint8_t Index=0;
#define WIFI_BaudRate 115200

#define WIFI_RX_ON 0
#define WIFI_RXNE 1
extern uint8_t WIFI_R_Flag;//WIFI_RX_ON允许接收
extern char WIFI_RxPacket[];

typedef enum {
	TIME_OUT,
	CMD_triggered,
	CMD_ERROR,
}Cmd_Rceive_Flag;

void WIFI_Init(void);
void WIFI_printf(char* format,...);

void WIFI_Check(void);
void WIFI_Interaction(void);//自带标志位清除

Cmd_Rceive_Flag ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime );
#endif

