#ifndef __CH04_H__
#define __CH04_H__


#define String_MaxLength 100
#define Plot_Number_Length 10
#define RxPacket_Length 100//若大于256则需要修改此处保证索引范围适合：static uint8_t Index=0;
extern char RxPacket[];
extern uint8_t RxFlag;

void LanYa_Init(void);
void Test(void);

void Serial_printf(char* format,...);
void Display(uint8_t x,uint8_t y,char* string,uint8_t fontsize);
void Display_Clear(void);
void Plot(uint8_t size,...);
void Plot_Clear(void);

void LanYa_Interaction(void);
#endif

