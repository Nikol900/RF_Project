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
	//外部晶振没起振，死循环
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

	//中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 						//通道TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;				//占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = System1msInt_Priority; 	//副优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//模式
	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	//重新将Timer设置为缺省值
	TIM_DeInit(TIM2);
	//采用内部时钟给TIM2提供时钟源
	TIM_InternalClockConfig(TIM2);
	//预分频系数为72-1，这样计数器时钟为72MHz/71 = 1MHz
	TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;		
	//设置时钟分割
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//设置计数器模式为向上计数模式
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//设置计数溢出大小，每计2000个数就产生一个更新事件
	TIM_TimeBaseStructure.TIM_Period = 1000 - 1;	   //周期为1ms
	//将配置应用到TIM2中
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	//清除溢出中断标志
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	//禁止ARR预装载缓冲器
	TIM_ARRPreloadConfig(TIM2, DISABLE);		//预装载寄存器与影子寄存器之间没有缓冲器，直连的
	//开启TIM2的中断
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM2,ENABLE); //使能定时器			
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
	//检测是否发生溢出更新事件
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
		UartRcvTimeOut( );
//		WaitingWorkTimeOut();
/*		
#ifndef Debug
		WatchDog_Feed();		//看门狗
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
