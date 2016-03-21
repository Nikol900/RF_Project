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
 * \file       sx1276.c
 * \brief      SX1276 RF chip driver
 *
 * \version    2.0.0 
 * \date       May 6 2013
 * \author     Gregory Cristian
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#include "platform.h"
#include "radio.h"
#include "include.h"
#if defined( USE_SX1276_RADIO )

#include "sx1276.h"

#include "sx1276-Hal.h"
#include "sx1276-Fsk.h"
#include "sx1276-LoRa.h"

/*!
 * SX1276 registers variable
 */
uint8_t SX1276Regs[0x70];

extern tLoRaSettings LoRaSettings;
extern uint8_t ReadSetting[SetInFlashNum];
static bool LoRaOn = false;
static bool LoRaOnState = 0;//false;

uint32_t RFFrequency;

void SX1276Init( void )
{
// Initialize FSK and LoRa registers structure

    SX1276 = ( tSX1276* )SX1276Regs;
    SX1276LR = ( tSX1276LR* )SX1276Regs;

//  SX1276InitIo( );

    SX1276Reset( );
	//载波频率初始化，从STM32寄存器中读取
	RFFrequency = (((ReadSetting[SettingRFFrequencyH]<<16)|(ReadSetting[SettingRFFrequencyM]<<8))|ReadSetting[SettingRFFrequencyH])*61.035;	

	LoRaSettings.RFFrequency = RFFrequency;
	LoRaSettings.SpreadingFactor = ReadSetting[SettingSpreadingFactor];		//扩频因子
	LoRaSettings.SignalBw = ReadSetting[SettingSignalBw];					//带宽
	LoRaSettings.Power = ReadSetting[SettingPower];							//发射功率

    // REMARK: After radio reset the default modem is FSK

#if ( LORA == 0 )

    LoRaOn = false;
    SX1276SetLoRaOn( LoRaOn );
    // Initialize FSK modem
    SX1276FskInit( );

#else

    LoRaOn = true;
    SX1276SetLoRaOn( LoRaOn );
    // Initialize LoRa modem
    SX1276LoRaInit( );

#endif

}

void SX1276Reset( void )
{
    SX1276SetReset( RADIO_RESET_ON );

// Wait 1ms
//    uint32_t startTick = GET_TICK_COUNT( );
	dellayxm(2);
	
//    while( ( GET_TICK_COUNT( ) - startTick ) < TICK_RATE_MS( 1 ) );    

    SX1276SetReset( RADIO_RESET_OFF );
    
//    Wait 6ms
//    startTick = GET_TICK_COUNT( );
//    while( ( GET_TICK_COUNT( ) - startTick ) < TICK_RATE_MS( 6 ) );    
	dellayxm(6);
}

void SX1276SetLoRaOn( bool enable )
{
    if( LoRaOnState == enable )
    {
        return;
    }
    LoRaOnState = enable;
    LoRaOn = enable;

    if( LoRaOn == true )
    {
        SX1276LoRaSetOpMode( RFLR_OPMODE_SLEEP );
        
        SX1276LR->RegOpMode = ( SX1276LR->RegOpMode & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_ON;
        SX1276Write( REG_LR_OPMODE, SX1276LR->RegOpMode );
        
        SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );
                                        // RxDone               RxTimeout                   FhssChangeChannel           CadDone
        SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
                                        // CadDetected          ModeReady
        SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
        SX1276WriteBuffer( REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2 );
        
        SX1276ReadBuffer( REG_LR_OPMODE, SX1276Regs + 1, 0x70 - 1 );
    }
    else
    {
        SX1276LoRaSetOpMode( RFLR_OPMODE_SLEEP );
        
        SX1276LR->RegOpMode = ( SX1276LR->RegOpMode & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_OFF;
        SX1276Write( REG_LR_OPMODE, SX1276LR->RegOpMode );
        
        SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );
        
        SX1276ReadBuffer( REG_OPMODE, SX1276Regs + 1, 0x70 - 1 );
    }
}

bool SX1276GetLoRaOn( void )
{
    return LoRaOn;
}

void SX1276SetOpMode( uint8_t opMode )
{
    if( LoRaOn == false )
    {
        SX1276FskSetOpMode( opMode );
    }
    else
    {
        SX1276LoRaSetOpMode( opMode );
    }
}

uint8_t SX1276GetOpMode( void )
{
    if( LoRaOn == false )
    {
        return SX1276FskGetOpMode( );
    }
    else
    {
        return SX1276LoRaGetOpMode( );
    }
}



uint8_t SX1276ReadRxGain( void )
{
    if( LoRaOn == false )
    {
        return SX1276FskReadRxGain( );
    }
    else
    {
        return SX1276LoRaReadRxGain( );
    }
}

uint8_t SX1276GetPacketRxGain( void )
{
    if( LoRaOn == false )
    {
        return SX1276FskGetPacketRxGain(  );
    }
    else
    {
        return SX1276LoRaGetPacketRxGain(  );
    }
}



uint32_t SX1276GetPacketAfc( void )
{
    if( LoRaOn == false )
    {
        return SX1276FskGetPacketAfc(  );
    }
    else
    {
         while( 1 )
         {
             // Useless in LoRa mode
             // Block program here
         }
    }
}

void SX1276StartRx( void )
{
    if( LoRaOn == false )
    {
        SX1276FskSetRFState( RF_STATE_RX_INIT );
    }
    else
    {
        SX1276LoRaSetRFState( RFLR_STATE_RX_INIT );
    }
}

void SX1276GetRxPacket( void *buffer, uint16_t *size )
{
    if( LoRaOn == false )
    {
        SX1276FskGetRxPacket( buffer, size );
    }
    else
    {
        SX1276LoRaGetRxPacket( buffer, size );
    }
}

void SX1276SetTxPacket( const void *buffer, uint16_t size )
{
    if( LoRaOn == false )
    {
        SX1276FskSetTxPacket( buffer, size );
    }
    else
    {
        SX1276LoRaSetTxPacket( buffer, size );
    }
}

uint8_t SX1276GetRFState( void )
{
    if( LoRaOn == false )
    {
        return SX1276FskGetRFState( );
    }
    else
    {
        return SX1276LoRaGetRFState( );
    }
}

void SX1276SetRFState( uint8_t state )
{
    if( LoRaOn == false )
    {
        SX1276FskSetRFState( state );
    }
    else
    {
        SX1276LoRaSetRFState( state );
    }
}

uint32_t SX1276Process( void )
{
    if( LoRaOn == false )
    {
        return SX1276FskProcess( );
    }
    else
    {
        return SX1276LoRaProcess( );
    }
}
/*********************************************************************************************************
** 2015-04-22 增加以下函数  **
*********************************************************************************************************/

uint8_t SX1276Dio0State(void)
{
    return SX1276ReadDio0();
}
/* 读取包信噪比 */
int8_t SX1276GetPacketSnr( void )
{
    if( LoRaOn == false ){
         while( 1 ){
             // Useless in FSK mode
             // Block program here
         }
    }else{
//        return SX1276LoRaGetPacketSnr();
		return GetLoRaSNR ();
    }
}
/* 读取包信号强度 */
double SX1276GetPacketRssi( void )
{
    if( LoRaOn == false ){
        return SX1276FskGetPacketRssi(  );
    }else{
//        return SX1276LoRaGetPacketRssi( );
		return GetPackLoRaRSSI();
    }
}
/* 读取当前信号强度 */
double SX1276ReadRssi( void )
{
    if( LoRaOn == false ){
        return SX1276FskReadRssi( );
    }else{
        return SX1276LoRaReadRssi( );
    }
}
/* 进入接收状态 */
void SX1276RxStateEnter( void )
{
    if( LoRaOn == false ){
//        return SX1276FskProcess();
    } else {
		LoRaRxStateEnter ();
    }
}
/* 读取接收到的数据 */
void SX1276RxDataRead(uint8_t *pbuf, uint8_t *size )
{
    if( LoRaOn == false ){
   //     return SX1276FskProcess( );
    }else{
        LoRaRxDataRead (pbuf,size );
    }
}
/* 发送数据 */
uint8_t SX1276TxData(uint8_t *pbuf, uint8_t size )
{
    if( LoRaOn == false ) {
        return SX1276FskProcess( );
    }else{
        return LoRaTxData (pbuf, size);
    }
}
/* 设置发射功率 */
void SX1276TxPower(uint8_t pwr )
{
    if( LoRaOn == false ){
//        FSK;
    } else {
        LoRaTxPower (pwr);
    }
}
/* 设置频率载波 */
void SX1276FreqSet(uint32_t freq)
{
    if( LoRaOn == false ){
//        FSK;
    } else{
        LoRaFreqSet ( freq );
    }
}
#endif // USE_SX1276_RADIO
