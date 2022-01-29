/***************************************************************************//**
 * @file        adcd.cpp
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 920 $
 *              $LastChangedDate: 2010-06-16 12:51:44 +0200 (Mi, 16 Jun 2010) $
 * @brief       Analog to digital converter driver
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "adc.h"
#include "interrupt.h"
#include "lowpass.h"

/* Global variables ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define ADC_LP_LENGTH  128
#define ADC_LP_SHIFT     7

/* Private variables ---------------------------------------------------------*/
u16 adc_battery;
u16 adc_battRaw;
u16 adc_LPBuf[ADC_LP_LENGTH];
lowpassHandle_t batteryLP = DEFINE_LOWPASS_FILTER(adc_LPBuf,ADC_LP_LENGTH,ADC_LP_SHIFT);
u16 adc_counter;

/*******************************************************************************
 * adc_isr
 ******************************************************************************/
void adc_isr(void) {
    adc_battRaw = ADC1->DR;
    //berechne batteriespannung in millivolt. Spannungsteiler = 5.7 / 1, Referenzspanung = 3.3Volt
    u16 vBatt = (((u32)lowpass_update(&batteryLP, adc_battRaw))*19092) >> 12;
    if (adc_counter == ADC_LP_LENGTH) adc_battery = vBatt;
    else adc_counter++;
    ADC1->SR &= ~ADC_FLAG_EOC;
}

/*******************************************************************************
 * adc_start
 ******************************************************************************/
void adc_start(void) {
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*******************************************************************************
 * Init
 ******************************************************************************/
void adc_init(void) {

    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    NVIC_InitTypeDef NVIC_InitStruct;

    // GPIO --------------------------------------------------------------------
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ADC1 Clock --------------------------------------------------------------
    RCC_ADCCLKConfig(RCC_PCLK2_Div4);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    ADC_DeInit(ADC1);

    // ADC1 configuration ------------------------------------------------------
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // Kanal setup -------------------------------------------------------------
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_239Cycles5);

    // Start ADC1 --------------------------------------------------------------
    ADC_Cmd(ADC1, ENABLE);

    // ADC1 Calibration --------------------------------------------------------
    //Enable ADC1 reset calibaration register
    ADC_ResetCalibration(ADC1);
    //Check the end of ADC1 reset calibration register
    while(ADC_GetResetCalibrationStatus(ADC1)){}
    //Start calibration
    ADC_StartCalibration(ADC1);
    //Check the end of ADC1 calibration
    while(ADC_GetCalibrationStatus(ADC1)){}

    //Aktiviere EOC interrupt
    ADC1->SR &= ~ADC_FLAG_EOC;
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

    // Configure ADC interrupts
    NVIC_InitStruct.NVIC_IRQChannel = ADC1_2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = IRQ_PRIORITY_ADC;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    adc_counter = 0;
    adc_battery = 0;
    lowpass_init(&batteryLP);
}

