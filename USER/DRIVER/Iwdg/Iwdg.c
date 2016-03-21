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
//���Ź���ʼ����������prer-��Ƶ��reld-��������װ��ֵ
void WatchDog_Init(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //�������PR��RLR�Ĵ���
	IWDG_SetPrescaler(4);  //���÷�Ƶ4
	IWDG_SetReload(2500); //�趨��������ֵ2500
	WatchDog_Feed(); //����װ��ֵ
	IWDG_WriteAccessCmd(KR_KEY_Enable);  //�������Ź���ʱ��
}
/*******************************************************************************
* Function Name  : WatchDog_Feed
* Description    : WatchDog_Feed
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//ι��
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
