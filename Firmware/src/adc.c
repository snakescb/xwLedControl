/***************************************************************************//**
 * @file        adcd.cpp
 * @author      Created: CLU
 * @brief       Analog to digital converter driver
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "adc.h"
#include "interrupt.h"
#include "lowpass.h"

/* Global variables ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define ADC_LP_LENGTH       64
#define ADC_LP_SHIFT         6
#define ADC_CHANNEL_REFINT  17
#define ADC_CHANNEL_VBAT     6

/* Private variables ---------------------------------------------------------*/
uint16_t adc_battery;
uint16_t adc_LPBuf[ADC_LP_LENGTH];
lowpassHandle_t batteryLP = DEFINE_LOWPASS_FILTER(adc_LPBuf,ADC_LP_LENGTH,ADC_LP_SHIFT);

/*******************************************************************************
 * adc_start
 ******************************************************************************/
void adc_start(void) {
    //berechne batteriespannung in millivolt. Spannungsteiler = 5.7 / 1, Referenzspanung = 3.22Volt
    //adc_battery = (((uint32_t)lowpass_update(&batteryLP, ADC1->DR)) * 18345) >> 12;
    adc_battery = lowpass_update(&batteryLP, ADC1->DR);

    ADC1->CR2 |= ADC_CR2_ADON;
}

/*******************************************************************************
 * Init
 ******************************************************************************/
void adc_init(void) {

    // Variables ---------------------------------------------------------------
    adc_battery = 0;
    lowpass_init(&batteryLP);
    
    // GPIO --------------------------------------------------------------------
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin =  GPIO_PIN_6;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ADC1 Clock --------------------------------------------------------------    
    RCC_PeriphCLKInitTypeDef ADC_CLKInit;    
    ADC_CLKInit.PeriphClockSelection=RCC_PERIPHCLK_ADC;       //ADC peripheral clock
    ADC_CLKInit.AdcClockSelection=RCC_CFGR_ADCPRE_DIV6;       //Frequency division factor 6 clock is 72M/6=12MHz
    HAL_RCCEx_PeriphCLKConfig(&ADC_CLKInit);                  //Set ADC clock    
    __ADC1_CLK_ENABLE();

    // ADC1 configuration ------------------------------------------------------
    ADC1->CR1 = 0;
    ADC1->CR2 = ADC_CR2_TSVREFE | ADC_CR2_ADON;

    // ADC1 calibration --------------------------------------------------------
    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while (ADC1->CR2 & ADC_CR2_RSTCAL);
    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL);

    // Set sample time on channel 0 --------------------------------------------
    ADC1->SMPR1 = 0;
    ADC1->SMPR2 = ADC_SMPR2_SMP0_2;

    // Regular channel length 1, set battery input channel ---------------------
    ADC1->SQR1 = ADC_SQR1_L_0;
    ADC1->SQR2 = 0;
    ADC1->SQR3 = ADC_CHANNEL_REFINT;
}

