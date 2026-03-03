#ifndef _PORT_CLK_CONFIG_H
#define _PORT_CLK_CONFIG_H


#include "stm32f4xx.h"

#define SLOT PEOUT(12)
#define RS485_TEN  PAOUT(9)

#define TRG PDOUT(8)
void Port_Clk_Config( void );

#endif

