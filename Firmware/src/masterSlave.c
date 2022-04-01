/***************************************************************************//**
 * @file        masterSlave.c
 * @author      Created: CLU
 * @brief       Master / Slave Verbindung
 ******************************************************************************/
#include "masterslave.h"
#include "softUart.h"
#include "com.h"
#include "teco.h"
#include "ledControl.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

comFrame_t masterslave_com;

/*******************************************************************************
 * mastertSlave_hisr
 ******************************************************************************/
void mastertSlave_hisr(void) {

    uint16_t rxData;
    if (softuart_getChar(&rxData)) {     
        if (com_update(&masterslave_com, (uint8_t)rxData)) {
            ledControl_masterSlaveRx(masterslave_com.rxCount, masterslave_com.data);
        }
    }

}

/*******************************************************************************
 * mastertSlave_sendMasterData
 ******************************************************************************/
void masterSlave_sendMasterData(uint8_t numBytes, uint8_t* data) {

    //skip data if uart still is busy
    if (!softuart_txIdle()) return;

    //data packaging
    uint8_t txData[64];
    uint16_t txBytes = com_prepareBuffer(0, numBytes, data, txData);
    for (uint16_t i=0; i<txBytes; i++) softuart_putChar(txData[i]);

}


/*******************************************************************************
 * masterslave_init
 ******************************************************************************/
void masterSlave_init(void) {

    // Enable clock for alternate functions
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN);
    
    // Disable both SWD and JTAG to free PA13, PA14, PA15, PB3 and PB4
    MODIFY_REG(AFIO->MAPR, AFIO_MAPR_SWJ_CFG, AFIO_MAPR_SWJ_CFG_DISABLE); 

    // Initialize soft uart
    uartSettings_t uart;
    uart.baudrate = 38400;
    uart.dataBits = SOFT_UART_DATA_BITS_8;
    uart.stopBits = SOFT_UART_STOPP_BITS_2;
    uart.parity = SOFT_UART_PARITY_NONE;
    uart.inverted = false;
    uart.txPort = GPIOA;
    uart.rxPort = GPIOA;
    uart.txPin = GPIO_PIN_14;
    uart.rxPin = GPIO_PIN_13;

    softuart_init(uart);
    com_handleInit(&masterslave_com);
}
