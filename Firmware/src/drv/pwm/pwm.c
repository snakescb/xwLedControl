/***************************************************************************//**
 * @file        pwm.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 440 $
 *              $LastChangedDate: 2010-05-20 09:59:35 +0200 (Do, 20 Mai 2010) $
 * @brief       PWM Treiber für LED Ausgänge
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "pwm.h"
#include "interrupt.h"

/* Private typedef -----------------------------------------------------------*/
#define PWM_TIM_DIVIDER  3
#define PWM_TIM_PERIOD   10000

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const u16 pwmTable[256] = {
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
//ist der dutycycle >=PWM_TIM_PERIOD ist der ausgang konstant auf 1. Ist der Wert 0, bleibt
//der Ausgang konstant auf 0.
const u16* pwmValue[6] = {
    (u16*)&(TIM3->CCR4),
    (u16*)&(TIM3->CCR3),
    (u16*)&(TIM3->CCR2),
    (u16*)&(TIM2->CCR4),
    (u16*)&(TIM2->CCR3),
    (u16*)&(TIM2->CCR2)
};

/*******************************************************************************
 * pwm_init
 * Für die 6 LED Ports wird TIM2 und TIM3 benötigt, Die Timer laufen mit 36MHZ.
 * Der Teiler wird auf 141 eingsetellt, dann laufen die Zähler mit ca 255kHz, Die
 * Periode wird auf 256 eingestellt, somit wird die PWM Frequenz ca 1kHz,
 ******************************************************************************/
void pwm_init(void) {

    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    //aktiviere clocks zu den timer modulen
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    //definiere pins für die pwm ausgänge
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    //Basistimer einstellen, gleich für TIM2 und TIM3
    TIM_TimeBaseStructure.TIM_Period = PWM_TIM_PERIOD - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = PWM_TIM_DIVIDER - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    //ausgangsmodus für die ensprechenden captur/compare units
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);

    //TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    //TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    //TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
    //TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    //TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    //TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

    //TIM_ARRPreloadConfig(TIM2, ENABLE);
    //TIM_ARRPreloadConfig(TIM3, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}
