#include "include.h"  

struct UartStruct UartCb;

char str[UartStartSendNum] = "AnylinkMINI";
INT8U Rcv_Data;
INT8U uartsendbuffer[255];

extern INT32U p,l,a,y;

extern INT16U CRC16(INT8U *puchMsgg,INT8U usDataLen);
extern INT8U   RFRcvData[PACK_LENGTH];                                 
extern tRadioDriver *Radio;

INT8U tempbuf[PACK_LENGTH];
INT8U SetAnswerBuffer[SetAnswerNum];

INT32U BaudRate[BoundRateIntSetNum]={0,1200,2400,4800,9600,19200,38400,57600,115200};
INT16U StopBits[StopBitsIntSetNum]={USART_StopBits_1,USART_StopBits_0_5,USART_StopBits_2,USART_StopBits_1_5};
INT16U Parity[ParityIntSetNum]={USART_Parity_No,USART_Parity_Even,USART_Parity_Odd};
INT16U WordLength[WordLengthIntSetNum]={USART_WordLength_8b,USART_WordLength_9b};
/*******************************************************************************
* Function Name  : Init485EnPin
* Description    : 485方向脚的初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init485EnPin()											//使能脚配置
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_ResetBits(GPIOB , GPIO_Pin_9);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : UartPriorityConfig
* Description    : UartPriorityConfig
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UartPriorityConfig()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART3_Priority;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
* Function Name  : UartPinConfig
* Description    : UartPinConfig
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UartPinConfig()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : UartParameter1Config
* Description    : 应用寄存器中保存的串口通讯参数配置初始化串口
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UartParameterConfig(INT32U baudrate,INT16U parity,INT16U wordlength,INT16U stopbits)
{
	USART_InitTypeDef USART_InitStructure;
	USART_TypeDef* USARTx;
	
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_StopBits = stopbits;
	USART_InitStructure.USART_Parity = parity;
	USART_InitStructure.USART_WordLength = wordlength;

	USARTx = USART3;

	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USARTx, &USART_InitStructure);

	USART_Cmd( USARTx, DISABLE );
	USART_ITConfig(USARTx, USART_IT_TC, DISABLE);
	USART_ITConfig(USARTx, USART_IT_RXNE, DISABLE);

	if( (USART_GetFlagStatus(USARTx, USART_FLAG_ORE) != RESET)	||			 //USART_IT_ORE_ER : OverRun Error interrupt if the EIE bit is set
		(USART_GetFlagStatus(USARTx, USART_FLAG_NE) != RESET)	||			 //USART_IT_NE     : Noise Error interrupt
		(USART_GetFlagStatus(USARTx, USART_FLAG_FE) != RESET)	||			 //USART_IT_FE     : Framing Error interrupt
		(USART_GetFlagStatus(USARTx, USART_FLAG_PE) != RESET)	)			 //USART_IT_PE     : Parity Error interrupt
	{//同  @arg USART_IT_ORE_ER : OverRun Error interrupt if the EIE bit is set  
		USART_ClearFlag(USARTx, USART_FLAG_ORE);
		USART_ClearFlag(USARTx, USART_FLAG_NE);
		USART_ClearFlag(USARTx, USART_FLAG_FE);
		USART_ClearFlag(USARTx, USART_FLAG_PE);
		USART_ReceiveData(USARTx); //取出来扔掉
	}

	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USARTx, USART_IT_TC, ENABLE);
	USART_Cmd(USARTx, ENABLE);
}
/*******************************************************************************
* Function Name  : UartInit
* Description    : UartInit
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UartInit(INT32U baudrate,INT16U parity,INT16U wordlength,INT16U stopbits)
{
	UartPriorityConfig();
	UartPinConfig();
	UartParameterConfig(baudrate,parity,wordlength,stopbits);
	Init485EnPin();

	UartCb.UartState = Free;
	UartCb.DataOverFlow = FALSE;
	UartCb.HaveSendDataNum = 0;
	UartCb.EnterToSleepingTime = 0;
}
/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : 串口中断处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler()
{
	//溢出中断处理
	if(USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
	{
		USART_ClearFlag(USART3, USART_FLAG_ORE);
		USART_ReceiveData(USART3);
	}
	
	else if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		Rcv_Data=USART_ReceiveData(USART3);
		if(UartCb.HaveReceiveDataNum < (PACK_LENGTH-4))
		{
			switch( UartCb.UartState )
			{
				case Free:
						UartCb.HaveReceiveDataNum = 0;
						UartRcvData[UartCb.HaveReceiveDataNum]=Rcv_Data;
						UartCb.HaveReceiveDataNum++;
						UartCb.AccSpaceTime = 0 ;
						UartCb.UartState = Recving;
						break;
				case Recving:
						UartRcvData[UartCb.HaveReceiveDataNum]=Rcv_Data;
						UartCb.HaveReceiveDataNum++;
						UartCb.AccSpaceTime = 0;
						break;
				default:
						break;
			}
		}
		else
		{
			UartCb.HaveReceiveDataNum = 0;
			UartCb.DataOverFlow = TRUE;
			UartCb.AccSpaceTime = 0;
		}
	}
	if(USART_GetITStatus(USART3,USART_IT_TC)!=RESET)
	{
		if(UartCb.UartState == Sending)
		{
			if (UartCb.HaveSendDataNum < UartCb.NeedToSendDataNum)
			{
				USART_SendData(USART3,UartSendData[UartCb.HaveSendDataNum++]);
			}
			else
			{
				UartCb.HaveSendDataNum = 0;
				UartCb.UartState = Free;
				MAX485_Enable_RX;
			}
		}
		USART_ClearITPendingBit(USART3, USART_IT_TC);
	}
}
/*******************************************************************************
* Function Name  : UartSend
* Description    : 串口发送函数
* Input          : *buffer，需要发送的字符串数组
* Output         : None
* Return         : None
*******************************************************************************/
void UartSend()
{
	MAX485_Enable_TX;
	UartCb.UartState = Sending;
	USART_SendData(USART3,UartSendData[UartCb.HaveSendDataNum++]);
}
/*******************************************************************************
* Function Name  : UartPowerSend
* Description    : UartPowerSend
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UartPowerSend()
{
	MAX485_Enable_TX;
	UartCb.UartState = Sending;
	memcpy(UartSendData,str,strlen(str));
	USART_SendData(USART3,UartSendData[UartCb.HaveSendDataNum++]);	
}
/*******************************************************************************
* Function Name  : UartSetDisplay
* Description    : 实现在串口通讯9600 N 8 1 情况下，上电瞬间显示当前设备的串口通讯参数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UartSetDisplay()
{
	FLASH_ReadByte(SetInFlash,ReadSetting,SetInFlashNum);
	switch (ReadSetting[SettingBaudRate])
	{
		case bt_1200:
			strcat(str,"   1200");
			break;
		
		case bt_2400:
			strcat(str,"   2400");
			break;
		
		case bt_4800:
			strcat(str,"   4800");
			break;
		
		case bt_9600:
			strcat(str,"   9600");
			break;
		
		case bt_19200:
			strcat(str,"  19200");
			break;
		
		case bt_38400:
			strcat(str,"  38400");
			break;
		
		case bt_57600:
			strcat(str,"  57600");
			break;
		
		case bt_115200:
			strcat(str," 115200");
			break;
		
		default:
			break;
	}
	switch (ReadSetting[SettingParity])
	{
		case Parity_N:
			strcat(str," N");
			strcat(str," 8");
			break;
		
		case Parity_E:
			strcat(str," E");
			strcat(str," 7");
			break;
		
		case Parity_O:
			strcat(str," O");
			strcat(str," 7");
			break;
		
		default:
			break;
	}
	strcat(str," 1");
	UartCb.NeedToSendDataNum=UartStartSendNum;
	UartPowerSend();
}
/*******************************************************************************
* Function Name  : UartSetAnswer
* Description    : 串口数据分析，判断是否是修改参数命令的字符串
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UartSetAnswer()
{
	MAX485_Enable_TX;
	UartCb.UartState = Sending;
	memcpy(UartSendData,SetAnswerBuffer,SetAnswerNum);
	USART_SendData(USART3,UartSendData[UartCb.HaveSendDataNum++]);
}
/*******************************************************************************
* Function Name  : UartDataAnalysis
* Description    : 串口数据分析，判断是否是修改参数命令的字符串
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UartDataAnalysis()
{
	INT8U i,j;
	INT16U SendKey;
	j = UartCb.HaveReceiveDataNum;
	memcpy(tempbuf,UartRcvData,j);

	if (UartCb.HaveReceiveDataNum == SetAnswerNum)
	{
		UartCb.NeedToSendDataNum = UartCb.HaveReceiveDataNum;
		UartCb.UartState=Free;
		UartCb.HaveReceiveDataNum = 0 ;
		if((tempbuf[0] == 0xAF)&&(tempbuf[1] == 0xAF)&&(tempbuf[4] == 0xAF)&&(tempbuf[21] == 0x0D)&&(tempbuf[22] == 0x0A))
		{
			switch(tempbuf[6])
			{
				case 0x01:
						FLASH_ReadByte(SetInFlash,ReadSetting,SetInFlashNum);
						ReadSetting[SettingBaudRate]=tempbuf[8];
						ReadSetting[SettingParity]=tempbuf[9];
						ReadSetting[SettingRFFrequencyH]=tempbuf[10];
						ReadSetting[SettingRFFrequencyM]=tempbuf[11];
						ReadSetting[SettingRFFrequencyL]=tempbuf[12];
						ReadSetting[SettingSpreadingFactor]=tempbuf[13];
						ReadSetting[SettingMode]=tempbuf[14];
						ReadSetting[SettingSignalBw]=tempbuf[15];
						ReadSetting[SettingId_H]=tempbuf[16];
						ReadSetting[SettingId_L]=tempbuf[17];
						ReadSetting[SettingNetId]=tempbuf[18];
						ReadSetting[SettingPower]=tempbuf[19];
						FLASH_WriteByte(SetInFlash,ReadSetting,SetInFlashNum);
						Radio->Init();											//Sx1278初始化
						Radio->RFRxStateEnter();								//Sx1278进入接受状态
						AnylinkMINI.AnylinkMINIState = AllFree;
					break;

				case 0x02:
						FLASH_ReadByte(SetInFlash,ReadSetting,SetInFlashNum);
						memcpy(SetAnswerBuffer,tempbuf,j);
						SetAnswerBuffer[5]=0x00;
						SetAnswerBuffer[6]=0x02;
						SetAnswerBuffer[8]=ReadSetting[SettingBaudRate];
						SetAnswerBuffer[9]=ReadSetting[SettingParity];
						SetAnswerBuffer[10]=ReadSetting[SettingRFFrequencyH];
						SetAnswerBuffer[11]=ReadSetting[SettingRFFrequencyM];
						SetAnswerBuffer[12]=ReadSetting[SettingRFFrequencyL];
						SetAnswerBuffer[13]=ReadSetting[SettingSpreadingFactor];
						SetAnswerBuffer[14]=ReadSetting[SettingMode];
						SetAnswerBuffer[15]=ReadSetting[SettingSignalBw];
						SetAnswerBuffer[16]=ReadSetting[SettingId_H];
						SetAnswerBuffer[17]=ReadSetting[SettingId_L];
						SetAnswerBuffer[18]=ReadSetting[SettingNetId];
						SetAnswerBuffer[19]=ReadSetting[SettingPower];
						UartSetAnswer();
						AnylinkMINI.AnylinkMINIState = UartWorking;
					break;
				default:
					break;
			}
		}
	}
	else
	{
		if(UartCb.DataOverFlow == FALSE)
		{
			RFSendData[0] = ReadSetting[14];
			RFSendData[1] = j;
			for(i=0;i<j;i++)
			{
				RFSendData[2+i]=UartRcvData[i];
			}
			
			SendKey=(INT16U)CRC16(RFSendData,j+2);
			RFSendData[j+2]=(INT8U)(SendKey>>BitsPerByte);
			RFSendData[j+3]=(INT8U)(SendKey&0xFF);
			
			y++;
			
			Radio->RFTxData( &RFSendData[0],j+4 );
			Radio->RFRxStateEnter();
		}
		else UartCb.DataOverFlow = FALSE;
		UartCb.UartState=Free;
		AnylinkMINI.AnylinkMINIState = Sx1278Working;
	}
}
/*********************************END OF FILE**********************************/
