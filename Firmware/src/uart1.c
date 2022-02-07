/***************************************************************************//**
 * @file        uart1.c
 * @author      Created: CLU
 * @brief       implementation of uart1.h
 ******************************************************************************/
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
uint8_t txBuffer[USART1_TX_BUFSIZE];
uint8_t rxBuffer[USART1_RX_BUFSIZE];
ringbufferHandle_t  txBufferHandle = DEFINE_RINGBUFFER(txBuffer, USART1_TX_BUFSIZE);
ringbufferHandle_t  rxBufferHandle = DEFINE_RINGBUFFER(rxBuffer, USART1_RX_BUFSIZE);

/*******************************************************************************
 * uart1_putChar
 ******************************************************************************/
bool uart1_putChar(uint8_t d) {
    bool retval = rbuf_push(&txBufferHandle, d);
    USART1->CR1 |= USART_IT_TXE;
    return retval;
}

/*******************************************************************************
 * uart1_getChar
 ******************************************************************************/
bool uart1_getChar(uint8_t* pD) {
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
	if(USART1->SR & USART_FLAG_RXNE) {
        uint8_t d = USART1->DR;
        rbuf_push(&rxBufferHandle, d);
    }

	// transmitter empty interrupt?
	if(USART1->SR & USART_FLAG_TXE) {               
		uint8_t data = 0;
		if (rbuf_pop(&txBufferHandle, &data)) USART1->DR = data;	
		else USART1->CR1 &= ~USART_IT_TXE;
	}
}

/*******************************************************************************
 * uart1_init
 ******************************************************************************/
void uart1_init() {

    //flush buffers
    rbuf_flush(&txBufferHandle);
    rbuf_flush(&rxBufferHandle);

    GPIO_InitTypeDef GPIO_InitStruct;

    // Configure USART1 Tx pin
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure USART1 Rx pin
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    //enable peripheral clock
    __USART1_CLK_ENABLE();

    //manual initialization of USART1 (HAL was not working, whyever)
    //Initialize UART1 (115200,8,1,N)    
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE;
    USART1->CR2 = 0;
    USART1->CR3 = 0;
    USART1->BRR = USART_BRR(HAL_RCC_GetPCLK2Freq(), 115200);
    USART1->CR1 |= USART_CR1_UE;

    // Peripheral interrupt init
    HAL_NVIC_SetPriority(USART1_IRQn, IRQ_PRIORITY_USART1, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    // RXNE interrupt aktivieren
    USART1->CR1 |= USART_IT_RXNE;
}

/*******************************************************************************
 * sendString
 ******************************************************************************/
void uart1_sendString(uint8_t* s) {
    uint16_t i = 0;
    while (s[i]) uart1_putChar(s[i++]);
}
