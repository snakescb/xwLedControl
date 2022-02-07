/***************************************************************************//**
 * @file        jumper.h
 * @author      Created: CLU
 * @brief       jumper
 ******************************************************************************/

#ifndef JUMPER_H
#define JUMPER_H

/******************************************************************************
 Imported Include References
******************************************************************************/
#include "common.h"

 /**************************************************************************//**
 * @brief    jumper_update
 *****************************************************************************/
uint8_t jumper_read(void);

/**************************************************************************//**
 * @brief    initialisierung
 *****************************************************************************/
void jumper_init(void);


#endif //JUMPER_H

