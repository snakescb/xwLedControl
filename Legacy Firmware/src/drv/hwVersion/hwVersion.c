/***************************************************************************//**
 * @file        statusLed.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 440 $
 *              $LastChangedDate: 2010-05-20 09:59:35 +0200 (Do, 20 Mai 2010) $
 * @brief       Treiber f�r die Statusled
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hwVersion.h"
#include "xwCom.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define HW_IDENT_1_IS_HIGH  ((GPIOA->IDR) & GPIO_Pin_13)
#define HW_IDENT_2_IS_HIGH  ((GPIOA->IDR) & GPIO_Pin_12)
#define HW_IDENT_3_IS_HIGH  ((GPIOA->IDR) & GPIO_Pin_11)

/* Private variables ---------------------------------------------------------*/

/*******************************************************************************
 * hwVersion_read
 ******************************************************************************/
u8 hwVersion_read(void) {


    return HW_VERSION_XWLECONTROL_V1;

    //if ( HW_IDENT_1_IS_HIGH &&  HW_IDENT_2_IS_HIGH) return HW_VERSION_LED_CONTROL_V1;
    //if (!HW_IDENT_1_IS_HIGH &&  HW_IDENT_2_IS_HIGH) return HW_VERSION_LED_CONTROL_V2;
    //if ( HW_IDENT_1_IS_HIGH && !HW_IDENT_2_IS_HIGH) return HW_VERSION_LED_CONTROL_V3;

    //return HW_VERSION_UNKNOWN;
}

/*******************************************************************************
 * hwVersion_init
 ******************************************************************************/
void hwVersion_init(void) {

    // GPIO init structure for port setting
    GPIO_InitTypeDef GPIO_InitStruct;

    //PA13 und PA12 sind f�r HW indentifikation reserviert. Aktiviere Pullups
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

}
