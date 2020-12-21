/*
 * ��������
 * Author: Kyle
 */
 
#ifndef __BUTTON_H
#define __BUTTON_H

#include "stm32f10x.h"
#include "sys_tick.h"
#include "stdbool.h"

// �����ĸ���
#define KEY_NUM 6

// �������
#define KEY1 0
#define KEY2 1
#define KEY3 2
#define KEY4 3
#define KEYA 4
#define KEYB 5

// GPIO��Դ����
#define KEY1_GPIO_Port GPIOB
#define KEY1_Pin GPIO_Pin_4
#define KEY2_GPIO_Port GPIOB
#define KEY2_Pin GPIO_Pin_5
#define KEY3_GPIO_Port GPIOB
#define KEY3_Pin GPIO_Pin_6
#define KEY4_GPIO_Port GPIOB
#define KEY4_Pin GPIO_Pin_7
#define KEYA_GPIO_Port GPIOB
#define KEYA_Pin GPIO_Pin_8
#define KEYB_GPIO_Port GPIOB
#define KEYB_Pin GPIO_Pin_9

// ������ƽ����
#define KEY_RELEASE RESET
#define KEY_PRESSED SET

// ����ɨ������(��λms)
#define KEY_SCAN_INTERVAL 10

// �����¼�����
#define KEY_EVENT_NONE 0 				// ���¼�����
#define KEY_EVENT_SINGLE_CLICK 1 		// ��������
#define KEY_EVENT_DOUBLE_CLICK 2 		// ����˫��
#define KEY_EVENT_LONG_PRESSED_START 3 	// ����������ʼ
#define KEY_EVENT_LONG_PRESSED_END  4	// ������������

// ��������״̬�� 
#define KEY_FSM_RELEASED 0 		// �����ͷ�
#define KEY_FSM_DEBOUNCE 1 		// ����
#define KEY_FSM_SHORT_PRESSED 2	// �̰�
#define KEY_FSM_LONG_PRESSED 3	// ����

// ��������
#define KEY_DEBOUNCE_INTERVAL 10 		// ��������������(��λ ms)
#define KEY_LONG_PRESSED_THRESHOLD 250 	// �����볤�����ж���ֵ
										// ����ʱ��������ֵ�ͽ��볤��״̬
#define KEY_DOUBLE_CLICK_THRESHOLD 200	// ����������ε����¼������ʱ��С�����ֵ�����ж�Ϊ˫��

typedef struct{
	uint8_t fsmStatus; 				// ����״̬����״̬
	uint8_t eventFlag;	 			// �����¼�Flag
	uint8_t lastEventFlag; 			// �ϴε��¼�FLAG
	uint8_t level; 				// �����ĵ�ƽ״̬
	TimeTypedef pressedTime;		// �������µ�ʱ��
	TimeTypedef releaseTime;		// ����̧���ʱ��
	TimeTypedef lastPressedTime; 	// �ϴΰ������µ�ʱ��
	TimeTypedef lastReleaseTime; 	// �ϴΰ����ͷŵ�ʱ��
}KeyStatus;


extern KeyStatus keyStatusList[KEY_NUM];

// ����GPIO����
void Button_GPIO(void);

// ����״̬��ʼ��
void Button_KeyStatusInit(void);

// ������ʼ��
void Button_Init(void);

// ������ƽ״̬ɨ��
void Button_KeyLevelScan(void);

// ���°�����״̬
void Button_KeyStatusUpdate(uint8_t btnId);

// �������а�����״̬
void Button_KeyStatusUpdateAll(void);

#endif
