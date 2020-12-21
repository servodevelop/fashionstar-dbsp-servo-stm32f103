#ifndef __FASHION_STAR_DBSP_SERVO_H
#define __FASHION_STAR_DBSP_SERVO_H

#include "stm32f10x.h"
#include "usart.h"
#include "sys_tick.h"
#include "ring_buffer.h"

// 在串口舵机的通信系统设计里, 使用的字节序为Little Endian(低字节序/小端格式)
// STM32系统的数值存储模式就是Little Endian, 在实际发送的时候低位会在前面
// [0x13 0x4c 0x05 0x1d]
#define DBSP_PACK_HEADER (uint32_t)0x1d054c13
// 请求与响应代码
#define DBSP_REQUEST_SET_PROTOCAL (uint16_t)10010 		// 选择通信协议
#define DBSP_RESPONSE_SET_PROTOCAL (uint16_t)10011		
#define DBSP_REQUEST_RUN_MARCO (uint16_t)30090  		// 执行Marco

// 等待超时判断条件 ms
#define DBSP_TIMEOUT_MS 100
// 数据包的最大长度
#define DBSP_PACK_MAX_SIZE  100
// 接收数据帧的状态码
#define DBSP_STATUS uint8_t
#define DBSP_STATUS_SUCCESS 0 // 设置/读取成功
#define DBSP_STATUS_FAIL 1 // 设置/读取失败
#define DBSP_STATUS_TIMEOUT 2 // 等待超时 
#define DBSP_STATUS_WRONG_RESPONSE_HEADER 3 // 响应头不对
#define DBSP_STATUS_UNKOWN_CMD_ID 4 // 未知的控制指令
#define DBSP_STATUS_SIZE_TOO_BIG 5 // 参数的size大于DBSP_PACK_RESPONSE_MAX_SIZE里面的限制
#define DBSP_STATUS_CHECKSUM_ERROR 6 // 校验和错误
#define DBSP_STATUS_ID_NOT_MATCH 7 // 请求的舵机ID跟反馈回来的舵机ID不匹配

// 数据接收标志位
// 帧头接收完成的标志位
#define DBSP_RECV_FLAG_HEADER 0x01
// 控制指令接收完成的标志位
#define DBSP_RECV_FLAG_CODE 0x02
// 内容长度接收完成的标志位
#define DBSP_RECV_FLAG_SIZE 0x04
// 内容接收完成的标志位
#define DBSP_RECV_FLAG_CONTENT 0x08
// 校验和接收的标志位
#define DBSP_RECV_FLAG_CHECKSUM 0x10

// 通信协议模式
#define DBSP_PROTOCAL_OLD 0x00
#define DBSP_PROTOCAL_NEW 0x01
// DBSP的数据帧的结构体
typedef struct{
    uint32_t header; // 请求头
    uint16_t code; // 功能代码
    uint16_t size; // 包的长度
    uint8_t content[DBSP_PACK_MAX_SIZE]; // 包的内容
    uint8_t checksum; // 校验和
    // 用于帧的接收
    // 数据帧的接收状态 flag标志位
    uint8_t status; 
}DBSP_PackageTypeDef;


// 数据帧转换为字节数组
void DBSP_Package2RingBuffer(DBSP_PackageTypeDef *pkg,  RingBufferTypeDef *ringBuf);

// 销毁数据帧 (释放数据帧申请的内存)
void DBSP_DestroyPackage(DBSP_PackageTypeDef *pkg);

// 计算校验和
uint8_t DBSP_CalcChecksum(DBSP_PackageTypeDef *pkg);

// 判断是否为有效的请求头的
DBSP_STATUS DBSP_IsValidResponsePackage(DBSP_PackageTypeDef *pkg);

// 字节数组转换为数据帧
DBSP_STATUS DBSP_RingBuffer2Package(RingBufferTypeDef *ringBuf, DBSP_PackageTypeDef *pkg);

// 构造发送数据帧
void DBSP_SendPackage(Usart_DataTypeDef *usart, uint16_t code, uint16_t size, uint8_t *content);

// 接收数据帧 (在接收的时候动态的申请内存)
DBSP_STATUS DBSP_RecvPackage(Usart_DataTypeDef *usart, DBSP_PackageTypeDef *pkg);

// 设置通信协议模式
bool DBSP_SetProtocal(Usart_DataTypeDef *usart, uint8_t protocalId);

// 执行Marco动作组
void DBSP_RunMarco(Usart_DataTypeDef *usart, uint32_t marcoId);


#endif
