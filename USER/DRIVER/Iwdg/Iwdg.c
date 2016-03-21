/* Includes ------------------------------------------------------------------*/
#include "include.h"
INT32U SoftTime = 0;
/*******************************************************************************
* Function Name  : WatchDog_Init
* Description    : WatchDog_Init
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//看门狗初始化，参数：prer-分频，reld-计数器重装载值
void WatchDog_Init(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //允许访问PR和RLR寄存器
	IWDG_SetPrescaler(4);  //设置分频4
	IWDG_SetReload(2500); //设定计数器初值2500
	WatchDog_Feed(); //初次装初值
	IWDG_WriteAccessCmd(KR_KEY_Enable);  //启动看门狗定时器
}
/*******************************************************************************
* Function Name  : WatchDog_Feed
* Description    : WatchDog_Feed
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//喂狗
void WatchDog_Feed(void)
{	
	SoftTime++;
	if(SoftTime>DogFeedTime)
	{
		IWDG_WriteAccessCmd(KR_KEY_Reload);
		SoftTime=0;
	}
}
/*********************************END OF FILE**********************************/
