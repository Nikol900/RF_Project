/* Includes -------------------------------------------------------------------*/
#include "include.h"

void NVIC_Configuration(void)
{
    #ifdef  VECT_TAB_RAM  
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
    #else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
    #endif 
    /* Configure the NVIC Preemption Priority Bits */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
}
/*********************************END OF FILE**********************************/
