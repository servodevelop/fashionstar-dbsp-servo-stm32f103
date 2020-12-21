#include "sys.h"

// 注: 
void Sys_SysClockConfig(void){
	// 复位RCC寄存器
	RCC_DeInit();
	// 使能HSE(High Speed External) 
	// 选择使用高速外部时钟作为时钟源, 开发板上是8MHz的晶振
	RCC_HSEConfig(RCC_HSE_ON);
	// 等待HSE启动
	if(RCC_WaitForHSEStartUp() == ERROR){
		// HSE启动失败
		return;
	}
	
	//使能预取指，这是FLASH固件中的函数
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    //设置FLASH等待周期。  因为倍频成72M 所以等待两个周期。
    FLASH_SetLatency(FLASH_Latency_2);  
	
    // SYSCLK -> AHB 
	// AHB时钟频率最大为72M，所以不分频
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
	// AHB -> APB1 
	// APB1的输出时钟频率最大为36MHz, 所以设置分频系数为2
	RCC_PCLK1Config(RCC_HCLK_Div2);
    // AHB --> APB2
	// APB2输出时钟频率最大为72MHz,	所以不分频
	RCC_PCLK2Config(RCC_HCLK_Div1);
	
	//先配置锁相环 PLLCLK = HSE * 倍频因子
	// 8MHz * 9 = 72MHz
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    //使能PLL锁相环，开启倍频
	RCC_PLLCmd(ENABLE);        
    // RCC_FLAG_PLLRDY,
	// PLL:锁相环;  RDY:Ready 判断锁相环是否配置完成
	while ( RESET == RCC_GetFlagStatus(RCC_FLAG_PLLRDY) );  //等待PLL稳定
	// 选择系统时钟（选择锁相环输出， 配置SW开关）
    // 将锁相环输出的时钟信号作为系统时钟信号 72Mhz    
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    // 等待系统时钟源切换为PLLCLK
	// 0x00: HSI used as system clock
	// 0x04: HSE used as system clock
	// 0x08: PLL used as system clock
    while ( RCC_GetSYSCLKSource() != 0x08);
}
