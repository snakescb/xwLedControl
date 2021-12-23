/***************************************************************************//**
 * @file        dreamExtension.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 712 $
 *              $LastChangedDate: 2010-06-01 17:10:29 +0200 (Di, 01 Jun 2010) $
 *
 * Protokoll für dreamExtensions
 ******************************************************************************/

#ifndef DREAMEXTENSION_H
#define DREAMEXTENSION_H

/* Includes ------------------------------------------------------------------*/
#include "common.h"

/* Defines ------------------------------------------------------------------*/
#define DREAMEXTENSION_ADDRESS_BROADCAST 0xFF

/* Types ------------------------------------------------------------------*/
/**************************************************************************//**
 * @brief    Startet ein Dream Effekt
 *****************************************************************************/
void dreamEffect_Start(u8 boardAddress, u16 numPixels, u16 runTime, u16 fadeTime, u8 brightness, u8 effectNumber, u8* pOptionBytes);

/**************************************************************************//**
 * @brief    Stoppt einen Effekt
 *****************************************************************************/
void dreamEffect_StopImmediate(u8 boardAddress);

/**************************************************************************//**
 * @brief    Stoppt einen Effekt
 *****************************************************************************/
void dreamEffect_Stop(u8 boardAddress);

#endif //DREAMEXTENSION_H
