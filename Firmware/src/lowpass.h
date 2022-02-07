/***************************************************************************//**
 * @file        lowpass.h
 * @author      Created: CLU
 * @brief       Lowpass filters using averaging, for positive values only
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
    uint16_t* pBuffer;
    uint16_t  averageLentgh;
    uint16_t  averageShift;
    uint16_t  averageIndex;
    uint32_t  averageSum;
} lowpassHandle_t;

/*******************************************************************//**
* @brief    update the filter and get the filtered value
***********************************************************************/
uint16_t lowpass_update(lowpassHandle_t* h, uint16_t newValue);

/*******************************************************************//**
* @brief    init the filter buffer
***********************************************************************/
void lowpass_init(lowpassHandle_t* h);

#endif //LOWPASS_H
