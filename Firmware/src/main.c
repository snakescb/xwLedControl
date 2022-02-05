/***************************************************************************//**
 * @file        main.cpp
 * @author      Created: CLU
 * @brief       Project Mainfile
 ******************************************************************************/
#include "common.h"
#include "statusLed.h"
#include "hwVersion.h"
#include "uart1.h"
#include "teco.h"
#include "conf.h"
#include "adc.h"
#include "recv.h"
#include "ledControl.h"
#include <stdio.h>

/*******************************************************************************
 * Globale variabeln
 ******************************************************************************/

/***************************************************************************//**
 * Forward delcarations
 ******************************************************************************/
void gpio_init(void);
void SystemClock_Config(void);

/***************************************************************************//**
 * @brief  Program Main.
 *
 * This function is called after a reset condition.
 ******************************************************************************/
int main(void)  {
  
    /***************************************************************************
    * SYSTEM KONFIGURIEREN
    ***************************************************************************/
    HAL_Init();
    //Configure the system clock to 72 MHz
    SystemClock_Config();
    //initialize gpios
    gpio_init();

    /***************************************************************************
    * SOFTWARE MODULE INITIALISIEREN
    ***************************************************************************/
    teco_init();
    config_init();
    statusLed_init();

    TRACE("xwLedControl restarted");

    if(!config_boardConfigRead()) {
        TRACE("Boardconfig Error!!");
        config_boardConfigFactoryDefault();
        statusLed_setState(STATUS_LED_BLINK_3);
    }
    else {
        TRACE("Boardconfig OK");
        statusLed_setState(STATUS_LED_BLINK_2);
    }

    hwVersion_init();
    adc_init();    
    recv_init();
    ledControl_init();

    while (1) {        
        teco_hisr();

        static uint32_t cnt1 = 0;
        static uint32_t cnt2 = 0;
        cnt1++;
        if (cnt1 > 20000) {
            cnt1 = 0;
            cnt2++;
            //char buffer[128];
            //sprintf(buffer, "Receiver: %d", jumper_read());
            //TRACE(buffer);
        }
    }
}

/******************************************************************************
* implementation of gpio init (forward declared previously)
******************************************************************************/
void gpio_init(void)  {

    // Enable Clocks of all GPIO modules (all on APB2)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();

}

/******************************************************************************
* SystemClock_Config (forward declared previously)
******************************************************************************/
void SystemClock_Config(void) {

    RCC_ClkInitTypeDef clkinitstruct = {0};
    RCC_OscInitTypeDef oscinitstruct = {0};
  
    /* Enable HSE Oscillator and activate PLL with HSE as source */
    oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
    oscinitstruct.HSEState        = RCC_HSE_ON;
    oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
    oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
    oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
    oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK) {
        /* Initialization Error */
        while(1);
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
    clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
    clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
    if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK) {
        /* Initialization Error */
        while(1);
    }
}
