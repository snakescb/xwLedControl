/***************************************************************************//**
 * @file        ledControl.h
 * @author      Christian Luethi
 * @brief       LED steuerung
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
* @brief    initialisierung
***********************************************************************/
void ledControl_init();

/*******************************************************************//**
* @brief    startet eine LED Sequenz
***********************************************************************/
void ledControl_startSequence(uint8_t sequenceNumber);

/*******************************************************************//**
* @brief    stoppt eine LED Sequenz, Ausgang bleibt auf dem aktuellen
*           Zustand. Zustand jedes Ausgangs wird auf stop STOP gesetzt
***********************************************************************/
void ledControl_stopSequence();

/*******************************************************************//**
* @brief    setzt die pwm werte alles Ausgänge. nur möglich, wenn
*           der Zustand der Ausgänge = STOP ist.
***********************************************************************/
void ledControl_setPWM(uint8_t* values);

/*******************************************************************//**
* @brief    Regelmässig aufgerufen im 1 ms takt.
***********************************************************************/
void ledControl_update();

/*******************************************************************//**
* @brief    Regelmässig aufgerufen im main loop
***********************************************************************/
void ledControl_masterSlaveRx(uint8_t len, uint8_t* pData);

/*******************************************************************//**
* @brief    Regelmässig aufgerufen im 20 ms takt.
***********************************************************************/
void ledControl_lowSpeed();

/*******************************************************************//**
* @brief    aktiviert oder deaktiviert die led verarbeitung
***********************************************************************/
void ledControl_activate(bool enable);

/*******************************************************************//**
* @brief    Speichert die Ausgangsoptionen vor der Simulation
***********************************************************************/
bool ledControl_setSimChannelSettings(uint8_t output, uint8_t outputDim, uint8_t auxMin, uint8_t auxMax);

/*******************************************************************//**
* @brief    Speichert ein neues simulationsobjekt in der warteschlange
***********************************************************************/
bool ledControl_setSimObjects(uint8_t output, uint8_t numObjects, uint8_t* pObject);

/*******************************************************************//**
* @brief    Gets the status if the simulation buffers
***********************************************************************/
void ledControl_getBufferState(uint8_t* rsp);

/*******************************************************************//**
* @brief    Gets the info if the simulation buffers
***********************************************************************/
void ledControl_getBufferInfo(uint8_t* rsp);

/*******************************************************************//**
* @brief    Berechnet entprellten Jumper Wert
***********************************************************************/
void ledControl_jumperDebounce();

/*******************************************************************//**
* @brief    Startet die Simulation
***********************************************************************/
void ledControl_startSim(uint32_t speedInfo, uint8_t dimInfo, bool useOffsetData, uint8_t* pDirectionData, uint8_t* pOffsetData);

#endif //LEDCONTROL_H
