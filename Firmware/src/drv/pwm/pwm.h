/***************************************************************************//**
 * @file        pwm.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 440 $
 *              $LastChangedDate: 2010-05-20 09:59:35 +0200 (Do, 20 Mai 2010) $
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
extern const u16* pwmValue[6];
extern const u16 pwmTable[256];

/**************************************************************************//**
 * @brief    initialisierung
 *****************************************************************************/
void pwm_init(void);

#endif //PWM_H

