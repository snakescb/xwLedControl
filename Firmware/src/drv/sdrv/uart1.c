/***************************************************************************//**
 * @file        uart1.c
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 440 $
 *              $LastChangedDate: 2010-05-20 09:59:35 +0200 (Do, 20 Mai 2010) $
 * @brief       implementation of uart1.h
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "uart1.h"
#include "rbuf.h"
#include "interrupt.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USART1_TX_BUFSIZE   256
#define USART1_RX_BUFSIZE   512

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//create and initialize the ringhbuffers
u8 txBuffer[USART1_TX_BUFSIZE];
u8 rxBuffer[USART1_RX_BUFSIZE];
ringbufferHandle_t  txBufferHandle = DEFINE_RINGBUFFER(txBuffer, USART1_TX_BUFSIZE);
ringbufferHandle_t  rxBufferHandle = DEFINE_RINGBUFFER(rxBuffer, USART1_RX_BUFSIZE);

/*******************************************************************************
 * uart1_putChar
 ******************************************************************************/
bool uart1_putChar(u8 d) {
    bool retval = rbuf_push(&txBufferHandle, d);
    //enable the TXE interrupt
    *((u32*)(((u32)USART1) + 0x0C)) |= 0x80;
    return retval;
}

/*******************************************************************************
 * uart1_getChar
 ******************************************************************************/
bool uart1_getChar(u8* pD) {
    return rbuf_pop(&rxBufferHandle, pD);
}

/*******************************************************************************
 * uart1_txFlush
 ******************************************************************************/
void uart1_txFlush(void) {
    rbuf_flush(&txBufferHandle);
}

/*******************************************************************************
 * uart1_rxFlush
 ******************************************************************************/
void uart1_rxFlush(void) {
    rbuf_flush(&rxBufferHandle);
}

/*******************************************************************************
 * uart1_isr
 ******************************************************************************/
void uart1_isr(void) {
    //receive interrupt?
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
        //push data to the buffer
        rbuf_push(&rxBufferHandle, USART_ReceiveData(USART1));
	}

	// transmitter empty interrupt?
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET){
		u8 data = 0;
       //check if data to send is available
		if (rbuf_pop(&txBufferHandle, &data)) {
			//if so, then send the next byte
			USART_SendData(USART1, data);
		}
		else {
		 	//else disable transmit interrupt again
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
		}
	}
}

/*******************************************************************************
 * uart1_init
 ******************************************************************************/
void uart1_init() {

    //flush buffers
    rbuf_flush(&txBufferHandle);
    rbuf_flush(&rxBufferHandle);

    USART_InitTypeDef USART_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    //first, deinit usart
    USART_DeInit(USART1);

    // Configure USART1 Rx as input floating
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure USART1 Tx as alternate function push-pull
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Enable Clock of USART1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    // Initialize UART1 (115200,8,1,N)
    USART_StructInit(&USART_InitStruct);
    USART_InitStruct.USART_BaudRate = 115200;
    USART_Init(USART1, &USART_InitStruct);

    // Enable USART1 Receive interrupt
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // Configure USART1 interrupts
    // Apply medium to low priority
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = IRQ_PRIORITY_USART1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    //USART1 aktivieren
    USART_Cmd(USART1, ENABLE);
}

/*******************************************************************************
 * sendString
 ******************************************************************************/
void uart1_sendString(u8* s) {
    u16 i = 0;
    while (s[i]) {
        uart1_putChar(s[i]);
        i++;
    }
}
