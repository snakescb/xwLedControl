/***************************************************************************//**
 * @file        statusLed.h
 * @author      Created: CLU
 * @brief       Treiber für die Statusled
 ******************************************************************************/
#ifndef STATUSLED_H
#define STATUSLED_H

/******************************************************************************
 Imported Include References
******************************************************************************/

/**************************************************************************//**
 * @brief    definiert die m�glichen Zust�nde f�r die Status LED
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
 * @brief    initialisierung
 *****************************************************************************/
void statusLed_init(void);

/**************************************************************************//**
 * @brief    update, sollte vom system timer aufgerufen werden
 *****************************************************************************/
void statusLed_update(void);

/**************************************************************************//**
 * @brief    setzt neuen zustanf
 *****************************************************************************/
void statusLed_setState(eStausLedState state);

#endif //STATUSLED_H

