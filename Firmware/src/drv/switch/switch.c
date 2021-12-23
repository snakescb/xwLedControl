/***************************************************************************//**
 * @file        switch.c
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 440 $
 *              $LastChangedDate: 2010-05-20 09:59:35 +0200 (Do, 20 Mai 2010) $
 * @brief       Treiber für den externen switch
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "switch.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SET_SWITCH  (GPIOB->BSRR = GPIO_Pin_8)
#define CLR_SWITCH  (GPIOB->BRR  = GPIO_Pin_8)

/* Private variables ---------------------------------------------------------*/
/*******************************************************************************
 * switch_enable
 ******************************************************************************/
void switch_enable(bool enable) {
    if (enable) SET_SWITCH;
    else CLR_SWITCH;
}

/*******************************************************************************
 * switch_init
 ******************************************************************************/
void switch_init(void) {

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    CLR_SWITCH;
}
