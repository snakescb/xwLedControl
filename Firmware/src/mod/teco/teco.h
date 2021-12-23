/***************************************************************************//**
 * @file        teco.h
 * @author      Created: DWY
 *              $Author: CLU $
 *              $LastChangedRevision: 876 $
 *              $LastChangedDate: 2010-06-16 01:46:51 +0200 (Mi, 16 Jun 2010) $
 * @brief       Test and config module
 ******************************************************************************/

#ifndef TECO_H
#define TECO_H

/******************************************************************************
 Imported Include References
******************************************************************************/
#include "common.h"
#include "hdlc.h"
#include "xwCom.h"

/*****************************************************************************
                             PUBLIC DEFINITIONS
*****************************************************************************/
void teco_commandHandler(hdlc_frameHandle_t* handle);
void teco_configHandler(hdlc_frameHandle_t* handle);
void teco_ledHandler(hdlc_frameHandle_t* handle);

/**************************************************************************//**
* @brief    Initialize the TECO.
******************************************************************************/
void teco_init(void);

/**************************************************************************//**
* @brief  Collect the chunks and if a full event is collected send the
*         aoosciated eos event.
******************************************************************************/
void teco_hisr(void);

/**************************************************************************//**
* @brief  send teco data
******************************************************************************/
void teco_send(u8 hdlcDlci, u16 dataLen, u8* pData);

/**************************************************************************//**
* @brief  Sends trace data without formatting, for space savings
*
* @param  pData: pointer to the data string to transmit, 0 terminated
******************************************************************************/
void teco_stringTrace(u8* pData);

#endif /* TECO_H */
