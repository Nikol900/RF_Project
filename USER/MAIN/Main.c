/* Includes -------------------------------------------------------------------*/
#include "include.h"
INT32U p=0,l=0,a=0,y=0;
/********************************定义全局变量***********************************/
INT8U RFRcvData[PACK_LENGTH] = {0};										/* Sx1278FIFO芯片中读取到的数据 */
INT8U RFSendData[PACK_LENGTH] = {0};									/* Sx1278将要发送出去的数据		*/
INT8U UartRcvData[PACK_LENGTH] = {0};									/* 串口接收到的是裸数据         */
INT8U UartSendData[PACK_LENGTH] = {0};									/* 串口将要发送出去的数据  		*/
//ReadSetting作为全局变量时，初始化时读出一次，修改配置后
INT8U ReadSetting[SetInFlashNum];										/* 存放读取的配置寄存器的缓存	*/

const INT8U SettingInFlash[]__attribute__((at(SetInFlash)))=			/* 配置寄存器初始化				*/
{
	0x01,		/*  0版本号					*/
	0x18,		/*  1字符串长度				*/
	0x05,		/*  2波特率配置				*/
	0x01,		/*  3校验位配置				*/
	0x00,		/*  4数据位配置				*/
	0x00,		/*  5停止位配置				*/
	0x75,		/*  6载波频率高位			*/
	0x80,		/*  7载波频率中位			*/
	0x14,		/*  8载波频率低位			*/
	0x0A,		/*  9扩频因数[6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]*/
	0x01,		/*  10工作模式				*/
	0x09,		/*  11扩频带宽[0: 7.8kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
                5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]*/
	0x00,		/* 	12用户ID_H				*/
	0x00,		/*  13用户ID_L				*/
	0x01,		/*  14网络ID				*/
	0x14,		/*  15发射功率				*/
	0x00,		/*  16未设置				*/
	0x00,		/*  17未设置				*/
	0x00,		/*  18未设置				*/
	0x00,		/*  19未设置				*/
	0x00,		/*  20未设置				*/
	0x00,		/*  21未设置				*/
	0xCD,		/*  22CRC校验H				*/
	0xC1,		/*  23CRC校验L				*/
};

tRadioDriver *Radio = 0;												/*  声明Radio函数      			*/
struct AnylinkMINIStruct AnylinkMINI;									/*  AnylinkMINI工作状态机       */

/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
	INT8U j = 0;
	RCC_Configuration();  												//时钟配置
	NVIC_Configuration(); 												//中断优先级配置
	LedInit();
	UartInit(BaudRateDefault,ParityDefault,WordLengthDefault,StopBitsDefault);//初始化串口，配置9600，N，8，1

	Sx1278Init();														//初始化Sx1278模块
	InitSystem1msInt();	  												//初始化1ms系统定时
/*
#ifndef Debug
	WatchDog_Init();													//看门狗初始化
#endif
*/	
	AnylinkMINI.AnylinkMINIState = ReadyToRun;							//AnylinkMINI准备就绪
	
	while (1)
	{
		switch( AnylinkMINI.AnylinkMINIState )							//根据AnylinkMINI不同的状态机，判断需要进行的工作
		{
			case ( ReadyToRun ):										//准备就绪
				UartSetDisplay();										//在默认串口9600，N，8，1状态下发送工作参数
				while(UartCb.UartState != Free);						//保证串口数据发送完成
				UartParameterConfig(BaudRate[ReadSetting[SettingBaudRate]],Parity[ReadSetting[SettingParity]],WordLength[ReadSetting[SettingWordLength]],StopBits[ReadSetting[SettingStopBits]]);
				Radio->Init();											//Sx1278初始化
				Radio->RFRxStateEnter();								//Sx1278进入接受状态
				AnylinkMINI.AnylinkMINIState = AllFree;					//状态机切换到AllFree
				break;
				
			case ( AllFree ):											//等待工作状态
				if ((1 == Radio->RFDio0State())&&(UartCb.UartState == Free))//Sx1278接收到无线数据并且此时串口必须在空闲状态下
				{
					AnylinkMINI.AnylinkMINIState = RFRcvDataAnalysising;//进入无线数据处理状态
					p++;
				}
				else if (UartCb.UartState == RecveFinish)				//串口接收完成数据
				{
					AnylinkMINI.AnylinkMINIState = UartDataAnalysising;	//Sx1278准备发送无线数据
					l++;
				}
				else if ((AnylinkMINI.AnylinkMINIState != Sx1278Sleeping)&&(UartCb.UartState == Sleeping))
				{
					AnylinkMINI.AnylinkMINIState = Sx1278Sleeping;
				}break;
				
			case ( RFRcvDataAnalysising ):								//AnylinkMINI串口发送数据
				Radio->RFRxDataRead( &RFRcvData[0], &j );				//首先将存放在Sx1278FIFO芯片中的数据缓存读出来
				if (j<PACK_LENGTH)
				{
					Radio->RFRxStateEnter();
					RFRcvDataAnalysis( RFRcvData,j );						//串口发送前的数据处理，保证透传
				}
				break;
			
			case ( UartWorking ):
				while( UartCb.UartState != Free );						//保证串口数据发送完成
				AnylinkMINI.AnylinkMINIState = AllFree;					//切换到待工作模式
				break;
			
			case ( UartDataAnalysising ):
				UartDataAnalysis();										//串口接收数据处理
				break;
			
			case ( Sx1278Working ):										//AnylinkMINI无线发送数据
				AnylinkMINI.AnylinkMINIState = AllFree;					//进入休眠模式
				break;
			
			case ( Sx1278Sleeping ):
//				if(ReadSetting[10] == 1)
//				{
//					Radio->RFOpModeSet(RFLR_OPMODE_SLEEP);				//进入休眠模式
//				}
				UartCb.UartState = Free;
				AnylinkMINI.AnylinkMINIState = AllFree;					//待工作状态
				break;
			
			default:
				break;
		}
	}
}
/*********************************END OF FILE**********************************/
