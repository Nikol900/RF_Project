#ifndef _SYSTEM_H
#define _SYSTEM_H
/*******************************************************************************/
//��Ŀ�汾��Ϣ����
/*
1. �̼��汾�ţ�v1.0   
   ʱ�䣺2016/03/02
   �������ܣ�ԭʼ�汾
   �޸�BUG: ��
										
*/
/*******************************************************************************/
#define Debug
#ifdef Debug
	#define SystemVersion		90		//V9.0
#else
	#define SystemVersion		10		//V1.0
#endif
/*******************************************************************************/
//DEFFINE
#define OK			1
#define ERR			0

#define TRUE		1
#define FALSE		0

#define ON			1
#define OFF			0

#define BitsPerByte		8
#define BytesPerWord	2
#define BytesPerDWord	4
#define BitsPerWord		(BytesPerWord*BitsPerByte)
#define BitsPerDWord	(BytesPerDWord*BitsPerByte)

/*******************************************************************************/
//Priority
#define System1msInt_Priority	0x0F
#define UART1_Priority			0x0E
#define UART2_Priority			0x0C
#define UART3_Priority			0x0D

/*******************************************************************************/
#endif
/*********************************END OF FILE**********************************/
