/***************************************************************************//**
 * @file        crc.h
 * @author      Created: CLU
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
uint16_t crc_init(void);

/**************************************************************************//**
 * @brief    berechnet aus der alten crc und den daten eine neue crc
 *
 * @param    crcOld : die alte crc
 * @param    data   : die neuen daten
 * @return   neue crc
 *****************************************************************************/
uint16_t crc_update(uint16_t crcOld, uint8_t data);

/**************************************************************************//**
 * @brief    schliesst eine crc ab
 *
 * @param    crcOld : die alte crc
 * @return   abgeschlossene crc
 *****************************************************************************/
uint16_t crc_finish(uint8_t crcOld);

/**************************************************************************//**
 * @brief    berechnet crc über einen datenblock
 *
 * @param    pStart : pointer auf datenarray
 * @param    len    : länge des datenblocks
 * @return   berechnete crc
 *****************************************************************************/
uint16_t crc_calc(uint8_t* pStart, uint32_t len);

#endif //CRC_H

