/***************************************************************************//**
 * @file        uart3.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 641 $
 *              $LastChangedDate: 2010-05-28 15:59:01 +0200 (Fr, 28 Mai 2010) $
 * @brief       UART3 treiber, wird benötigt für master->slave kommunikation
 ******************************************************************************/

#ifndef UART3_H
#define UART3_H

#include "common.h"
#include "rbuf.h"

#define UART3_TX    1
#define UART3_RX    2

/*******************************************************************//**
* @brief    Initializes the hardware registers of USART1
***********************************************************************/
void uart3_init(u8 mode);

/*******************************************************************//**
* @brief    Used to send single bytes
*
* Pushes a single byte to the transmit buffer
*
* @param d : byte to send
* @return    true if ok, false if tx_buffer is full
***********************************************************************/
bool uart3_putChar(u8);

/*******************************************************************//**
* @brief    Used to read a single byte
*
* Gets a single byte from the receive buffer and stores it at the given
* address.
*
* @param    pD : address where data sould be stored
* @return   true if data could be read, false if receive buffer is empty
***********************************************************************/
bool uart3_getChar(u8*);

/*******************************************************************//**
* @brief    Clears the transmit buffer
*
* Can be used to discard all remaining data bytes in the transmit buffer
***********************************************************************/
void uart3_txFlush(void);

/*******************************************************************//**
* @brief    Clears the receive buffer
*
* Can be used to discard all remaining data bytes in the receive buffer
***********************************************************************/
void uart3_rxFlush(void);

/*******************************************************************//**
* @brief    Interrupt service handler
*
* The C interrupt handler should call this function when an interrupt
* occurs
***********************************************************************/
void uart3_isr(void);

/*******************************************************************//**
* @brief    Prüft ob der uart noch am senden ist
***********************************************************************/
bool uart3_txSending(void);

/*******************************************************************//**
* @brief    Prüft der uart sender aktiviert ist
***********************************************************************/
bool uart3_txActive(void);

#endif //UART3_H
