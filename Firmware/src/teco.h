/***************************************************************************//**
 * @file        teco.h
 * @author      Created: Christian Luethi
 * @brief       Test and config module
 ******************************************************************************/

#ifndef TECO_H
#define TECO_H

/******************************************************************************
 Imported Include References
******************************************************************************/
#include "common.h"
#include "com.h"
#include "xwCom.h"

/**************************************************************************//**
* @brief    Initialize the TECO.
******************************************************************************/
void teco_init(void);

/**************************************************************************//**
* @brief  Collect the chunks and if a full frame is received process it
******************************************************************************/
void teco_hisr(void);

/**************************************************************************//**
* @brief  send teco data
******************************************************************************/
void teco_send(uint8_t dataScope, uint16_t dataLen, uint8_t* pData);

/**************************************************************************//**
* @brief  Sends trace data
******************************************************************************/
void teco_stringTrace(uint8_t* s);

#endif /* TECO_H */