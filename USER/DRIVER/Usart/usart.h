#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f10x.h"
#include "type.h"

#define BoundRateIntSetNum	0x09
#define StopBitsIntSetNum	0x04
#define ParityIntSetNum	0x03
#define WordLengthIntSetNum	0x02
/*********************************波特率的宏定义************************************/
#define bt_1200 0x01
#define bt_2400 0x02
#define bt_4800 0x03
#define bt_9600 0x04
#define bt_19200 0x05
#define bt_38400 0x06
#define bt_57600 0x07
#define bt_115200 0x08
/*********************************校验位的宏定义************************************/
#define Parity_N 0x00
#define Parity_E 0x01
#define Parity_O 0x03
/*********************************使能脚的宏定义************************************/
#define MAX485_Enable_TX		GPIO_SetBits(GPIOB , GPIO_Pin_9)
#define MAX485_Enable_RX		GPIO_ResetBits(GPIOB , GPIO_Pin_9)
/***********************Flash读取失败后默认配置的的宏定义***************************/
#define	BaudRateDefault 9600
#define	ParityDefault USART_Parity_No
#define	WordLengthDefault USART_WordLength_8b
#define	StopBitsDefault USART_StopBits_1

extern INT8U Rcv_Data;
extern INT32U BaudRate[BoundRateIntSetNum];
extern INT16U StopBits[StopBitsIntSetNum];
extern INT16U Parity[ParityIntSetNum];
extern INT16U WordLength[WordLengthIntSetNum];

void UartSetDisplay(void );
void UartParameterConfig(INT32U baudrate,INT16U parity,INT16U wordlength,INT16U stopbits);
void UartInit( INT32U baudrate,INT16U parity,INT16U wordlength,INT16U stopbits );
void UartSend( void );
void UartRcvTimeOut(void);
void UartDataAnalysis(void);
void RFRcvDataAnalysis(INT8U *buffer,INT8U j);

struct UartStruct{
	INT8U UartState;
	BOOL DataOverFlow;
	INT8U HaveReceiveDataNum;
	INT8U NeedToProcessDataNum;
	INT8U NeedToSendDataNum;
	INT8U HaveSendDataNum;
	INT32U EnterToSleepingTime;
	INT8U AccSpaceTime;
};
enum UartStateDef {Free=0,Recving=1,RecveFinish=2,Sending=3,Sleeping=4};

struct AnylinkMINIStruct{
	INT8U AnylinkMINIState;
};
enum AnylinkMINIStateDef {ReadyToRun=0,AllFree=1,UartWorking=2,Sx1278Working=3,Sx1278Sleeping=4,
							RFRcvDataAnalysising=5,UartDataAnalysising=6};
#endif
/*********************************END OF FILE**********************************/
