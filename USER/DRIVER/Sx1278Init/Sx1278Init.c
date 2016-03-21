/* Includes ------------------------------------------------------------------*/
#include "include.h"

extern tRadioDriver *Radio;

/*******************************************************************************
* Function Name  : Sx1278Init
* Description    : ��Ƶģ��ĳ�ʼ��������SPI��Sx1278��غ���ע��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Sx1278Init()
{
	SPI_Configuration();
	Radio = RadioDriverInit();
}
/*******************************************************************************
* Function Name  : Spi_GpioInit
* Description    : Spi_GpioInit
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Spi_GpioInit()
{
	PowerKeyInit();
	Sx1276RecvPinInit();
}
/*******************************************************************************
* Function Name  : PowerKeyInit
* Description    : SPI���ŵĳ�ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PowerKeyInit()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}
/*******************************************************************************
* Function Name  : SPI_GPIO_Configuration
* Description    : SPI���ŵĳ�ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Sx1276RecvPinInit()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
}

/*******************************************************************************
* Function Name  : SPI_GPIO_Configuration
* Description    : SPI���ŵĳ�ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void SPI_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//PA4->CS,PA5->SCK,PA6->MISO,PA7->MOSI
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	//��ʼ��Ƭѡ�������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}

/*******************************************************************************
* Function Name  : SPI_Configuration
* Description    : SPI�Ĳ�����ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_Configuration(void)
{
	SPI_InitTypeDef SPI_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	SPI_GPIO_Configuration();
	Spi_GpioInit();
	
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	SPI_InitStruct.SPI_Direction= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1,&SPI_InitStruct);

	SPI_SSOutputCmd(SPI1, ENABLE);
	SPI_Cmd(SPI1, ENABLE);
}
/*******************************************************************************
* Function Name  : SPI_WriteByte
* Description    : ��SPI����д��һ���ֽ�����
* Input          : TxDataд��SPI���ߵ�����
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_WriteByte(uint8_t TxData)
{
	while((SPI1->SR&SPI_I2S_FLAG_TXE)==0);	//�ȴ���������
	SPI1->DR=TxData;	 	  				//����һ��byte
	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==0); //�ȴ�������һ��byte
	SPI1->DR;
}
/*******************************************************************************
* Function Name  : SPI_WriteByte
* Description    : ��SPI���߶�ȡ1�ֽ�����
* Input          : None
* Output         : None
* Return         : ����������
*******************************************************************************/
uint8_t SPI_ReadByte(void)
{
	while((SPI1->SR&SPI_I2S_FLAG_TXE)==0);	//�ȴ���������
	SPI1->DR=0xFF;	 	  					//����һ�������ݲ����������ݵ�ʱ��
	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==0); //�ȴ�������һ��byte
	return SPI1->DR;
}
/*******************************************************************************
* Function Name  : SPI_CS_Select
* Description    : Ƭѡ�ź�����͵�ƽ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_CS_Select(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
}
/*******************************************************************************
* Function Name  : SPI_CS_Deselect
* Description    : Ƭѡ�ź�����͵�ƽ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_CS_Deselect(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}
/*******************************************************************************
* Function Name  : Power_Select
* Description    : SX1278Power��ʹ�ܽţ��͵�ƽ��Ч
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Power_Select(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}
/*******************************************************************************
* Function Name  : Power_Deselect
* Description    : SX1278Power��ʹ�ܽţ��ߵ�ƽ��Ч
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Power_Deselect(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
}
/*********************************END OF FILE**********************************/
