/***************************************************************************//**
 * @file        common.h
 * @author      Created: CLU
 * @brief       Common header file. Should be included in all source code
 *              documents within this project.
 ******************************************************************************/
#ifndef _COMMON_H_
#define _COMMON_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

//*****************************************************************************
// FIRMWARE VERSION
//*****************************************************************************
#define SW_VER_MAJOR         1
#define SW_VER_MINOR         4

//*****************************************************************************
// LOCAL CONTROL SWITCHES
//*****************************************************************************
#define TRACE(n) teco_stringTrace((uint8_t*)n)
#define null  0
#define false 0
#define FALSE 0
#define true  1
#define TRUE  1
#define bool  uint8_t
#ifndef NULL
    #define NULL  null
#endif

#endif /* _COMMON_H_ */