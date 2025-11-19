
#include "systick.h"
uint8_t timeout_flag = 0;  // 超时标志

volatile uint32_t ms_count = 0;  // 毫秒计数器
// 初始化SysTick定时器，每隔1ms触发一次中断
void SysTick_Init(void) {
    // 假设CPU主频72MHz，1ms = 72000个时钟周期
    SysTick_Config(SystemCoreClock / 1000);  // 1ms中断一次
  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;  // 先关闭中断，用时再开启  
}
// SysTick中断服务函数：每1ms将超时标志置1
void SysTick_Handler(void) {
    ms_count++;	
}


uint8_t WaitForRXNEWithTimer(uint32_t timeout_ms) {
		ms_count=0;
		SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  // 开启SysTick中断

    while (1) {
        // 条件1：RXNE标志触发
//        if (WIFI_R_Flag == WIFI_RXNE) {
//					Serial_printf("%d",ms_count);
//					  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;  // 先关闭中断，用时再开启  

//            return 1;
//        }
        // 条件2：定时器超时（每1ms检查一次）
				if (ms_count >= timeout_ms) {
							  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;  // 先关闭中断，用时再开启  
                return 0;
            }
        
    }
}

