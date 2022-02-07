/***************************************************************************//**
 * @file        statusLed.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 440 $
 *              $LastChangedDate: 2010-05-20 09:59:35 +0200 (Do, 20 Mai 2010) $
 * @brief       Treiber für die Statusled
 ******************************************************************************/
#ifndef STATUSLED_H
#define STATUSLED_H

/******************************************************************************
 Imported Include References
******************************************************************************/
#include "common.h"

/**************************************************************************//**
 * @brief    definiert die möglichen Zustände für die Status LED
 *****************************************************************************/
 typedef enum {
    STATUS_LED_OFF = 0,
    STATUS_LED_ON,
    STATUS_LED_BLINK_1,
    STATUS_LED_BLINK_2,
    STATUS_LED_BLINK_3,
    STATUS_NUM_STATES
 } eStausLedState;

 /**************************************************************************//**
 * @brief    definiert die möglichen Zustände für die Status LED
 *****************************************************************************/
 typedef enum {
    STATUS_LED_MODE_NORMAL = 0,
    STATUS_LED_MODE_SENSORTEST,
 } eStausLedMode;

/**************************************************************************//**
 * @brief    initialisierung
 *****************************************************************************/
void statusLed_init(void);

/**************************************************************************//**
 * @brief    update, sollte vom system timer aufgerufen werden
 *****************************************************************************/
void statusLed_update(void);

/**************************************************************************//**
 * @brief    Setzt den modus
 *****************************************************************************/
void statusLed_setMode(eStausLedMode mode);

/**************************************************************************//**
 * @brief    setzt neuen zustanf
 *****************************************************************************/
void statusLed_setState(eStausLedState state);

#endif //STATUSLED_H

