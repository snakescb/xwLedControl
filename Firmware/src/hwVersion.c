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
#define HW_TYPE_IS_HIGH     (GPIOB->IDR & GPIO_PIN_3)

/* Private variables ---------------------------------------------------------*/
uint8_t hwType, hwVersion;

/*******************************************************************************
 * hwVersion_read
 ******************************************************************************/
uint8_t hwVersion_read(void) {

    hwType = HW_TYPE_XWLEDCONTROL;
    if ( !HW_TYPE_IS_HIGH ) hwType = HW_TYPE_XWLEDCONTROL_PRO;

    hwVersion = HW_VERSION_V1;
    if ( !HW_IDENT_1_IS_HIGH  &&   HW_IDENT_2_IS_HIGH ) hwVersion =  HW_VERSION_V2;
    if (  HW_IDENT_1_IS_HIGH  &&  !HW_IDENT_2_IS_HIGH ) hwVersion =  HW_VERSION_V3;
    if ( !HW_IDENT_1_IS_HIGH  &&  !HW_IDENT_2_IS_HIGH ) hwVersion =  HW_VERSION_V4;

    return (hwType << 4) + hwVersion;
}

/*******************************************************************************
 * hwVersion_init
 ******************************************************************************/
void hwVersion_init(void) {

    //Thank you HAL for NOTHING -> HAL did not set the pullup config, doing it manually
    GPIOA->CRH &= ~(GPIO_CRH_CNF11_0 | GPIO_CRH_CNF12_0);
    GPIOA->CRH |=   GPIO_CRH_CNF11_1 | GPIO_CRH_CNF12_1;
    GPIOA->BSRR = GPIO_PIN_11 | GPIO_PIN_12;

    GPIOB->CRL &= ~(GPIO_CRL_CNF3_0);
    GPIOB->CRL |=   GPIO_CRL_CNF3_1;
    GPIOB->BSRR = GPIO_PIN_3;

    hwVersion_read();
}
