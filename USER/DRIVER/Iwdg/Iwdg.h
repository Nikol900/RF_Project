#ifndef WATCHDOG_H
#define WATCHDOG_H

#define KR_KEY_Reload    ((uint16_t)0xAAAA)
#define KR_KEY_Enable    ((uint16_t)0xCCCC)
#define DogFeedTime		1000
void WatchDog_Init(void);//申明看门狗的初始化函数
void WatchDog_Feed(void);//申明喂狗的函数

#endif
/*********************************END OF FILE**********************************/
