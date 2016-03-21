/* Includes -------------------------------------------------------------------*/
#include "include.h"
INT32U p=0,l=0,a=0,y=0;
/********************************����ȫ�ֱ���***********************************/
INT8U RFRcvData[PACK_LENGTH] = {0};										/* Sx1278FIFOоƬ�ж�ȡ�������� */
INT8U RFSendData[PACK_LENGTH] = {0};									/* Sx1278��Ҫ���ͳ�ȥ������		*/
INT8U UartRcvData[PACK_LENGTH] = {0};									/* ���ڽ��յ�����������         */
INT8U UartSendData[PACK_LENGTH] = {0};									/* ���ڽ�Ҫ���ͳ�ȥ������  		*/
//ReadSetting��Ϊȫ�ֱ���ʱ����ʼ��ʱ����һ�Σ��޸����ú�
INT8U ReadSetting[SetInFlashNum];										/* ��Ŷ�ȡ�����üĴ����Ļ���	*/

const INT8U SettingInFlash[]__attribute__((at(SetInFlash)))=			/* ���üĴ�����ʼ��				*/
{
	0x01,		/*  0�汾��					*/
	0x18,		/*  1�ַ�������				*/
	0x05,		/*  2����������				*/
	0x01,		/*  3У��λ����				*/
	0x00,		/*  4����λ����				*/
	0x00,		/*  5ֹͣλ����				*/
	0x75,		/*  6�ز�Ƶ�ʸ�λ			*/
	0x80,		/*  7�ز�Ƶ����λ			*/
	0x14,		/*  8�ز�Ƶ�ʵ�λ			*/
	0x0A,		/*  9��Ƶ����[6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]*/
	0x01,		/*  10����ģʽ				*/
	0x09,		/*  11��Ƶ����[0: 7.8kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
                5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]*/
	0x00,		/* 	12�û�ID_H				*/
	0x00,		/*  13�û�ID_L				*/
	0x01,		/*  14����ID				*/
	0x14,		/*  15���书��				*/
	0x00,		/*  16δ����				*/
	0x00,		/*  17δ����				*/
	0x00,		/*  18δ����				*/
	0x00,		/*  19δ����				*/
	0x00,		/*  20δ����				*/
	0x00,		/*  21δ����				*/
	0xCD,		/*  22CRCУ��H				*/
	0xC1,		/*  23CRCУ��L				*/
};

tRadioDriver *Radio = 0;												/*  ����Radio����      			*/
struct AnylinkMINIStruct AnylinkMINI;									/*  AnylinkMINI����״̬��       */

/*******************************************************************************
* Function Name  : main
* Description    : ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
	INT8U j = 0;
	RCC_Configuration();  												//ʱ������
	NVIC_Configuration(); 												//�ж����ȼ�����
	LedInit();
	UartInit(BaudRateDefault,ParityDefault,WordLengthDefault,StopBitsDefault);//��ʼ�����ڣ�����9600��N��8��1

	Sx1278Init();														//��ʼ��Sx1278ģ��
	InitSystem1msInt();	  												//��ʼ��1msϵͳ��ʱ
/*
#ifndef Debug
	WatchDog_Init();													//���Ź���ʼ��
#endif
*/	
	AnylinkMINI.AnylinkMINIState = ReadyToRun;							//AnylinkMINI׼������
	
	while (1)
	{
		switch( AnylinkMINI.AnylinkMINIState )							//����AnylinkMINI��ͬ��״̬�����ж���Ҫ���еĹ���
		{
			case ( ReadyToRun ):										//׼������
				UartSetDisplay();										//��Ĭ�ϴ���9600��N��8��1״̬�·��͹�������
				while(UartCb.UartState != Free);						//��֤�������ݷ������
				UartParameterConfig(BaudRate[ReadSetting[SettingBaudRate]],Parity[ReadSetting[SettingParity]],WordLength[ReadSetting[SettingWordLength]],StopBits[ReadSetting[SettingStopBits]]);
				Radio->Init();											//Sx1278��ʼ��
				Radio->RFRxStateEnter();								//Sx1278�������״̬
				AnylinkMINI.AnylinkMINIState = AllFree;					//״̬���л���AllFree
				break;
				
			case ( AllFree ):											//�ȴ�����״̬
				if ((1 == Radio->RFDio0State())&&(UartCb.UartState == Free))//Sx1278���յ��������ݲ��Ҵ�ʱ���ڱ����ڿ���״̬��
				{
					AnylinkMINI.AnylinkMINIState = RFRcvDataAnalysising;//�����������ݴ���״̬
					p++;
				}
				else if (UartCb.UartState == RecveFinish)				//���ڽ����������
				{
					AnylinkMINI.AnylinkMINIState = UartDataAnalysising;	//Sx1278׼��������������
					l++;
				}
				else if ((AnylinkMINI.AnylinkMINIState != Sx1278Sleeping)&&(UartCb.UartState == Sleeping))
				{
					AnylinkMINI.AnylinkMINIState = Sx1278Sleeping;
				}break;
				
			case ( RFRcvDataAnalysising ):								//AnylinkMINI���ڷ�������
				Radio->RFRxDataRead( &RFRcvData[0], &j );				//���Ƚ������Sx1278FIFOоƬ�е����ݻ��������
				if (j<PACK_LENGTH)
				{
					Radio->RFRxStateEnter();
					RFRcvDataAnalysis( RFRcvData,j );						//���ڷ���ǰ�����ݴ�����֤͸��
				}
				break;
			
			case ( UartWorking ):
				while( UartCb.UartState != Free );						//��֤�������ݷ������
				AnylinkMINI.AnylinkMINIState = AllFree;					//�л���������ģʽ
				break;
			
			case ( UartDataAnalysising ):
				UartDataAnalysis();										//���ڽ������ݴ���
				break;
			
			case ( Sx1278Working ):										//AnylinkMINI���߷�������
				AnylinkMINI.AnylinkMINIState = AllFree;					//��������ģʽ
				break;
			
			case ( Sx1278Sleeping ):
//				if(ReadSetting[10] == 1)
//				{
//					Radio->RFOpModeSet(RFLR_OPMODE_SLEEP);				//��������ģʽ
//				}
				UartCb.UartState = Free;
				AnylinkMINI.AnylinkMINIState = AllFree;					//������״̬
				break;
			
			default:
				break;
		}
	}
}
/*********************************END OF FILE**********************************/
