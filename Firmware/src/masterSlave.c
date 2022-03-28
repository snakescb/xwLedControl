/***************************************************************************//**
 * @file        masterSlave.c
 * @author      Created: CLU
 * @brief       Master / Slave Verbindung
 ******************************************************************************/
#include "masterslave.h"
#include "softUart.h"
#include "common.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/*******************************************************************************
 * mastertSlave_update
 ******************************************************************************/
void mastertSlave_update(void) {

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
    softUart_init();

    /*
    // PA13 und 14 als output
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 

    GPIOA->BSRR = GPIO_PIN_13;
    GPIOA->BRR = GPIO_PIN_14;
    */
}
