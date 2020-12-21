/*
 * 系统时间管理
 * --------------------------
 * 作者: 阿凯|Kyle
 * 邮箱: xingshunkai@qq.com
 * 更新时间: 2020/05/19
 */
#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"
#include "stdbool.h"

// 时间的格式
#define TimeTypedef u32

// 系统定时器初始化
void SysTick_Init(void);

// Systic失能
void SysTick_Enable(bool enable);

// 延时
// 延时 us
void SysTick_DelayUs(__IO TimeTypedef nTime);

// 延时 us(简写)
void delay_us(TimeTypedef nTime);

// 获取经过了多少时间
TimeTypedef SysTick_TimePassMs(TimeTypedef tStart);

// 延时 ms
void SysTick_DelayMs(__IO TimeTypedef nTime);

// 延时 ms(简写)
void delay_ms(TimeTypedef nTime);

// 延时 s
void SysTick_DelayS(__IO TimeTypedef nTime);

// 延时 s(简写)
void delay(TimeTypedef nTime);

// 倒计时
// 设置倒计时(非阻塞式)
void SysTick_CountdownBegin(__IO TimeTypedef nTime);

// 撤销倒计时
void SysTick_CountdownCancel(void);

// 判断倒计时是否超时
uint8_t SysTick_CountdownIsTimeout(void);

// 获取当前的时间戳
TimeTypedef SysTick_Millis(void);

// 获取当前的时间戳(简写)
TimeTypedef millis(void);

#endif
