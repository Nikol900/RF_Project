/* Includes ------------------------------------------------------------------*/
#include "include.h"

extern void UartRcvTimeOut(void);

void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus;
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();
    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);
    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();
	//�ⲿ����û������ѭ��
	while(HSEStartUpStatus != SUCCESS);

    if(HSEStartUpStatus == SUCCESS)
    {
        /* HCLK = SYSCLK */
        RCC_HCLKConfig(RCC_SYSCLK_Div1);

        /* PCLK2 = HCLK */
        RCC_PCLK2Config(RCC_HCLK_Div1);

        /* PCLK1 = HCLK/2 */
        RCC_PCLK1Config(RCC_HCLK_Div2);

        /* Flash 2 wait state */
        FLASH_SetLatency(FLASH_Latency_2);
        /* Enable Prefetch Buffer */
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        /* PLLCLK = 8MHz * 9 = 72 MHz */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

        /* Enable PLL */
        RCC_PLLCmd(ENABLE);

        /* Wait till PLL is ready */
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

        /* Select PLL as system clock source */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        /* Wait till PLL is used as system clock source */
        while(RCC_GetSYSCLKSource() != 0x08);
    }
}

/*******************************************************************************
* Function Name  : InitSystem1msInt
* Description    : InitSystem1msInt
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitSystem1msInt(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	//�ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 						//ͨ��TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;				//ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = System1msInt_Priority; 	//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//ģʽ
	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	//���½�Timer����Ϊȱʡֵ
	TIM_DeInit(TIM2);
	//�����ڲ�ʱ�Ӹ�TIM2�ṩʱ��Դ
	TIM_InternalClockConfig(TIM2);
	//Ԥ��Ƶϵ��Ϊ72-1������������ʱ��Ϊ72MHz/71 = 1MHz
	TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;		
	//����ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//���ü�����ģʽΪ���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//���ü��������С��ÿ��2000�����Ͳ���һ�������¼�
	TIM_TimeBaseStructure.TIM_Period = 1000 - 1;	   //����Ϊ1ms
	//������Ӧ�õ�TIM2��
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	//�������жϱ�־
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	//��ֹARRԤװ�ػ�����
	TIM_ARRPreloadConfig(TIM2, DISABLE);		//Ԥװ�ؼĴ�����Ӱ�ӼĴ���֮��û�л�������ֱ����
	//����TIM2���ж�
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM2,ENABLE); //ʹ�ܶ�ʱ��			
}
/*******************************************************************************
* Function Name  : UartRcvTimeOut
* Description    : UartRcvTimeOut
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*
void WaitingWorkTimeOut()
{
	if(UartCb.EnterToSleepingTime > EnterToSleepingTimeOut)
	{
		UartCb.UartState = Sleeping;
		UartCb.EnterToSleepingTime = 0;
	}
	else if (UartCb.UartState == Free)
	{
		UartCb.EnterToSleepingTime++;
	}
	else UartCb.EnterToSleepingTime = 0;
}
*/
/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : TIM2_IRQHandler
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{
	//����Ƿ�����������¼�
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
		UartRcvTimeOut( );
//		WaitingWorkTimeOut();
/*		
#ifndef Debug
		WatchDog_Feed();		//���Ź�
#endif
*/		
	}
}
/*******************************************************************************
* Function Name  : UartRcvTimeOut
* Description    : UartRcvTimeOut
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UartRcvTimeOut()
{
	if(( UartCb.UartState == Recving )&&(AnylinkMINI.AnylinkMINIState == AllFree))
	{
		if( UartCb.AccSpaceTime > AccSpaceTimeOutMark)
		{
			UartCb.UartState = RecveFinish;
		}
		else
		{
			UartCb.AccSpaceTime++;
		}
	}
}

/*********************************END OF FILE**********************************/