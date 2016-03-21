/* Includes ------------------------------------------------------------------*/
#include "include.h"

extern INT16U CRC16(INT8U *puchMsgg,INT8U usDataLen);//CRC校验
extern INT8U ReadSetting[];
extern INT8U SettingInFlash[];
/*******************************************************************************
* Function Name  : FlashCrc
* Description    : FlashCrc
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FlashCrc( INT8U *p , INT16U Byte_Num )							//Flash读取时的CRC校验
{
	INT16U Cal_CRC;
	INT16U Rcv_CRC;
	Cal_CRC = CRC16(p,Byte_Num);
	Rcv_CRC = ((INT16U)p[Byte_Num-2])|(((INT16U)p[Byte_Num-1])<<BitsPerByte);
	if ( Cal_CRC != Rcv_CRC)										//读取失败,选择默认值，并显示警告，用LED灯闪烁来实现
	{
		memcpy(ReadSetting,SettingInFlash,SetInFlashNum);
	}
}
/*******************************************************************************
* Function Name  : FLASH_WriteByte
* Description    : FLASH_WriteByte
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_WriteByte(INT32U addr,INT8U *p,INT16U Byte_Num)
{
	INT32U HalfWord;
	INT16U SendKey;
	Byte_Num = Byte_Num/2;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(addr);
	//CRC校验的插入
	SendKey = (INT16U)CRC16(p,Byte_Num);
	p[Byte_Num-2]=(INT8U)(SendKey>>BitsPerByte);
	p[Byte_Num-1]=(INT8U)(SendKey&0xFF);
	
	while(Byte_Num--)
	{
		HalfWord=*(p++);
		HalfWord|=*(p++)<<BitsPerByte;
		FLASH_ProgramHalfWord(addr, HalfWord);

		addr+=2;
	}
	FLASH_Lock();
}
/*******************************************************************************
* Function Name  : FLASH_ReadByte
* Description    : FLASH_ReadByte
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_ReadByte(INT32U addr , INT8U *p , INT16U Byte_Num)
{
	INT8U i= Byte_Num;
	while(Byte_Num--)
	{
		*(p++)=*((INT8U*)addr++);
	}
	FlashCrc(p,i);
}
/*********************************END OF FILE**********************************/
