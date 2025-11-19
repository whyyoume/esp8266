#ifndef __SYSTICK_H__
#define __SYSTICK_H__
#include "stm32f10x.h"                  // Device header
#include "esp8266.h"
#include "CH_04.h"

extern uint8_t timeout_flag ;  // ≥¨ ±±Í÷æ

void SysTick_Init(void);
uint8_t WaitForRXNEWithTimer(uint32_t timeout_ms);

#endif

