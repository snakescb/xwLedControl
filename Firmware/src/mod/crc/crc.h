/***************************************************************************//**
 * @file        crc.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 440 $
 *              $LastChangedDate: 2010-05-20 09:59:35 +0200 (Do, 20 Mai 2010) $
 * @brief       functions for recursive 16bit crc computations
 ******************************************************************************/

#ifndef CRC_H
#define CRC_H

/******************************************************************************
 * Imported Include References
 ******************************************************************************/
#include "common.h"

/**************************************************************************//**
 * @brief    initialisiert eine crc auf den startwert
 *
 * @return   neue crc
 *****************************************************************************/
u16 crc_init(void);

/**************************************************************************//**
 * @brief    berechnet aus der alten crc und den daten eine neue crc
 *
 * @param    crcOld : die alte crc
 * @param    data   : die neuen daten
 * @return   neue crc
 *****************************************************************************/
u16 crc_update(u16 crcOld, u8 data);

/**************************************************************************//**
 * @brief    schliesst eine crc ab
 *
 * @param    crcOld : die alte crc
 * @return   abgeschlossene crc
 *****************************************************************************/
u16 crc_finish(u8 crcOld);

/**************************************************************************//**
 * @brief    berechnet crc über einen datenblock
 *
 * @param    pStart : pointer auf datenarray
 * @param    len    : länge des datenblocks
 * @return   crc
 *****************************************************************************/
u16 crc_calc(u8* pStart, u32 len);

#endif //CRC_H

