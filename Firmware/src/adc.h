/***************************************************************************//**
 * @file        adc.h
 * @author      Created: CLU
 * @brief       Analog to digital converter driver
 ******************************************************************************/

#ifndef ADC_H
#define ADC_H

/* Includes ------------------------------------------------------------------*/
#include "common.h"

/* Defines -------------------------------------------------------------------*/

/**************************************************************************//**
 * @brief    dies macht die adc rohwerte sichtbar in allen
 *           modulen die adc.h einbinden
 *****************************************************************************/
extern uint16_t adc_battery;

/**************************************************************************//**
 * @brief    initialisiert die nötigen hardwareregister
 *****************************************************************************/
void adc_init(void);

/**************************************************************************//**
 * @brief    startet eine messung
 *****************************************************************************/
void adc_start(void);

#endif //ADC_H
