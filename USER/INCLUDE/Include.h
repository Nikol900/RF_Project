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
#define PACK_LENGTH   255					// ���Ͱ���󳤶�               
#define AccSpaceTimeOutMark 10				// ���ڽ��ճ�ʱ						
#define SetInFlash		0x08005000			// ���üĴ�����ŵ�ַ
#define SetInFlashNum	24					// ���üĴ�������
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
extern INT8U RFRcvData[PACK_LENGTH];					/* Sx1278FIFOоƬ�ж�ȡ�������� */
extern INT8U RFSendData[PACK_LENGTH];					/* Sx1278��Ҫ���ͳ�ȥ������		*/
extern INT8U UartRcvData[PACK_LENGTH];					/* ���ڽ��յ�����������         */
extern INT8U UartSendData[PACK_LENGTH];					/* ���ڽ�Ҫ���ͳ�ȥ������  		*/
//ReadSetting��Ϊȫ�ֱ���ʱ����ʼ��ʱ����һ�Σ��޸����ú�
extern INT8U ReadSetting[SetInFlashNum];				/* ��Ŷ�ȡ�����üĴ����Ļ���	*/
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
