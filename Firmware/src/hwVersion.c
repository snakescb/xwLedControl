/***************************************************************************//**
 * @file        statusLed.h
 * @author      Created: CLU
 * @brief       Treiber für die Statusled
 ******************************************************************************/
#include "hwVersion.h"
#include "xwCom.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define HW_IDENT_1_IS_HIGH  ((GPIOA->IDR) & GPIO_PIN_11)
#define HW_IDENT_2_IS_HIGH  ((GPIOA->IDR) & GPIO_PIN_12)

/* Private variables ---------------------------------------------------------*/

/*******************************************************************************
 * hwVersion_read
 ******************************************************************************/
uint8_t hwVersion_read(void) {

    if ( HW_IDENT_1_IS_HIGH && HW_IDENT_2_IS_HIGH ) return HW_VERSION_LED_CONTROL_V1;
    return HW_VERSION_UNKNOWN;

}

/*******************************************************************************
 * hwVersion_init
 ******************************************************************************/
void hwVersion_init(void) {

    //PA11 und PA12 sind für HW indentifikation reserviert. Aktiviere Pullups
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 

}
