/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND 
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, SEMTECH SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * 
 * Copyright (C) SEMTECH S.A.
 */
/*! 
 * \file       sx1276-Hal.c
 * \brief      SX1276 Hardware Abstraction Layer
 *
 * \version    2.0.B2 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#include "include.h"


#if defined( USE_SX1276_RADIO )


/*  移值时需要修改的宏定义    */	

//#define  IRQ_PIN   (1ul << 4)                                           /* 模块中断输出引脚             */
//#define  RST_PIN   (1ul << 3)                                           /* 电源使能引脚，低电平有效     */
//#define  SEL_PIN   (1ul << 2)                                           /* 模块SPI片选引脚，低电平有效  */

//#define  SDO_PIN   (1ul << 8)                                           /* 模块SPI输出引脚，            */
//#define  CLK_PIN   (1ul << 6)                                           /* SPI时钟，频率不要超过5MHz    */
//#define  SDI_PIN   (1ul << 9)                                           /* 模块SPI输入引脚              */

//#define SEL_L()        LPC_GPIO0->DATA &= ~SEL_PIN
//#define SEL_H()       {LPC_GPIO0->DATA |=  SEL_PIN;}
#define SEL_L()			GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define SEL_H()			GPIO_SetBits(GPIOA,GPIO_Pin_4)

//#define SDI_L()        LPC_GPIO0->DATA &= ~SDI_PIN
//#define SDI_H()       {LPC_GPIO0->DATA |=  SDI_PIN ;}
#define SDI_L()			GPIO_ResetBits(GPIOA,GPIO_Pin_7)
#define SDI_H()			GPIO_SetBits(GPIOA,GPIO_Pin_7)

//#define CLK_L()        LPC_GPIO0->DATA &= ~CLK_PIN
//#define CLK_H()       {LPC_GPIO0->DATA |=  CLK_PIN ;}
#define CLK_L()			GPIO_ResetBits(GPIOA,GPIO_Pin_5)
#define CLK_H()			GPIO_SetBits(GPIOA,GPIO_Pin_5)

//#define SDO_READ()    ((LPC_GPIO0->DATA & SDO_PIN) != 0)
#define SDO_READ()		GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_6)

//#define RST_L()        LPC_GPIO0->DATA &= ~RST_PIN
//#define RST_H()       {LPC_GPIO0->DATA |=  RST_PIN ;}
#define RST_L()			GPIO_ResetBits(GPIOA,GPIO_Pin_8)
#define RST_H()			GPIO_SetBits(GPIOA,GPIO_Pin_8)

//#define IRQ_READ()    ((LPC_GPIO0->DATA & IRQ_PIN) != 0)                /* 读取模块中断输出引脚状态     */
#define IRQ_READ()		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)
/* 用于控制SPI总线速率，根据不同的MCU速度来调整；当前为MCU主频为48MHz    */
#define TIME   1

/*********************************************************************************************************
** Function name:        dellay
** Descriptions:         延时一小段时间
** input parameters:     i 毫秒数据
** output parameters:    无
*********************************************************************************************************/
void dellayus(unsigned int i)
{
   unsigned int j,k;
   
   for (j = 0; j <1; j++){
	   for (k = 0; k < i; k++);
   }
}

/*********************************************************************************************************
** Function name:        dellayms
** Descriptions:         延时若干毫秒
** input parameters:     uiNum: 毫秒数据
** output parameters:    无
*********************************************************************************************************/
void dellayxm(unsigned int uiNum)
{
   unsigned int i = 0;
   unsigned int j = 0;
   while(i < uiNum) {
   		 i++;
		 for(j = 0; j < 4369; j++);
   }
}

/*复位*/
void SX1276SetReset( uint8_t state )
{
	if (state == 1){
		Power_Select();
	} else {
		Power_Deselect();
	}
}


void SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

//NSS = 0;
//    GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_RESET );
    SPI_CS_Select();

    SPI_WriteByte( addr | 0x80 );
    for( i = 0; i < size; i++ )
    {
        SPI_WriteByte( buffer[i] );
    }

    //NSS = 1;
    //GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_SET );
    SPI_CS_Deselect();
}

void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

//NSS = 0;
//    GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_RESET );
    SPI_CS_Select();

    SPI_WriteByte( addr & 0x7F );

    for( i = 0; i < size; i++ )
    {
        buffer[i] = SPI_ReadByte();
    }

    //NSS = 1;
    //GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_SET );
    SPI_CS_Deselect();
}


#if 1
void SX1276Write( uint8_t addr, uint8_t data )
{
    SX1276WriteBuffer( addr, &data, 1 );
}

void SX1276Read( uint8_t addr, uint8_t *data )
{
    SX1276ReadBuffer( addr, data, 1 );
}

#endif

void SX1276WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX1276WriteBuffer( 0, buffer, size );
}

void SX1276ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX1276ReadBuffer( 0, buffer, size );
}

inline uint8_t SX1276ReadDio0( void )
{
	unsigned char state = 0;

	state = IRQ_READ();

    return ( state );
//    return GPIO_ReadInputDataBit( DIO0_IOPORT, DIO0_PIN );
}

#if 1
inline uint8_t SX1276ReadDio1( void )
{
    return 0;//GPIO_ReadInputDataBit( DIO1_IOPORT, DIO1_PIN );
}

inline uint8_t SX1276ReadDio2( void )
{
    return 0;//GPIO_ReadInputDataBit( DIO2_IOPORT, DIO2_PIN );
}

inline uint8_t SX1276ReadDio3( void )
{
    return 0;//IoePinGet( RF_DIO3_PIN );
}

inline uint8_t SX1276ReadDio4( void )
{
    return 0;//IoePinGet( RF_DIO4_PIN );
}

inline uint8_t SX1276ReadDio5( void )
{
    return 0;//IoePinGet( RF_DIO5_PIN );
}
#endif

#endif // USE_SX1276_RADIO
