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
    //__HAL_USART_ENABLE_IT(&husart1, USART_IT_TXE);

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
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4); 	

        //d++;
        //rbuf_push(&rxBufferHandle, d);
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
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure USART1 Rx pin
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    __USART1_CLK_ENABLE();

    // Initialize UART1 (115200,8,1,N)
    USART_HandleTypeDef husart1;
    husart1.Instance = USART1;
    husart1.Init.BaudRate = 115200;
    husart1.Init.WordLength = UART_WORDLENGTH_8B;
    husart1.Init.StopBits = UART_STOPBITS_1;
    husart1.Init.Parity = UART_PARITY_NONE;
    husart1.Init.Mode = USART_MODE_TX_RX;
    HAL_USART_DeInit(&husart1);
    HAL_USART_Init(&husart1);

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
