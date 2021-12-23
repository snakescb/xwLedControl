/***************************************************************************//**
 * @file        lowpass.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 712 $
 *              $LastChangedDate: 2010-06-01 17:10:29 +0200 (Di, 01 Jun 2010) $
 *
 * Lowpass filters using averaging, for positive values only
 ******************************************************************************/

#ifndef LOWPASS_H
#define LOWPASS_H

#include "common.h"

///this macro can be used to initializ a lowpass handle
#define DEFINE_LOWPASS_FILTER(pBuf,len,shift) {pBuf,len,shift,0,0}

/*******************************************************************//**
* @brief    Defines the handle used for lowpass filters
***********************************************************************/
typedef struct {
    u16* pBuffer;
    u16  averageLentgh;
    u16  averageShift;
    u16  averageIndex;
    u32  averageSum;
} lowpassHandle_t;

/*******************************************************************//**
* @brief    update the filter and get the filtered value
***********************************************************************/
u16 lowpass_update(lowpassHandle_t* h, u16 newValue);

/*******************************************************************//**
* @brief    init the filter buffer
***********************************************************************/
void lowpass_init(lowpassHandle_t* h);

#endif //LOWPASS_H
