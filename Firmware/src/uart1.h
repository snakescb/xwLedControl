/***************************************************************************//**
 * @file        uart1.h
 * @author      Created: CLU
 * @brief       UART1 driver
 ******************************************************************************/
#ifndef UART1_H
#define UART1_H

#include "common.h"

/*******************************************************************//**
* @brief    Initializes the hardware registers of USART1
***********************************************************************/
void uart1_init(void);

/*******************************************************************//**
* @brief    Used to send single bytes
*
* Pushes a single byte to the transmit buffer
*
* @param d : byte to send
* @return    true if ok, false if tx_buffer is full
***********************************************************************/
bool uart1_putChar(uint8_t);

/*******************************************************************//**
* @brief    Used to read a single byte
*
* Gets a single byte from the receive buffer and stores it at the given
* address.
*
* @param    pD : address where data sould be stored
* @return   true if data could be read, false if receive buffer is empty
***********************************************************************/
bool uart1_getChar(uint8_t*);

/*******************************************************************//**
* @brief    Clears the transmit buffer
*
* Can be used to discard all remaining data bytes in the transmit buffer
***********************************************************************/
void uart1_txFlush(void);

/*******************************************************************//**
* @brief    Clears the receive buffer
*
* Can be used to discard all remaining data bytes in the receive buffer
***********************************************************************/
void uart1_rxFlush(void);

/*******************************************************************//**
* @brief    Used to send a string over usart2
*
* This function can be used to send a string over usart1. The string has
* to be zero-terminated. It pushes characters to the send buffer until
* a zero-character is determined or the transmit buffer is full.
***********************************************************************/
void uart1_sendString(uint8_t*);

/*******************************************************************//**
* @brief    Interrupt service handler
*
* The C interrupt handler should call this function when an interrupt
* occurs
***********************************************************************/
void uart1_isr(void);

#endif //UART1_H
