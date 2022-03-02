/***************************************************************************//**
 * @file        statusLed.h
 * @author      Created: CLU
 * @brief       Treiber für die Statusled
 ******************************************************************************/
#include "hwVersion.h"
#include "xwCom.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define HW_IDENT_1_IS_HIGH  (GPIOA->IDR & GPIO_PIN_11)
#define HW_IDENT_2_IS_HIGH  (GPIOA->IDR & GPIO_PIN_12)

/* Private variables ---------------------------------------------------------*/

/*******************************************************************************
 * hwVersion_read
 ******************************************************************************/
uint8_t hwVersion_read(void) {

    if (  HW_IDENT_1_IS_HIGH  &&  HW_IDENT_2_IS_HIGH ) return HW_VERSION_LED_CONTROL_V1;
    if ( !HW_IDENT_1_IS_HIGH  &&  HW_IDENT_2_IS_HIGH ) return HW_VERSION_LED_CONTROL_V2;
    return HW_VERSION_UNKNOWN;

}

/*******************************************************************************
 * hwVersion_init
 ******************************************************************************/
void hwVersion_init(void) {

    //Thank you HAL for NOTHING -> HAL did not set the pullup config, doing it manually
    GPIOA->CRH &= ~(GPIO_CRH_CNF11_0 | GPIO_CRH_CNF12_0);
    GPIOA->CRH |=   GPIO_CRH_CNF11_1 | GPIO_CRH_CNF12_1;
    GPIOA->BSRR = GPIO_PIN_11 | GPIO_PIN_12;

}
