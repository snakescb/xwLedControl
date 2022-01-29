/***************************************************************************//**
 * @file        uart3.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 641 $
 *              $LastChangedDate: 2010-05-28 15:59:01 +0200 (Fr, 28 Mai 2010) $
 * @brief       UART3 treiber, wird benötigt für sollvorgaben für die UART Regler
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "uart3.h"
#include "rbuf.h"
#include "interrupt.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USART3_TX_BUFSIZE   256
#define USART3_RX_BUFSIZE   256

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//create and initialize the ringhbuffers
u8 uart3_txBuffer[USART3_TX_BUFSIZE];
u8 uart3_rxBuffer[USART3_RX_BUFSIZE];
ringbufferHandle_t uart3_txBufferHandle = DEFINE_RINGBUFFER(uart3_txBuffer, USART3_TX_BUFSIZE);
ringbufferHandle_t uart3_rxBufferHandle = DEFINE_RINGBUFFER(uart3_rxBuffer, USART3_RX_BUFSIZE);
bool usart3txBusy, usart3txActive;

/*******************************************************************************
 * uart3_putChar
 ******************************************************************************/
bool uart3_putChar(u8 d) {
    bool retval = rbuf_push(&uart3_txBufferHandle, d);
    //enable the TXE interrupt
    *((u32*)(((u32)USART3) + 0x0C)) |= 0x80;
    return retval;
}

/*******************************************************************************
 * uart3_getChar
 ******************************************************************************/
bool uart3_getChar(u8* pD) {
    return rbuf_pop(&uart3_rxBufferHandle, pD);
}

/*******************************************************************************
 * uart3_txFlush
 ******************************************************************************/
void uart3_txFlush(void) {
    rbuf_flush(&uart3_txBufferHandle);
}

/*******************************************************************************
 * uart1_rxFlush
 ******************************************************************************/
void uart3_rxFlush(void) {
    rbuf_flush(&uart3_rxBufferHandle);
}

/*******************************************************************************
 * uart3_txSending
 ******************************************************************************/
bool uart3_txSending(void) {
    return usart3txBusy;
}

/*******************************************************************************
 * uart3_txActive
 ******************************************************************************/
bool uart3_txActive(void) {
    return usart3txActive;
}

/*******************************************************************************
 * uart3_isr
 ******************************************************************************/
void uart3_isr(void) {

    //receive interrupt?
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){
        //push data to the buffer
        rbuf_push(&uart3_rxBufferHandle, USART_ReceiveData(USART3));
	}

	// transmitter empty interrupt?
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET){
        u8 data = 0;
       //check if data to send is available
		if (rbuf_pop(&uart3_txBufferHandle, &data)) {
			//if so, then send the next byte
			usart3txBusy = true;
			USART_SendData(USART3, data);
		}
		else {
		    usart3txBusy = false;
		 	//else disable transmit interrupt again
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
		}
	}
}

/*******************************************************************************
 * uart1_init
 ******************************************************************************/
void uart3_init(u8 mode) {

    //flush buffers
    rbuf_flush(&uart3_txBufferHandle);
    rbuf_flush(&uart3_rxBufferHandle);

    USART_InitTypeDef USART_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    //first, deinit usart
    USART_DeInit(USART3);
    usart3txActive = false;

    // Initialize UART3 (230400,8,1,N)
    USART_StructInit(&USART_InitStruct);
    USART_InitStruct.USART_Mode = 0;
    USART_InitStruct.USART_BaudRate = 115200;

    if (mode & UART3_RX) {
        // Configure USART3 Rx as input pull up
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init(GPIOB, &GPIO_InitStruct);
        USART_InitStruct.USART_Mode += USART_Mode_Rx;
    }
    if (mode & UART3_TX) {
        //Configure USART3 Tx as alternate function push-pull
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOB, &GPIO_InitStruct);
        USART_InitStruct.USART_Mode += USART_Mode_Tx;
        usart3txActive = true;
    }

    // Enable Clock of USART3
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    //init peripherial
    USART_Init(USART3, &USART_InitStruct);

    // Enable USART3 Receive interrupt
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    // Configure USART3 interrupts
    // Apply medium to low priority
    NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = IRQ_PRIORITY_USART3;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    usart3txBusy = false;

    //USART1 aktivieren
    USART_Cmd(USART3, ENABLE);
}
