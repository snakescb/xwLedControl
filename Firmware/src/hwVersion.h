/***************************************************************************//**
 * @file        hwVersion.h
 * @author      Created: CLU
 * @brief       Liest die Hardware version durch identification pins
 ******************************************************************************/
#ifndef HWVERSION_H
#define HWVERSION_H

/******************************************************************************
 Imported Include References
******************************************************************************/
#include "common.h"

extern uint8_t hwType, hwVersion;

/**************************************************************************//**
 * @brief    initialisierung
 *****************************************************************************/
void hwVersion_init(void);

/**************************************************************************//**
 * @brief    liest die hardware version, definiert in xwCom
 *****************************************************************************/
uint8_t hwVersion_read(void);

#endif //HWVERSION_H

