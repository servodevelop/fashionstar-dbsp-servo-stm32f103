#include "sys.h"

// ע: 
void Sys_SysClockConfig(void){
	// ��λRCC�Ĵ���
	RCC_DeInit();
	// ʹ��HSE(High Speed External) 
	// ѡ��ʹ�ø����ⲿʱ����Ϊʱ��Դ, ����������8MHz�ľ���
	RCC_HSEConfig(RCC_HSE_ON);
	// �ȴ�HSE����
	if(RCC_WaitForHSEStartUp() == ERROR){
		// HSE����ʧ��
		return;
	}
	
	//ʹ��Ԥȡָ������FLASH�̼��еĺ���
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    //����FLASH�ȴ����ڡ�  ��Ϊ��Ƶ��72M ���Եȴ��������ڡ�
    FLASH_SetLatency(FLASH_Latency_2);  
	
    // SYSCLK -> AHB 
	// AHBʱ��Ƶ�����Ϊ72M�����Բ���Ƶ
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
	// AHB -> APB1 
	// APB1�����ʱ��Ƶ�����Ϊ36MHz, �������÷�Ƶϵ��Ϊ2
	RCC_PCLK1Config(RCC_HCLK_Div2);
    // AHB --> APB2
	// APB2���ʱ��Ƶ�����Ϊ72MHz,	���Բ���Ƶ
	RCC_PCLK2Config(RCC_HCLK_Div1);
	
	//���������໷ PLLCLK = HSE * ��Ƶ����
	// 8MHz * 9 = 72MHz
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    //ʹ��PLL���໷��������Ƶ
	RCC_PLLCmd(ENABLE);        
    // RCC_FLAG_PLLRDY,
	// PLL:���໷;  RDY:Ready �ж����໷�Ƿ��������
	while ( RESET == RCC_GetFlagStatus(RCC_FLAG_PLLRDY) );  //�ȴ�PLL�ȶ�
	// ѡ��ϵͳʱ�ӣ�ѡ�����໷����� ����SW���أ�
    // �����໷�����ʱ���ź���Ϊϵͳʱ���ź� 72Mhz    
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    // �ȴ�ϵͳʱ��Դ�л�ΪPLLCLK
	// 0x00: HSI used as system clock
	// 0x04: HSE used as system clock
	// 0x08: PLL used as system clock
    while ( RCC_GetSYSCLKSource() != 0x08);
}
