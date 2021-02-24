/*
 * 系统时间管理
 * --------------------------
 * 作者: 阿凯|Kyle
 * 邮箱: kyle.xing@fashionstar.com.hk
 * 更新时间: 2020/08/16
 */
#include "sys_tick.h"

// 比例系数
static uint32_t systickCntPerMs = 0; // 1ms对应的滴答计数器的计数
static uint32_t tCountDownBegin = 0; // 开始倒计时开始的时间
static uint32_t tCountDownPeriod = 0; // 倒计时的时长

// 记录系统的时间 计时器
static __IO u32 sysTimeStampMs; // 系统的时间戳, 累加

// 系统定时器初始化
void SysTick_Init(void){
	// SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);	//选择外部时钟  HCLK
	// 计算比例系数
	systickCntPerMs = (uint32_t)((SystemCoreClock / 8) / 1000); // 1ms对应的滴答计数器的计数		
	// systickCntPerMs = (uint32_t)(SystemCoreClock / 1000); // 1ms对应的滴答计数器的计数		
    // 这里设置为1ms中断一次
	if(SysTick_Config(systickCntPerMs)){
		// 捕获异常
		while(1);
	}
	
	// 因为AHB预分频器为1, 所以系统时钟的频率与HCLK的频率相同都是72MHz
	// Systick频率最高就是9Mhz,因此需要设置滴答定时器的时钟源为HCLK分频8
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	
	// 滴答定时器开启
	SysTick_Enable(true);
}

/* 使能SysTick*/
void SysTick_Enable(bool enable){
	if(enable){
		// 定时器使能
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	}else{
		// 定时器失能
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	}
}

// 延时us
void SysTick_DelayUs(__IO TimeTypedef nTime){
	// 普通延时
	u16 i=0;  
	while(nTime--)
	{
		i=10;
		while(i--) ;    
	}
}

/* 延时us(简写)*/
void delay_us(TimeTypedef nTime){
	SysTick_DelayUs(nTime);
}

/* 计算举例上次经过的时间,单位ms */
TimeTypedef SysTick_TimePassMs(TimeTypedef tStart){
	int64_t tPass	= (SysTick_Millis() - tStart);
	if (tPass < 0){
		tPass = 4294967296 + tPass; // 注: 4294967296 = 2^32
	}
	return (TimeTypedef)tPass;
}	
	
	
// 延时ms
void SysTick_DelayMs(__IO TimeTypedef nTime){
	TimeTypedef tDelayBegin = SysTick_Millis(); // 获取当前的系统时间
	while(true){
		if(SysTick_TimePassMs(tDelayBegin) >= nTime){
			break;
		}
	};
}

// 延时ms，简写
void delay_ms(TimeTypedef nTime){
	SysTick_DelayMs(nTime);
}

// 延时s
void SysTick_DelayS(__IO TimeTypedef nTime){
	for (int i=0; i<nTime; i++){
		SysTick_DelayMs(1000);
	}
}

void delay(TimeTypedef nTime){
	SysTick_DelayS(nTime);
}

// 设置倒计时(非阻塞式)
void SysTick_CountdownBegin(__IO TimeTypedef nTime){
    tCountDownBegin = SysTick_Millis();
	tCountDownPeriod = nTime;
}

// 撤销倒计时
void SysTick_CountdownCancel(void){
    tCountDownPeriod = 0;
}

// 判断倒计时是否超时
uint8_t SysTick_CountdownIsTimeout(void){
    return  (SysTick_Millis() - tCountDownBegin) > tCountDownPeriod;
}

// 获取系统的时间戳
TimeTypedef SysTick_Millis(void){
	return sysTimeStampMs;
}

TimeTypedef millis(void){
	return SysTick_Millis();
}

// 设置系统定时器中断的回调函数
void SysTick_Handler(void)
{
	// 时间戳累加1
	sysTimeStampMs += 1; // 系统时间戳累加
}

