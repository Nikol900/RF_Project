/* Includes ------------------------------------------------------------------*/
#include "include.h"

extern tRadioDriver *Radio;

/*******************************************************************************
* Function Name  : Sx1278Init
* Description    : 射频模块的初始化，包括SPI和Sx1278相关函数注册
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
* Description    : SPI引脚的初始化
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
* Description    : SPI引脚的初始化
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
* Description    : SPI引脚的初始化
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
	//初始化片选输出引脚
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}

/*******************************************************************************
* Function Name  : SPI_Configuration
* Description    : SPI的参数初始化
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
* Description    : 向SPI总线写入一个字节数据
* Input          : TxData写入SPI总线的数据
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_WriteByte(uint8_t TxData)
{
	while((SPI1->SR&SPI_I2S_FLAG_TXE)==0);	//等待发送区空
	SPI1->DR=TxData;	 	  				//发送一个byte
	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==0); //等待接收完一个byte
	SPI1->DR;
}
/*******************************************************************************
* Function Name  : SPI_WriteByte
* Description    : 从SPI总线读取1字节数据
* Input          : None
* Output         : None
* Return         : 读到的数据
*******************************************************************************/
uint8_t SPI_ReadByte(void)
{
	while((SPI1->SR&SPI_I2S_FLAG_TXE)==0);	//等待发送区空
	SPI1->DR=0xFF;	 	  					//发送一个空数据产生输入数据的时钟
	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==0); //等待接收完一个byte
	return SPI1->DR;
}
/*******************************************************************************
* Function Name  : SPI_CS_Select
* Description    : 片选信号输出低电平
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
* Description    : 片选信号输出低电平
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
* Description    : SX1278Power的使能脚，低电平有效
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
* Description    : SX1278Power的使能脚，高电平有效
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Power_Deselect(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
}
/*********************************END OF FILE**********************************/
