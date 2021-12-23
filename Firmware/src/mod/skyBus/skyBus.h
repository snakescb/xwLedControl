/***************************************************************************//**
 * @file        skyBus.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 712 $
 *              $LastChangedDate: 2010-06-01 17:10:29 +0200 (Di, 01 Jun 2010) $
 *
 * Inter Board Protokoll
 ******************************************************************************/

#ifndef SKYBUS_H
#define SKYBUS_H

/* Includes ------------------------------------------------------------------*/
#include "common.h"

/* Defines ------------------------------------------------------------------*/
#define SKYBUS_MODE_RX_ONLY 0
#define SKYBUS_MODE_RX_TX   1

/* Types ------------------------------------------------------------------*/
typedef void (*skybusCallback_t)(u8 skyPort, u8 datalen, u8* pData);

/**************************************************************************//**
 * @brief    initialisiert die Kommunikation
 *****************************************************************************/
void skyBus_init(u8 skyBusMode);

/**************************************************************************//**
 * @brief    Callback registrieren, wird aufgerufen wenn ein Paket empfangen wird
 *****************************************************************************/
void skyBus_registerCallback(skybusCallback_t f);

/**************************************************************************//**
 * @brief    Gibt true zurück, falls der Bus am senden ist
 *****************************************************************************/
bool skyBus_tx_busy();

/**************************************************************************//**
 * @brief    hisr
 *****************************************************************************/
void skyBus_hisr(void);

/**************************************************************************//**
 * @brief    Sendet ein Datenframe
 *****************************************************************************/
void skyBus_send(u8 port, u8 len, u8* data);

#endif //SKYBUS_H
