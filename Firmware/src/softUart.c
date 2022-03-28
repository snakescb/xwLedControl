/***************************************************************************//**
 * @file        softUart.c
 * @author      Created: CLU
 * @brief       software UART with DMA using timer 1
 ******************************************************************************/
#include "rbuf.h"
#include "interrupt.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SOFTUART_TX_BUFSIZE   128
#define SOFTUART_RX_BUFSIZE   128

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//create and initialize the ringhbuffers
uint8_t softUart_txBuffer[SOFTUART_TX_BUFSIZE];
uint8_t softUart_rxBuffer[SOFTUART_RX_BUFSIZE];
ringbufferHandle_t softUart_txBufferHandle = DEFINE_RINGBUFFER(softUart_txBuffer, SOFTUART_TX_BUFSIZE);
ringbufferHandle_t softUart_rxBufferHandle = DEFINE_RINGBUFFER(softUart_rxBuffer, SOFTUART_RX_BUFSIZE);

/*******************************************************************************
 * softUart_putChar
 ******************************************************************************/
bool softUart_putChar(uint8_t d) {
    return false;
}

/*******************************************************************************
 * softUart_getChar
 ******************************************************************************/
bool softUart_getChar(uint8_t* pD) {
    return false;
}

/*******************************************************************************
 * softUart_txFlush
 ******************************************************************************/
void softUart_txFlush(void) {
    rbuf_flush(&softUart_txBufferHandle);
}

/*******************************************************************************
 * softUart_rxFlush
 ******************************************************************************/
void softUart_rxFlush(void) {
    rbuf_flush(&softUart_rxBufferHandle);
}

/*******************************************************************************
 * softUart_init
 ******************************************************************************/
void softUart_init() {

}
