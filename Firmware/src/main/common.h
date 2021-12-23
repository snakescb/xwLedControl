/***************************************************************************//**
 * @file        common.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 980 $
 *              $LastChangedDate: 2010-06-17 17:31:00 +0200 (Do, 17 Jun 2010) $
 * @brief       Common header file. Should be included in all source code
 *              documents within this project.
 ******************************************************************************/

#include "stm32f10x.h"
#include "system_stm32f10x.h"
#include "stm32f10x_conf.h"

//*****************************************************************************
// LOCAL DEFINITIONS
//*****************************************************************************
#define SW_VER_MAJOR         5
#define SW_VER_MINOR         0

//*****************************************************************************
// LOCAL CONTROL SWITCHES
//*****************************************************************************
#define TRACE(n) teco_stringTrace((u8*)n)

#define null  0
#define false 0
#define FALSE 0
#define true  1
#define TRUE  1
#define bool  u8
#ifndef NULL
    #define NULL  null
#endif
