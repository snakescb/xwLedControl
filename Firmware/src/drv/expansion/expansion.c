/***************************************************************************//**
 * @file        expansion.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 440 $
 *              $LastChangedDate: 2010-05-20 09:59:35 +0200 (Do, 20 Mai 2010) $
 * @brief       Treiber f�r den expansion port
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "expansion.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define JUMPER_1_IS_OFF  ((GPIOB->IDR) & GPIO_Pin_10)
#define JUMPER_0_IS_OFF  ((GPIOB->IDR) & GPIO_Pin_11)

/* Private variables ---------------------------------------------------------*/
u8 expansionJumper = 0;

/*******************************************************************************
 * exPort_update
 ******************************************************************************/
void expansion_update(void) {
    u8 tmp = 3;
    if (JUMPER_0_IS_OFF) tmp -= 1;
    if (JUMPER_1_IS_OFF) tmp -= 2;
    expansionJumper = tmp;
}

/*******************************************************************************
 * statusLed_init
 ******************************************************************************/
void expansion_init(void) {

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

}
