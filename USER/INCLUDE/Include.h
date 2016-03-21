#ifndef _INCLUDE_H
#define _INCLUDE_H
/*******************************************************************************/
#include "stm32f10x.h"
#include "system.h"
#include "type.h"
#include "usart.h"
#include "Sx1278.h"
#include "Iwdg.h"
#include "string.h"
#include "stdint.h"
#include "platform.h"
#include "radio.h"
#include "sx1276-Hal.h"
#include "sx1276-Lora.h"
/*******************************************************************************/
#define PACK_LENGTH   255					// 发送包最大长度               
#define AccSpaceTimeOutMark 10				// 串口接收超时						
#define SetInFlash		0x08005000			// 配置寄存器存放地址
#define SetInFlashNum	24					// 配置寄存器数量
#define UartStartSendNum	25
#define SetAnswerNum  23
/*******************************************************************************/
#define SettingVersion 0x00
#define SettingLength 0x01
#define SettingBaudRate 0x02
#define SettingParity 0x03
#define SettingWordLength 0x04
#define SettingStopBits 0x05
#define	SettingRFFrequencyH 0x06
#define SettingRFFrequencyM 0x07
#define SettingRFFrequencyL 0x08
#define SettingSpreadingFactor 0x09
#define SettingMode 0x0A
#define SettingSignalBw 0x0B
#define SettingId_H 0x0C
#define SettingId_L 0x0D
#define SettingNetId 0x0E
#define SettingPower 0x0F
#define SettingMisc1 0x10
#define SettingMisc2 0x11
#define SettingMisc3 0x12
#define SettingMisc4 0x13
#define SettingMisc5 0x14
#define SettingMisc6 0x15
#define SettingFinalH 0x16
#define SettingFinalL 0x17
/*******************************************************************************/
extern INT8U RFRcvData[PACK_LENGTH];					/* Sx1278FIFO芯片中读取到的数据 */
extern INT8U RFSendData[PACK_LENGTH];					/* Sx1278将要发送出去的数据		*/
extern INT8U UartRcvData[PACK_LENGTH];					/* 串口接收到的是裸数据         */
extern INT8U UartSendData[PACK_LENGTH];					/* 串口将要发送出去的数据  		*/
//ReadSetting作为全局变量时，初始化时读出一次，修改配置后
extern INT8U ReadSetting[SetInFlashNum];				/* 存放读取的配置寄存器的缓存	*/
/*******************************************************************************/
void FLASH_WriteByte(INT32U addr,INT8U *p,INT16U Byte_Num);
void FLASH_ReadByte(INT32U addr , INT8U *p ,INT16U Byte_Num);
/*******************************************************************************/
void InitSystem1msInt(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);
extern struct UartStruct UartCb;
extern struct AnylinkMINIStruct AnylinkMINI;
/*******************************************************************************/
void LedInit(void);
#endif
/*********************************END OF FILE**********************************/
