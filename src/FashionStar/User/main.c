/*
 * Marco发送
 * --------------------------
 * 作者: 阿凯|Kyle
 * 邮箱: xingshunkai@qq.com
 * 更新时间: 2020/05/26
 */
 
#include "stm32f10x.h"
#include "usart.h"
#include "sys_tick.h"
#include "fashion_star_dbsp_servo.h"

// 巨集的配置文件, 需要跟DBSP工程文件同步
#define MARCO0_ID (uint32_t)1513264628 // 动作组Marco0的ID号
#define MARCO0_INTERVAL 2000 // 动作组Marco0的周期,单位ms

#define MARCO1_ID (uint32_t)251853078 // 动作组Marco1的ID号
#define MARCO1_INTERVAL 2000 // 动作组Marco1的周期,单位ms

#define MARCO2_ID (uint32_t)1946965611 // 动作组Marco2的ID号
#define MARCO2_INTERVAL 2000 // 动作组Marco2的周期,单位ms

/* 执行巨集0 */
void Marco0(void){
	// 发送Marco执行指令
	DBSP_RunMarco(servoUsart, MARCO0_ID);
	// 等待Marco执行完成
	delay_ms(MARCO0_INTERVAL);
}

/* 执行巨集1 */
void Marco1(void){
	// 发送Marco执行指令
	DBSP_RunMarco(servoUsart, MARCO1_ID);
	// 等待Marco执行完成
	delay_ms(MARCO1_INTERVAL);
}

/* 执行巨集2 */
void Marco2(void){
	// 发送Marco执行指令
	DBSP_RunMarco(servoUsart, MARCO2_ID);
	// 等待Marco执行完成
	delay_ms(MARCO2_INTERVAL);
}

int main (void)
{
	SysTick_Init(); 			// 嘀嗒定时器初始化
	Usart_Init(); 				// 串口初始化
	
	DBSP_SetProtocal(servoUsart, DBSP_PROTOCAL_NEW); // 选择DBSP的通信协议
	delay_ms(200);
	
	while (1){
		printf("Excute Marco0");
		Marco0();
		delay_ms(1000);

		printf("Excute Marco1");
		Marco1();
		delay_ms(1000);

		printf("Excute Marco2");
		Marco2();
		delay_ms(1000);
	}
}
