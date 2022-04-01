/***************************************************************************//**
 * @file        softUart.h
 * @author      Created: CLU
 * @brief       Software UART with DMA using timer 1
 ******************************************************************************/
#ifndef SOFTUART_H
#define SOFTUART_H

#include "common.h"

/******************************************************************************
 Public definitions
******************************************************************************/
#define SOFT_UART_PARITY_NONE    0
#define SOFT_UART_PARITY_EVEN    1
#define SOFT_UART_PARITY_ODD     2

#define SOFT_UART_STOPP_BITS_1   1
#define SOFT_UART_STOPP_BITS_2   2

#define SOFT_UART_DATA_BITS_7    7
#define SOFT_UART_DATA_BITS_8    8
#define SOFT_UART_DATA_BITS_9    9
#define SOFT_UART_DATA_BITS_10  10

/******************************************************************************
 Public types
******************************************************************************/
typedef struct {
    uint32_t baudrate;
    uint8_t  dataBits;
    uint8_t  stopBits;
    uint8_t  parity;
    bool     inverted;
    GPIO_TypeDef* rxPort;
    uint16_t rxPin;
    GPIO_TypeDef* txPort;
    uint16_t txPin;
} uartSettings_t;

/*******************************************************************//**
* @brief    Initialization
***********************************************************************/
void softuart_init(uartSettings_t settings);

/*******************************************************************//**
* @brief    Used to send single bytes
*
* Pushes a single byte to the transmit buffer
*
* @param d : byte to send
* @return    true if ok, false if tx_buffer is full
***********************************************************************/
bool softuart_putChar(uint16_t);

/*******************************************************************//**
* @brief    Used to read a single byte
*
* Gets a single byte from the receive buffer and stores it at the given
* address.
*
* @param    pD : address where data sould be stored
* @return   true if data could be read, false if receive buffer is empty
***********************************************************************/
bool softuart_getChar(uint16_t*);

/*******************************************************************//**
* @brief    Clears the transmit buffer
*
* Can be used to discard all remaining data bytes in the transmit buffer
***********************************************************************/
void softuart_txFlush(void);

/*******************************************************************//**
* @brief    Clears the receive buffer
*
* Can be used to discard all remaining data bytes in the receive buffer
***********************************************************************/
void softuart_rxFlush(void);

/*******************************************************************//**
* @brief    Check if transmitbuffer is empty
***********************************************************************/
bool softuart_txIdle(void);

/*******************************************************************//**
* @brief    Run in main loop to control transmitting bytes
***********************************************************************/
void softuart_hisr(void);

/*******************************************************************//**
* @brief   get rx overrun counter
***********************************************************************/
uint32_t softuart_getOverruns(void);

/**************************************************************************//**
 * @brief    isr Routines
 *****************************************************************************/
void softuart_txDmaIsr(void);
void softuart_rxDmaIsr(void);
void softuart_rxExtiIsr(void);

#endif //SOFTUART_H
