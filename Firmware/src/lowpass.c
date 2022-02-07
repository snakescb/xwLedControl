/***************************************************************************//**
 * @file        lowpass.c
 * @author      Created: CLU
 * @brief       Lowpass filters using averaging, for positive values only
 ******************************************************************************/
 #include "lowpass.h"

/*******************************************************************************
 * lowpass_update
 ******************************************************************************/
uint16_t lowpass_update(lowpassHandle_t* h, uint16_t newValue) {

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

    for (uint16_t i=0; i<(h->averageLentgh); i++) {
        h->pBuffer[i] = 0;
    }
}
