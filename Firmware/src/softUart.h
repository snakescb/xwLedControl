/***************************************************************************//**
 * @file        softUart.h
 * @author      Created: CLU
 * @brief       software UART with DMA using timer 1
 ******************************************************************************/
#ifndef SOFTUART_H
#define SOFTUART_H

#include "common.h"

/*******************************************************************//**
* @brief    Initialization
***********************************************************************/
void softUart_init(void);

/*******************************************************************//**
* @brief    Used to send single bytes
*
* Pushes a single byte to the transmit buffer
*
* @param d : byte to send
* @return    true if ok, false if tx_buffer is full
***********************************************************************/
bool softUart_putChar(uint8_t);

/*******************************************************************//**
* @brief    Used to read a single byte
*
* Gets a single byte from the receive buffer and stores it at the given
* address.
*
* @param    pD : address where data sould be stored
* @return   true if data could be read, false if receive buffer is empty
***********************************************************************/
bool softUart_getChar(uint8_t*);

/*******************************************************************//**
* @brief    Clears the transmit buffer
*
* Can be used to discard all remaining data bytes in the transmit buffer
***********************************************************************/
void softUart_txFlush(void);

/*******************************************************************//**
* @brief    Clears the receive buffer
*
* Can be used to discard all remaining data bytes in the receive buffer
***********************************************************************/
void softUart_rxFlush(void);

#endif //SOFTUART_H
