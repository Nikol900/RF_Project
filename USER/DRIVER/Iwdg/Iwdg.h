#ifndef WATCHDOG_H
#define WATCHDOG_H

#define KR_KEY_Reload    ((uint16_t)0xAAAA)
#define KR_KEY_Enable    ((uint16_t)0xCCCC)
#define DogFeedTime		1000
void WatchDog_Init(void);//�������Ź��ĳ�ʼ������
void WatchDog_Feed(void);//����ι���ĺ���

#endif
/*********************************END OF FILE**********************************/
