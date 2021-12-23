/***************************************************************************//**
 * @file        simpleString.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 712 $
 *              $LastChangedDate: 2010-06-01 17:10:29 +0200 (Di, 01 Jun 2010) $
 *
 * Einfache Stringfunktionen ohne C libs
 ******************************************************************************/

#ifndef SIMPLESTRING_H
#define SIMPLESTRING_H

/* Includes ------------------------------------------------------------------*/
#include "common.h"

/**************************************************************************//**
 * @brief    generiert ein string aus einem signed int 32
 *
 * @return   pointer auf den String
 *****************************************************************************/
u8* intToString(s32 value, bool cr);

#endif //SIMPLESTRING_H
