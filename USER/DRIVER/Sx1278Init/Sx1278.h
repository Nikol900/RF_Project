/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H
#define __SPI_H
/* Includes ------------------------------------------------------------------*/
#include "type.h"
/* Exported Functions --------------------------------------------------------*/

void SPI_Configuration(void);
void SPI_WriteByte(INT8U TxData);
INT8U SPI_ReadByte(void);
void SPI_CS_Select(void);
void SPI_CS_Deselect(void);
void Power_Select(void);
void Power_Deselect(void);
void Sx1278Init(void);
void Spi_GpioInit(void);
void PowerKeyInit(void);
void Sx1276RecvPinInit(void);

#endif /* __SPI_H */
/*********************************END OF FILE**********************************/
