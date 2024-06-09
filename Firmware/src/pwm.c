/***************************************************************************//**
 * @file        pwm.h
 * @author      Created: CLU
 * @brief       PWM Treiber für LED Ausgänge
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "pwm.h"
#include "hwVersion.h"
#include "xwCom.h"

/* Private typedef -----------------------------------------------------------*/
#define PWM_TIM_DIVIDER      3
#define PWM_TIM_PERIOD   10000

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const uint16_t pwmTable[256] = {
    0, 2, 4, 7, 9, 11, 14, 16, 19, 21, 24, 26, 29, 32, 34, 37, 40,
    43, 46, 49, 52, 55, 58, 61, 65, 68, 71, 75, 78, 82, 85, 89, 93,
    97, 100, 104, 108, 112, 117, 121, 125, 129, 134, 138, 143, 148, 152, 157, 162,
    167, 172, 177, 183, 188, 194, 199, 205, 211, 216, 222, 228, 235, 241, 247, 254,
    260, 267, 274, 281, 288, 295, 302, 310, 317, 325, 333, 341, 349, 357, 365, 374,
    383, 391, 400, 410, 419, 428, 438, 448, 458, 468, 478, 489, 499, 510, 521, 532,
    544, 555, 567, 579, 591, 604, 617, 629, 643, 656, 669, 683, 697, 712, 726, 741,
    756, 771, 787, 803, 819, 835, 852, 869, 886, 903, 921, 940, 958, 977, 996, 1015,
    1035, 1055, 1076, 1097, 1118, 1139, 1161, 1184, 1206, 1229, 1253, 1277, 1301, 1326, 1351, 1377,
    1403, 1429, 1456, 1484, 1512, 1540, 1569, 1598, 1628, 1659, 1690, 1721, 1753, 1786, 1819, 1853,
    1887, 1922, 1957, 1993, 2030, 2067, 2106, 2144, 2184, 2224, 2264, 2306, 2348, 2391, 2434, 2479,
    2524, 2570, 2617, 2664, 2713, 2762, 2812, 2863, 2915, 2967, 3021, 3076, 3131, 3187, 3245, 3303,
    3363, 3423, 3485, 3547, 3611, 3676, 3742, 3809, 3877, 3946, 4017, 4089, 4162, 4236, 4312, 4389,
    4467, 4547, 4628, 4710, 4794, 4879, 4966, 5054, 5144, 5235, 5328, 5423, 5519, 5617, 5717, 5818,
    5921, 6026, 6132, 6241, 6351, 6463, 6578, 6694, 6812, 6932, 7055, 7179, 7306, 7435, 7566, 7699,
    7835, 7973, 8113, 8256, 8401, 8549, 8699, 8852, 9008, 9166, 9327, 9491, 9658, 9827, 10000
};

//definiert die pointer um numeriert auf den PWM Dutycycle eines Ausgangs zugreifen zu können
//ist der dutycycle >= PWM_TIM_PERIOD ist der ausgang konstant auf 1. Ist der Wert 0, bleibt
//der Ausgang konstant auf 0.
const uint16_t* pwmValue[6] = {
    (uint16_t*)&(TIM3->CCR4),
    (uint16_t*)&(TIM3->CCR3),
    (uint16_t*)&(TIM3->CCR2),
    (uint16_t*)&(TIM2->CCR4),
    (uint16_t*)&(TIM2->CCR3),
    (uint16_t*)&(TIM2->CCR2)
};

/*******************************************************************************
 * pwm_init
 * Für die 6 LED Ports wird TIM2 und TIM3 benötigt, Die Timer laufen mit 36MHZ.
 ******************************************************************************/
void pwm_init(void) {

    // Periphery clocks --------------------------------------------------------
    __TIM2_CLK_ENABLE();
    __TIM3_CLK_ENABLE();

    // GPIO --------------------------------------------------------------------
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Timer --------------------------------------------------------------------
    //Divider = 3 -> TIM clock = 12 Mhz
    //Period = 10000 -> PWM frequency = 1.2 kHz
    TIM2->ARR = PWM_TIM_PERIOD - 1;
    TIM3->ARR = PWM_TIM_PERIOD - 1;
    TIM2->PSC = PWM_TIM_DIVIDER - 1;
    TIM3->PSC = PWM_TIM_DIVIDER - 1;

    // Edge aligned PWM Mode 1 on CH2,3 and 4 of both timers
    TIM2->CCMR1 = TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
    TIM2->CCMR2 = TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC4PE | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1;
    TIM3->CCMR1 = TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
    TIM3->CCMR2 = TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC4PE | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1;

    //for PRO version, output polarity is changed
    if (hwType == HW_TYPE_XWLEDCONTROL_PRO) {
        TIM2->CCER = TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E | TIM_CCER_CC2P | TIM_CCER_CC3P | TIM_CCER_CC4P;
        TIM3->CCER = TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E | TIM_CCER_CC2P | TIM_CCER_CC3P | TIM_CCER_CC4P;
    }
    else {
        TIM2->CCER = TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
        TIM3->CCER = TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
    }    

    //update registers
    TIM2->EGR |= TIM_EGR_UG;
    TIM3->EGR |= TIM_EGR_UG;

    //start timers
    TIM2->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;
    TIM3->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;

}
