#include "button.h"


KeyStatus keyStatusList[KEY_NUM];

// ����GPIO����
void Button_GPIO(void){
	// ����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	// ���ùܽ�Ϊ��������
	GPIO_InitTypeDef GPIO_InitStructure; // IO���ýṹ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // ��������
	// ����KEY1
	GPIO_InitStructure.GPIO_Pin = KEY1_Pin;
	GPIO_Init(KEY1_GPIO_Port, &GPIO_InitStructure);
	// ����KEY2
	GPIO_InitStructure.GPIO_Pin = KEY2_Pin;
	GPIO_Init(KEY2_GPIO_Port, &GPIO_InitStructure);
	// ����KEY3
	GPIO_InitStructure.GPIO_Pin = KEY3_Pin;
	GPIO_Init(KEY3_GPIO_Port, &GPIO_InitStructure);
	// ����KEY4
	GPIO_InitStructure.GPIO_Pin = KEY4_Pin;
	GPIO_Init(KEY4_GPIO_Port, &GPIO_InitStructure);
	// ����KEYA
	GPIO_InitStructure.GPIO_Pin = KEYA_Pin;
	GPIO_Init(KEYA_GPIO_Port, &GPIO_InitStructure);
	// ����KEYB
	GPIO_InitStructure.GPIO_Pin = KEYB_Pin;
	GPIO_Init(KEYB_GPIO_Port, &GPIO_InitStructure);
}

// ����ɨ��,��ƽ״̬����
void Button_KeyLevelScan(void){
	keyStatusList[KEY1].level = GPIO_ReadInputDataBit(KEY1_GPIO_Port, KEY1_Pin);
	keyStatusList[KEY2].level = GPIO_ReadInputDataBit(KEY2_GPIO_Port, KEY2_Pin);
	keyStatusList[KEY3].level = GPIO_ReadInputDataBit(KEY3_GPIO_Port, KEY3_Pin);
	keyStatusList[KEY4].level = GPIO_ReadInputDataBit(KEY4_GPIO_Port, KEY4_Pin);
	keyStatusList[KEYA].level = GPIO_ReadInputDataBit(KEYA_GPIO_Port, KEYA_Pin);
	keyStatusList[KEYB].level = GPIO_ReadInputDataBit(KEYB_GPIO_Port, KEYB_Pin);
}

// ���°���״̬
void Button_KeyStatusInit(void){
	for(uint8_t btnIdx=0; btnIdx < KEY_NUM; btnIdx++){
		keyStatusList[btnIdx].fsmStatus = KEY_FSM_RELEASED;
		keyStatusList[btnIdx].eventFlag = KEY_EVENT_NONE;
		keyStatusList[btnIdx].lastEventFlag = KEY_EVENT_NONE;
		keyStatusList[btnIdx].level = KEY_RELEASE;
		keyStatusList[btnIdx].pressedTime = 0;
		keyStatusList[btnIdx].releaseTime = 0;
		keyStatusList[btnIdx].lastPressedTime = 0;
		keyStatusList[btnIdx].lastReleaseTime = 0;
	}
}
// ���°�����״̬
void Button_KeyStatusUpdate(uint8_t btnId){
	KeyStatus* keyStatus = &(keyStatusList[btnId]);
	switch(keyStatus->fsmStatus){
		case KEY_FSM_RELEASED:
			// STATE1: ��ʼ״̬(����δ����)
			if(keyStatus->level == KEY_PRESSED){
				keyStatus->pressedTime = SysTick_Millis();
				keyStatus->fsmStatus = KEY_FSM_DEBOUNCE;
			}
			break;
		case KEY_FSM_DEBOUNCE:
			// STATE2: ����״̬
			if(keyStatus->level == KEY_RELEASE){
				// ��������, ���� �ָ�״̬Ϊ����δ����
				keyStatus->fsmStatus = KEY_FSM_RELEASED;
			}else if((SysTick_Millis()-keyStatus->pressedTime) > KEY_DEBOUNCE_INTERVAL){
				// ������Ϊ����״̬ -> �л�״̬Ϊ�̰�״̬
				keyStatus->fsmStatus = KEY_FSM_SHORT_PRESSED;
			}
			break;
		case KEY_FSM_SHORT_PRESSED:
			// STATE3: �����˰�
			if ((SysTick_Millis() - keyStatus->pressedTime) > KEY_LONG_PRESSED_THRESHOLD){
				// �л�Ϊ��������״̬
				keyStatus->fsmStatus = KEY_FSM_LONG_PRESSED;
				// ����״̬Ϊ������ʼ
				keyStatus->eventFlag =  KEY_EVENT_LONG_PRESSED_START;
			}else if(keyStatus->level == KEY_RELEASE){
				// ���°����ͷŵ�ʱ��
				keyStatus->releaseTime = SysTick_Millis();
				// �ж��Ƿ�ΪDouble Click
				if(keyStatus->lastEventFlag == KEY_EVENT_SINGLE_CLICK && \
					(keyStatus->pressedTime - keyStatus->lastReleaseTime) <= KEY_DOUBLE_CLICK_THRESHOLD){
					// �ϴ��¼�Ϊ���������ϴ��ͷŵ�ʱ�����ǰ���µ�ʱ���м���¼����С��(KEY_DOUBLE_CLICK_THRESHOLD)ms
					keyStatus->eventFlag = KEY_EVENT_DOUBLE_CLICK;
				}else{
					// �ж�Ϊ�����¼�
					keyStatus->eventFlag = KEY_EVENT_SINGLE_CLICK;
				}
				// ������ʷ����
				keyStatus->lastEventFlag = keyStatus->eventFlag;
				keyStatus->lastPressedTime = keyStatus->pressedTime;
				keyStatus->lastReleaseTime = keyStatus->releaseTime;
				// ״̬�л�Ϊ�ͷ�״̬
				keyStatus->fsmStatus = KEY_FSM_RELEASED;
			}
			break;
		case KEY_FSM_LONG_PRESSED:
			// STATE4: ��������
			if (keyStatus->level == KEY_RELEASE){
				// �����ͷŵ��¼�
				keyStatus->releaseTime = SysTick_Millis();
				// �����¼�����Ϊ��������
				keyStatus->eventFlag =  KEY_EVENT_LONG_PRESSED_END;
				// ������ʷ����
				keyStatus->lastEventFlag = keyStatus->eventFlag;
				keyStatus->lastPressedTime = keyStatus->pressedTime;
				keyStatus->lastReleaseTime = keyStatus->releaseTime;
				// ״̬�л�Ϊ�ͷ�״̬
				keyStatus->fsmStatus = KEY_FSM_RELEASED;
			}
			break;
	}
}

// �������а�����״̬
void Button_KeyStatusUpdateAll(void){
	for(uint8_t btnIdx=0; btnIdx < KEY_NUM; btnIdx++){
		Button_KeyStatusUpdate(btnIdx);
	}
}

// ������ʼ��
void Button_Init(void){
	
	// ����GPIO����
	Button_GPIO();
	
	// ��ʼ��keyStatusList
	Button_KeyStatusInit();
}


