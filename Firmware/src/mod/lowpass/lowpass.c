/***************************************************************************//**
 * @file        lowpass.c
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 712 $
 *              $LastChangedDate: 2010-06-01 17:10:29 +0200 (Di, 01 Jun 2010) $
 *
 * Lowpass filters using averaging, for positive values only
 ******************************************************************************/
 #include "lowpass.h"

/*******************************************************************************
 * lowpass_update
 ******************************************************************************/
u16 lowpass_update(lowpassHandle_t* h, u16 newValue) {

    h->averageSum -= h->pBuffer[h->averageIndex];
    h->pBuffer[h->averageIndex] = newValue;
    h->averageSum += newValue;

    h->averageIndex++;
    if ((h->averageIndex) >= (h->averageLentgh)) h->averageIndex = 0;

    return ((h->averageSum) >> (h->averageShift));
}

/*******************************************************************************
 * lowpass_init
 ******************************************************************************/
void lowpass_init(lowpassHandle_t* h) {

    for (u16 i=0; i<(h->averageLentgh); i++) {
        h->pBuffer[i] = 0;
    }

}
