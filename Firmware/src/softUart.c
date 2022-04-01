/***************************************************************************//**
 * @file        softUart.c
 * @author      Created: CLU
 * @brief       Software UART with DMA using timer 1
 ******************************************************************************/
#include "rbuf.h"
#include "interrupt.h"
#include "softUart.h"
#include "teco.h"
#include <stdio.h>

/* Private define ------------------------------------------------------------*/
#define SOFTUART_TX_BUFSIZE     64
#define SOFTUART_RX_BUFSIZE     64
#define SOFTUART_REGISTER_SIZE  16 //1 start, 1 parity, max 2 stop and max 10 databits

#define SOFTUART_HALFDUPLEX_UNUSED     0
#define SOFTUART_HALFDUPLEX_TRANSMIT   1
#define SOFTUART_HALFDUPLEX_RECEIVE    2

#define IRQ_PRIORITY_EXTI      0
#define IRQ_PRIORITY_RX_DMA    1
#define IRQ_PRIORITY_TX_DMA    2

/* Private typedef -----------------------------------------------------------*/
typedef struct {       
    uint8_t   halfDuplexMode;  
    uint8_t   txDmaBufSize;
    uint8_t   rxDmaBufSize;
    uint32_t  txRegister[SOFTUART_REGISTER_SIZE];
    uint16_t  rxRegister1[SOFTUART_REGISTER_SIZE];
    uint16_t  rxRegister2[SOFTUART_REGISTER_SIZE];
    uint16_t* pReceiveRegister;
    uint16_t* pDecodeRegister;
    uint32_t  tx_set_0;
    uint32_t  tx_set_1;
    GPIO_InitTypeDef txLine;
    uint32_t  halfBitTime;
    uint32_t  parityErrorCount;
    uint32_t  extiLine;
    uint32_t  rxOverRunCounter;
    uint32_t  rxDmaRequestTime;
    bool      isInitialized;
} uartControl_t;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t softuart_txBuffer[SOFTUART_TX_BUFSIZE];
uint16_t softuart_rxBuffer[SOFTUART_RX_BUFSIZE];

typedef struct {
    uint16_t* pBuffer;
    uint16_t  bufsize;
    uint16_t  writeIndex;
    uint16_t  readIndex;
} softuart_rbufHandle_t;
softuart_rbufHandle_t softuart_txBufferHandle, softuart_rxBufferHandle; 

uartSettings_t uartSettings;
uartControl_t  uartControl;

/* Private functions ---------------------------------------------------------*/
void softuart_setHalfDuplexLineMode(bool transmit);
void softuart_prepareTxBuffer(uint32_t* buffer, uint16_t data);
void softuart_buffer_flush(softuart_rbufHandle_t* h);
bool softuart_buffer_push(softuart_rbufHandle_t* h, uint16_t e);
bool softuart_buffer_pop(softuart_rbufHandle_t* h, uint16_t* pE);

/*******************************************************************************
 * softUart_putChar
 ******************************************************************************/
bool softuart_putChar(uint16_t d) {
    return softuart_buffer_push(&softuart_txBufferHandle, d);
}

/*******************************************************************************
 * softuart_getChar
 ******************************************************************************/
bool softuart_getChar(uint16_t* pD) {
    return softuart_buffer_pop(&softuart_rxBufferHandle, pD);
}

/*******************************************************************************
 * softuart_txFlush
 ******************************************************************************/
void softuart_txFlush(void) {
    softuart_buffer_flush(&softuart_txBufferHandle);  
}

/*******************************************************************************
 * softuart_rxFlush
 ******************************************************************************/
void softuart_rxFlush(void) {
    softuart_buffer_flush(&softuart_rxBufferHandle);    
}

/*******************************************************************************
 * softuart_getOverruns
 ******************************************************************************/
uint32_t softuart_getOverruns(void) {
    return uartControl.rxOverRunCounter;
}

/*******************************************************************************
 * softuart_prepareTxBuffer
 ******************************************************************************/
void softuart_prepareTxBuffer(uint32_t* buffer, uint16_t data) {    
    uint8_t cnt = 0;

    //startbit
    buffer[cnt++] = uartControl.tx_set_0;

    //data bits
    uint8_t   oneCount = 0;
    uint16_t  mask = 0x01;
    for (uint8_t i=0; i<uartSettings.dataBits; i++) {
        if (data & mask) { buffer[cnt++] = uartControl.tx_set_1; oneCount++; }
        else buffer[cnt++] = uartControl.tx_set_0;
        mask = mask << 1;
    }

    //parity bit
    if (uartSettings.parity == SOFT_UART_PARITY_EVEN) {
        if (oneCount & 0x01) buffer[cnt++] = uartControl.tx_set_1;
        else buffer[cnt++] = uartControl.tx_set_0;
    }
    if (uartSettings.parity == SOFT_UART_PARITY_ODD) {
        if (oneCount & 0x01) buffer[cnt++] = uartControl.tx_set_0;
        else buffer[cnt++] = uartControl.tx_set_1;
    }

    //stop bits
    for (uint8_t i=0; i<uartSettings.stopBits; i++) buffer[cnt++] = uartControl.tx_set_1;
}

/*******************************************************************************
 * softuart_setHalfDuplexLineMode
 ******************************************************************************/
void softuart_setHalfDuplexLineMode(bool transmit) {
    if (transmit) {
        uartControl.txLine.Mode = GPIO_MODE_OUTPUT_PP;
        uartControl.halfDuplexMode = SOFTUART_HALFDUPLEX_TRANSMIT;
        EXTI->IMR &= ~uartControl.extiLine;
    }
    else {
        uartControl.txLine.Mode = GPIO_MODE_INPUT;
        if (uartSettings.inverted) uartControl.txLine.Pull = GPIO_PULLDOWN;
        else uartControl.txLine.Pull = GPIO_PULLUP;
        uartControl.halfDuplexMode = SOFTUART_HALFDUPLEX_RECEIVE; 
        EXTI->IMR |= uartControl.extiLine; 
    }

    HAL_GPIO_Init(uartSettings.txPort, &uartControl.txLine);
}

/*******************************************************************************
 * softuart_txDmaIsr
 ******************************************************************************/
void softuart_hisr(void) {

    //transmit control    
    //check if a DMA is idling    
    if (!(DMA1_Channel2->CCR & DMA_CCR_EN)) {
        //check if data is available for sending
        uint16_t data;        
        if (softuart_buffer_pop(&softuart_txBufferHandle, &data)) {            
            //prepare buffer
            softuart_prepareTxBuffer(uartControl.txRegister, data); 

            //switch to tx mode if needed
            if (uartControl.halfDuplexMode == SOFTUART_HALFDUPLEX_RECEIVE) softuart_setHalfDuplexLineMode(true); 

            //start transmission after a half bittime        
            uint32_t ccr = TIM1->CNT + uartControl.halfBitTime;
            if (ccr > TIM1->ARR) ccr -= TIM1->ARR;
            TIM1->CCR1 = ccr;        
            
            //Reset DMA counter, set memory address and enable DMA, enable DMA triggers
            DMA1_Channel2->CNDTR = uartControl.txDmaBufSize;
            DMA1_Channel2->CCR |= DMA_CCR_EN;
            TIM1->DIER |= TIM_DMA_CC1;    
        }
        else {
            //switch back to rx mode if needed
            if (uartControl.halfDuplexMode == SOFTUART_HALFDUPLEX_TRANSMIT) softuart_setHalfDuplexLineMode(false); 
        }
    }

    //start receive DMA when exti interrupt happened
    if (uartControl.rxDmaRequestTime != 0xFFFF) {

        //calculate first sampling point in the middle of the startbit    
        uint32_t ccr = uartControl.rxDmaRequestTime + uartControl.halfBitTime;
        if (ccr > TIM1->ARR) ccr -= TIM1->ARR;
        TIM1->CCR3 = ccr; 

        //register rotation
        if (uartControl.pReceiveRegister == uartControl.rxRegister1) uartControl.pReceiveRegister = uartControl.rxRegister2;
        else uartControl.pReceiveRegister = uartControl.rxRegister1;

        //Reset DMA counter, enable DMA and enable DMA trigger
        DMA1_Channel6->CNDTR = uartControl.rxDmaBufSize;
        DMA1_Channel6->CMAR = (uint32_t)uartControl.pReceiveRegister;
        DMA1_Channel6->CCR |= DMA_CCR_EN;
        TIM1->DIER |= TIM_DMA_CC3;

        uartControl.rxDmaRequestTime = 0xFFFF;
    }

    //decode received bytes
    if (uartControl.pDecodeRegister != null) {

        uint16_t rx = 0;
        uint8_t  oneCount = 0;
        uint16_t mask = 1;
        for (uint8_t i=0; i<uartSettings.dataBits; i++) {
            bool line = false;
            if (uartControl.pDecodeRegister[i+1] & uartSettings.rxPin) line = true;
            else if(uartSettings.inverted) line = true; 

            if (line) { rx |= mask; oneCount++; }
            mask = mask << 1;
        }

        //parity control
        bool parityOk = true;  

        if (uartSettings.parity != UART_PARITY_NONE) {
            uint8_t rxParity = 0;
            if (uartControl.pDecodeRegister[uartSettings.dataBits + 1] & uartSettings.rxPin) rxParity = 1;
            else if(uartSettings.inverted) rxParity = 1;

            if ((uartSettings.parity == UART_PARITY_EVEN) &&  ((oneCount + rxParity) & 0x01)) parityOk = false;
            if ((uartSettings.parity == UART_PARITY_ODD ) && !((oneCount + rxParity) & 0x01)) parityOk = false;        
        }

        if (parityOk) softuart_buffer_push(&softuart_rxBufferHandle, rx); 
        else uartControl.parityErrorCount++;


        uartControl.pDecodeRegister = null;
    }
}

/*******************************************************************************
 * softuart_txDmaIsr
 ******************************************************************************/
void softuart_txDmaIsr(void) {

    //reset dma interrupt flag, disable DMA channel, disable DMA requests
    TIM1->DIER &= ~TIM_DMA_CC1;
    DMA1_Channel2->CCR &= ~DMA_CCR_EN;        
    DMA1->IFCR = DMA_FLAG_TC2;    

}

/*******************************************************************************
 * softuart_rxDmaIsr
 ******************************************************************************/
void softuart_rxDmaIsr(void) {

    //reset DMA interrupt flag, disable DMA channel, disable DMA requests   
    TIM1->DIER &= ~TIM_DMA_CC3;   
    DMA1_Channel6->CCR &= ~DMA_CCR_EN;
    DMA1->IFCR = DMA_FLAG_GL6; 

    //set decode register to be decoded in hisr
    if (uartControl.pDecodeRegister != null) uartControl.rxOverRunCounter++; 
    uartControl.pDecodeRegister = uartControl.pReceiveRegister;

    //re-enable exti
    EXTI->PR = uartControl.extiLine;  
    EXTI->IMR |= uartControl.extiLine;         
}

/*******************************************************************************
 * softuart_rxExtiIsr
 ******************************************************************************/
void softuart_rxExtiIsr() {

    //check if the interrupt was triggered by the correct line (for shared interrupt handlers). If not, return from ISR
    if (EXTI->PR & uartControl.extiLine) {
        //disable interrupt
        EXTI->IMR &= ~uartControl.extiLine;  
        EXTI->PR = uartControl.extiLine;  

        //store timer time, hisr will start reception
        uartControl.rxDmaRequestTime = TIM1->CNT;          
    }

}

/*******************************************************************************
 * softuart_init
 ******************************************************************************/
void softuart_init(uartSettings_t settings) {

    //init ringbuffers
    softuart_txBufferHandle = (softuart_rbufHandle_t) { .pBuffer = softuart_txBuffer, .bufsize = SOFTUART_TX_BUFSIZE, .readIndex = 0, .writeIndex = 0 };
    softuart_rxBufferHandle = (softuart_rbufHandle_t) { .pBuffer = softuart_rxBuffer, .bufsize = SOFTUART_RX_BUFSIZE, .readIndex = 0, .writeIndex = 0 };
    
    //copy settings and initalize control
    uartSettings = settings;
    uartControl.parityErrorCount = 0;

    //check if halfduplex mode wil be used
    if ((uartSettings.rxPin == uartSettings.txPin) && (uartSettings.rxPort == uartSettings.txPort)) uartControl.halfDuplexMode = SOFTUART_HALFDUPLEX_RECEIVE;
    else uartControl.halfDuplexMode = SOFTUART_HALFDUPLEX_UNUSED;
    
    //GPIO control constants
    if (uartSettings.inverted) { uartControl.tx_set_0 = settings.txPin; uartControl.tx_set_1 = settings.txPin << 16; }
    else { uartControl.tx_set_0 = settings.txPin << 16; uartControl.tx_set_1 = settings.txPin; }

    //Calculate number of DMA transfers for transmission & reception. In reception, do not receive stop bits
    uartControl.rxDmaBufSize = uartSettings.dataBits + 1;
    if (uartSettings.parity != UART_PARITY_NONE) uartControl.rxDmaBufSize += 1;
    uartControl.txDmaBufSize = uartControl.rxDmaBufSize + uartSettings.stopBits;

    //rxline is input with pullup/down based on polarity. In half duplex, rx line is not used
    if (!uartControl.halfDuplexMode) {
        GPIO_InitTypeDef rxLine;        
        rxLine.Pin = uartSettings.rxPin;
        rxLine.Speed = GPIO_SPEED_FREQ_HIGH;
        rxLine.Mode = GPIO_MODE_INPUT;
        if (uartSettings.inverted) rxLine.Pull = GPIO_PULLDOWN;
        else rxLine.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(uartSettings.rxPort, &rxLine); 
    }

    //txline is output PP. In halfduplex mode, tx line is IPU or IPD depending on inversion (read mode after init)  
    uartControl.txLine.Pin = uartSettings.txPin;    
    if (uartControl.halfDuplexMode) {
        uartControl.txLine.Mode = GPIO_MODE_INPUT;
        if (uartSettings.inverted) uartControl.txLine.Pull = GPIO_PULLDOWN;
        else uartControl.txLine.Pull = GPIO_PULLUP;
    }
    else uartControl.txLine.Mode = GPIO_MODE_OUTPUT_PP;
    uartControl.txLine.Speed = GPIO_SPEED_FREQ_HIGH;  
    HAL_GPIO_Init(uartSettings.txPort, &uartControl.txLine);

    //set tx line to inital state (logic 1)
    uartSettings.txPort->BSRR = uartControl.tx_set_1;

    //Enable required clocks
    __HAL_RCC_TIM1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    //SoftUart is using TIM1 as timer
    //TIM1_CH1 maps to DMA1 channel 2, TIM1_CH2 maps to DMA1 channel 3, TIM1_CH3 maps to DMA1 channel 6, TIM1_CH4 maps to DMA1 channel 4
    //setup TIM3 Time base, ARR equal to 1 bittime
    uint32_t fullBitTime = 72000000 / uartSettings.baudrate;
    uartControl.halfBitTime = fullBitTime >> 1;

    //Timer runs at full 72Mhz, period is set to 1 bittime
    TIM1->PSC = 0;
    TIM1->ARR = fullBitTime - 1;
    TIM1->CCER = TIM_CCER_CC1E | TIM_CCER_CC3E;
    
    //start timer
    TIM1->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN; 

    //Setup DMA's
    //DMA 1 Channel 2 is used for transfers
    DMA1_Channel2->CCR = DMA_CCR_PL_0 | DMA_CCR_MSIZE_1 | DMA_CCR_PSIZE_1 | DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TCIE;
    DMA1_Channel2->CPAR = (uint32_t)&(uartSettings.txPort->BSRR);
    DMA1_Channel2->CMAR = (uint32_t)uartControl.txRegister;

    //DMA 1 Channel 6 is used for reception
    DMA1_Channel6->CCR = DMA_CCR_PL_1 | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_MINC | DMA_CCR_TCIE;
    DMA1_Channel6->CPAR = (uint32_t)&(uartSettings.rxPort->IDR);
    
    //map the rx interrupt line which will be used
    uartControl.extiLine = uartSettings.rxPin;
    if (uartControl.halfDuplexMode) uartControl.extiLine = uartSettings.txPin;

    //map external interrupt ISR source 
    uint8_t extiIRQ = EXTI0_IRQn + uartControl.extiLine - 1;
    if (uartControl.extiLine >= GPIO_PIN_5) extiIRQ = EXTI9_5_IRQn;
    if (uartControl.extiLine >= GPIO_PIN_10) extiIRQ = EXTI15_10_IRQn;

    //enable EXTI interrupt line, edge based on polarity
    if (uartSettings.inverted) EXTI->RTSR |= uartControl.extiLine;
    else EXTI->FTSR |= uartControl.extiLine;
    EXTI->PR |= uartControl.extiLine;
    EXTI->IMR |= uartControl.extiLine;

    //setup priorities and enable interrupts
    HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, IRQ_PRIORITY_TX_DMA, 0);
    HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, IRQ_PRIORITY_RX_DMA, 0);
    HAL_NVIC_SetPriority(extiIRQ, IRQ_PRIORITY_EXTI, 0);

    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    NVIC_EnableIRQ(DMA1_Channel6_IRQn);
    NVIC_EnableIRQ(extiIRQ);

    //variable initialization
    uartControl.rxDmaRequestTime = 0xFFFF;
    uartControl.pDecodeRegister = null;
    uartControl.isInitialized = true;
}

/*******************************************************************************
 * Ringbuffers in 16 bit form
 ******************************************************************************/
/*******************************************************************************
 * softUart_putChar
 ******************************************************************************/
bool softuart_txIdle(void) {
    if (softuart_txBufferHandle.readIndex == softuart_txBufferHandle.writeIndex) return true;
    return false;
}

/*******************************************************************************
 * softUart_putChar
 ******************************************************************************/
void softuart_buffer_flush(softuart_rbufHandle_t* h) {
    h->readIndex = 0;
    h->writeIndex = 0;
}

/******************************************************************************/
/* push               												          */
/******************************************************************************/
bool softuart_buffer_push(softuart_rbufHandle_t* h, uint16_t e) {
    //calculate the next write index
    uint16_t next = h->writeIndex + 1;
    if (next >= h->bufsize) {
        next = 0;
    }

    //check if the buffer is full
    if (next == h->readIndex) {
        return false;
    }

    //write data and change index
    h->pBuffer[h->writeIndex] = e;
    h->writeIndex = next;

    return true;
}

/******************************************************************************/
/* pop                       												  */
/******************************************************************************/
bool softuart_buffer_pop(softuart_rbufHandle_t* h, uint16_t* pE) {
    //check if buffer is empty
    if (h->readIndex == h->writeIndex) {
        return false;
    }
    //copy data to location
    *pE = h->pBuffer[h->readIndex];

    //increment read index
    h->readIndex++;
    if (h->readIndex >= h->bufsize) {
        h->readIndex = 0;
    }
    return true;
}
