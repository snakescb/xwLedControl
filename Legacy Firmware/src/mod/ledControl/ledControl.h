/***************************************************************************//**
 * @file        ledControl.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 712 $
 *              $LastChangedDate: 2010-06-01 17:10:29 +0200 (Di, 01 Jun 2010) $
 *
 * LED steuerung
 ******************************************************************************/

#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include "common.h"

#define LED_NUM_OUTPUTS    6
/*******************************************************************//**
* @brief    enumerates the current runmode of the led controller
***********************************************************************/
typedef enum {
    RUNMODE_DISABLED    = 0,
    RUNMODE_MASTER      = 1,
    RUNMODE_SLAVE       = 2,
    RUNMODE_SIMULATION  = 3
} runMode_e;

/*******************************************************************//**
* @brief    enumerates the mode for seuqence selection
***********************************************************************/
typedef enum {
    SELECTION_JUMPER   = 0,
    SELECTION_RECEIVER = 1
} selectionMode_e;

/*******************************************************************//**
* @brief    initialisierung
***********************************************************************/
void ledControl_init();

/*******************************************************************//**
* @brief    startet eine LED Sequenz
***********************************************************************/
void ledControl_startSequence(u8 sequenceNumber);

/*******************************************************************//**
* @brief    stoppt eine LED Sequenz, Ausgang bleibt auf dem aktuellen
*           Zustand. Zustand jedes Ausgangs wird auf stop STOP gesetzt
***********************************************************************/
void ledControl_stopSequence();

/*******************************************************************//**
* @brief    setzt die pwm werte alles ausgänge. nur möglich, wenn
*           der Zustand der Ausgänge = STOP ist.
***********************************************************************/
void ledControl_setPWM(u8* values);

/*******************************************************************//**
* @brief    Regelmässig aufgerufen im 1 ms takt.
***********************************************************************/
void ledControl_update();

/*******************************************************************//**
* @brief    Regelmässig aufgerufen im main loop
***********************************************************************/
void ledControl_skyBusCallback(u8 port, u8 len, u8* pData);

/*******************************************************************//**
* @brief    Regelmässig aufgerufen im 20 ms takt.
***********************************************************************/
void ledControl_lowSpeed();

/*******************************************************************//**
* @brief    aktiviert oder deaktiviert die led verarbeitung
***********************************************************************/
void ledControl_activate(bool enable);

/*******************************************************************//**
* @brief    Speichert ein neues simulationsobjekt in der warteschlange
***********************************************************************/
void ledControl_setSimObjects(u8 output, u8 numObjects, u8* pObject);

/*******************************************************************//**
* @brief    Gets the status if the simulation buffers
***********************************************************************/
void ledControl_getBufferState(u8* rsp);

/*******************************************************************//**
* @brief    Gets the info if the simulation buffers
***********************************************************************/
void ledControl_getBufferInfo(u8* rsp);

/*******************************************************************//**
* @brief    Berechnet entprellten Jumper Wert
***********************************************************************/
void ledControl_jumperDebounce();

/*******************************************************************//**
* @brief    Startet die Simulation
***********************************************************************/
void ledControl_startSim(u32 speedInfo, u8 dimInfo, bool useOffsetData, u8* pDirectionData, u8* pOffsetData);

#endif //LEDCONTROL_H
