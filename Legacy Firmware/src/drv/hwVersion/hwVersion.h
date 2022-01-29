/***************************************************************************//**
 * @file        hwVersion.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 440 $
 *              $LastChangedDate: 2010-05-20 09:59:35 +0200 (Do, 20 Mai 2010) $
 * @brief       Liest die Hardware version durch identification pins
 ******************************************************************************/
#ifndef HWVERSION_H
#define HWVERSION_H

/******************************************************************************
 Imported Include References
******************************************************************************/
#include "common.h"


/**************************************************************************//**
 * @brief    initialisierung
 *****************************************************************************/
void hwVersion_init(void);

/**************************************************************************//**
 * @brief    liest die hardware version, definiert in xwCom
 *****************************************************************************/
u8 hwVersion_read(void);

#endif //HWVERSION_H

