/***************************************************************************//**
 * @file        pwm.h
 * @author      Created: CLU
 * @brief       PWM Treiber für LED Ausgänge
 ******************************************************************************/
#ifndef PWM_H
#define PWM_H

/******************************************************************************
 Imported Include References
******************************************************************************/
#include "common.h"

/**************************************************************************//**
 * @brief   Erzeugt die Referenz auf das pointer array, über welches direkt
 *          auf das timerregister des jeweiligen PWM Ausgangs zugegriffen
 *          werden kann.
 *****************************************************************************/
extern const uint16_t* pwmValue[6];
extern const uint16_t pwmTable[256];

/**************************************************************************//**
 * @brief    initialisierung
 *****************************************************************************/
void pwm_init(void);

#endif //PWM_H

