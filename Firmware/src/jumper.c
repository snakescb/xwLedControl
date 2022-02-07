/***************************************************************************//**
 * @file        jumper.h
 * @author      Created: CLU
 * @brief       jumper
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "jumper.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define JUMPER_0_IS_HIGH  (GPIOB->IDR & GPIO_PIN_10)
#define JUMPER_1_IS_HIGH  (GPIOB->IDR & GPIO_PIN_11)

/* Private variables ---------------------------------------------------------*/

/*******************************************************************************
 * jumper_update
 ******************************************************************************/
uint8_t jumper_read(void) {

    uint8_t tmp = 3;
    if (JUMPER_0_IS_HIGH) tmp -= 1;
    if (JUMPER_1_IS_HIGH) tmp -= 2;
    return tmp;
    
}

/*******************************************************************************
 * jumper_init
 ******************************************************************************/
void jumper_init(void) {
 
    //Thank you HAL for NOTHING -> HAL did not set the pullup config, doing it manually
    GPIOB->CRH &= ~(GPIO_CRH_CNF10_0 | GPIO_CRH_CNF11_0);
    GPIOB->CRH |=   GPIO_CRH_CNF10_1 | GPIO_CRH_CNF11_1;
    GPIOB->BSRR = GPIO_PIN_10 | GPIO_PIN_11;

}
