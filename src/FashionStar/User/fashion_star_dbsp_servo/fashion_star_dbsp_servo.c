#include "fashion_star_dbsp_servo.h"


// 数据帧转换为字节数组
void DBSP_Package2RingBuffer(DBSP_PackageTypeDef *pkg,  RingBufferTypeDef *ringBuf){
    uint8_t checksum; // 校验和
    // 写入帧头
    RingBuffer_WriteULong(ringBuf, pkg->header);
    // 写入指令ID
    RingBuffer_WriteUShort(ringBuf, pkg->code);
    // 写入包的长度
    RingBuffer_WriteUShort(ringBuf, pkg->size);
    // 写入内容主题
    RingBuffer_WriteByteArray(ringBuf, pkg->content, pkg->size);
    // 计算校验和
    checksum = RingBuffer_GetChecksum(ringBuf);
    // 写入校验和
    RingBuffer_WriteByte(ringBuf, checksum);

}

// 计算Package的校验和
uint8_t DBSP_CalcChecksum(DBSP_PackageTypeDef *pkg){
    uint8_t checksum;
	// 初始化环形队列
	RingBufferTypeDef ringBuf;
	uint8_t pkgBuf[DBSP_PACK_MAX_SIZE+1];
	RingBuffer_Init(&ringBuf, DBSP_PACK_MAX_SIZE, pkgBuf);
    // 将Package转换为ringbuffer
	// 在转换的时候,会自动的计算checksum
    DBSP_Package2RingBuffer(pkg, &ringBuf);
	// 获取环形队列队尾的元素(即校验和的位置)
	checksum = RingBuffer_GetValueByIndex(&ringBuf, RingBuffer_GetByteUsed(&ringBuf)-1);
    return checksum;
}

// 判断是否为有效的请求头的
DBSP_STATUS DBSP_IsValidResponsePackage(DBSP_PackageTypeDef *pkg){
    // 帧头数据不对
    if (pkg->header != DBSP_PACK_HEADER){
        // 帧头不对
        return DBSP_STATUS_WRONG_RESPONSE_HEADER;
    }
    // 校验和不匹配
    if (DBSP_CalcChecksum(pkg) != pkg->checksum){
        return DBSP_STATUS_CHECKSUM_ERROR;
    }
    // 数据有效
    return DBSP_STATUS_SUCCESS;
}

// 字节数组转换为数据帧
DBSP_STATUS DBSP_RingBuffer2Package(RingBufferTypeDef *ringBuf, DBSP_PackageTypeDef *pkg){
    // 申请内存
    pkg = (DBSP_PackageTypeDef *)malloc(sizeof(DBSP_PackageTypeDef));
    // 读取帧头
    pkg->header = RingBuffer_ReadULong(ringBuf);
    // 读取指令ID
    pkg->code = RingBuffer_ReadUShort(ringBuf);
    // 读取包的长度
    pkg->size = RingBuffer_ReadUShort(ringBuf);
    // 写入content
    RingBuffer_ReadByteArray(ringBuf, pkg->content, pkg->size);
    // 写入校验和
    pkg->checksum = RingBuffer_ReadByte(ringBuf);
    // 返回当前的数据帧是否为有效反馈数据帧
    return DBSP_IsValidResponsePackage(pkg);
}

// 构造发送数据帧
void DBSP_SendPackage(Usart_DataTypeDef *usart, uint16_t code, uint16_t size, uint8_t *content){
    // 申请内存
	// printf("[Package] malloc for pkg\r\n");
    DBSP_PackageTypeDef pkg;
	
    // 设置帧头
    pkg.header = DBSP_PACK_HEADER;
    // 设置指令ID
    pkg.code = code;
    // 设置尺寸
    pkg.size = size;
	// 逐一拷贝数组里面的内容
	for(int i=0; i<size; i++){
		pkg.content[i] = content[i];
	}
    // 将pkg发送到发送缓冲区sendBuf里面
    DBSP_Package2RingBuffer(&pkg, usart->sendBuf);
	// 通过串口将数据发送出去
    Usart_SendAll(usart);
}

// 接收数据帧 (在接收的时候动态的申请内存)
DBSP_STATUS DBSP_RecvPackage(Usart_DataTypeDef *usart, DBSP_PackageTypeDef *pkg){
	
	pkg->status = 0; // Package状态初始化
    uint8_t bIdx = 0; // 接收的参数字节索引号
    uint32_t header = 0; // 帧头
	
    // 倒计时开始
    SysTick_CountdownBegin(DBSP_TIMEOUT_MS);

    // 如果没有超时
    while (!SysTick_CountdownIsTimeout()){
        if (RingBuffer_GetByteUsed(usart->recvBuf) == 0){
			// 没有新的字节读进来, 继续等待
            continue;
        }

        // 查看校验码是否已经接收到
        if(pkg->status & DBSP_RECV_FLAG_CONTENT){
            // 参数内容接收完成
            // 接收校验码
            pkg->checksum = RingBuffer_ReadByte(usart->recvBuf);
            // 设置状态码-校验码已经接收
            pkg->status = pkg->status | DBSP_RECV_FLAG_CHECKSUM;
            // 直接进行校验码检查
            if (DBSP_CalcChecksum(pkg) != pkg->checksum){
                // 撤销倒计时
                SysTick_CountdownCancel();
                // 校验码错误
                return DBSP_STATUS_CHECKSUM_ERROR;
            }else{
                // 撤销倒计时
                SysTick_CountdownCancel();
                // 数据帧接收成功
                return DBSP_STATUS_SUCCESS;
            }
        }else if(pkg->status & DBSP_RECV_FLAG_SIZE){
            // Size已经接收完成
            // 接收参数字节
            pkg->content[bIdx] = RingBuffer_ReadByte(usart->recvBuf);
            bIdx ++;
            // 判断是否接收完成
            if (bIdx == pkg->size){
                // 标记为参数接收完成
                pkg->status = pkg->status | DBSP_RECV_FLAG_CONTENT;
            }
        }else if(pkg->status & DBSP_RECV_FLAG_CODE){
            // 指令接收完成
            // 接收尺寸信息
            pkg->size = RingBuffer_ReadUShort(usart->recvBuf);
            // 设置尺寸接收完成的标志位
            pkg->status = pkg->status | DBSP_RECV_FLAG_SIZE;
        }else if(pkg->status & DBSP_RECV_FLAG_HEADER){
            // 帧头已接收 
            // 接收指令ID
            pkg->code = RingBuffer_ReadUShort(usart->recvBuf);
            // 判断指令是否合法
            // 判断控制指令是否有效 指令范围超出
            if (pkg->code > 10){
                // 撤销倒计时
                SysTick_CountdownCancel();
                return DBSP_STATUS_UNKOWN_CMD_ID;
            }
            // 设置code已经接收到标志位
            pkg->status = pkg->status | DBSP_RECV_FLAG_CODE;
        }else{
            // 接收帧头
            if (header == 0){
                // 接收第一个字节
                header = RingBuffer_ReadByte(usart->recvBuf);
                // 判断接收的第一个字节对不对
                if (header != (DBSP_PACK_HEADER&0x000F)){
                    // 第一个字节不对 header重置为0
                    header = 0;
                }
            }else if(header == (DBSP_PACK_HEADER&0x000F)){
                // 接收帧头第二个字节
                header =  header | (RingBuffer_ReadByte(usart->recvBuf) << 8);
                // 判断接收的第一个字节对不对
                if (header != (DBSP_PACK_HEADER&0x00FF)){
                    // 第一个字节不对 header重置为0
                    header = 0;
                }
            }else if(header == (DBSP_PACK_HEADER&0x00FF)){
                // 接收帧头第三个字节
                header =  header | (RingBuffer_ReadByte(usart->recvBuf) << 16);
                 // 判断接收的第一个字节对不对
                if (header != (DBSP_PACK_HEADER&0x0FFF)){
                    // 第一个字节不对 header重置为0
                    header = 0;
                }
            }
            else if(header == (DBSP_PACK_HEADER&0x0FFF)){
                // 接收帧头第三个字节
                header =  header | (RingBuffer_ReadByte(usart->recvBuf) << 24);
                 // 判断接收的第一个字节对不对
                if (header != (DBSP_PACK_HEADER)){
                    // 第一个字节不对 header重置为0
                    header = 0;
                }else{
                    pkg->header = header;
                    // 帧头接收成功
                    pkg->status = pkg->status | DBSP_RECV_FLAG_HEADER; 
                }
            }else{
                header = 0;
            }
        }
    }
    // 等待超时
    return DBSP_STATUS_TIMEOUT;
}

// 设置通信协议
bool DBSP_SetProtocal(Usart_DataTypeDef *usart, uint8_t protocalId){
	// 创建环形缓冲队列
	const uint8_t size = 1;
    uint8_t buffer[size+1];
    RingBufferTypeDef ringBuf;
	RingBuffer_Init(&ringBuf, size, buffer);
	
	// 写入内容
    RingBuffer_WriteByte(&ringBuf, protocalId);
    // 发送请求包
    DBSP_SendPackage(usart, DBSP_REQUEST_SET_PROTOCAL, size, buffer+1);
	return true;
}


// 执行Marco动作组
void DBSP_RunMarco(Usart_DataTypeDef *usart, uint32_t marcoId){
    // 创建环形缓冲队列
	const uint8_t size = 4;
    uint8_t buffer[size+1];
    RingBufferTypeDef ringBuf;
	RingBuffer_Init(&ringBuf, size, buffer);

    // 写入内容
    RingBuffer_WriteULong(&ringBuf, marcoId);
    // 发送请求包
    DBSP_SendPackage(usart, DBSP_REQUEST_RUN_MARCO, size, buffer+1);
}
