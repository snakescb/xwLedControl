/***************************************************************************//**
 * @file        main.cpp
 * @author      Created: CLU
 * @brief       Project Mainfile
 ******************************************************************************/
#include "common.h"
#include "statusLed.h"
#include "hwVersion.h"
#include "uart1.h"

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
    uart1_init();
    //teco_init();
    //config_init();

    //TRACE("XLed Control");

    //if(!config_boardConfigRead()) {
    //    TRACE("Boardconfig Error!!");
    //    config_boardConfigFactoryDefault();
    //}
    //else TRACE("Boardconfig OK");

    //adc_init();
    statusLed_init();
    hwVersion_init();
    //ledControl_init();
    statusLed_setState(STATUS_LED_ON);

    while (1) {
        //uart1_putChar('A');
        //HAL_Delay(1000);
        uint8_t rx;
        //if (uart1_getChar(&rx)) uart1_putChar(rx);
        HAL_Delay(1000);
        uart1_putChar('C');

        /*
        uint8_t d;
        if (uart1_getChar(&d)) {
            uart1_putChar(d);
        }
        */
    }

}

/******************************************************************************
* implementation of gpio init (forward declared previously)
******************************************************************************/
void gpio_init(void)  {
    // GPIO init structure for port setting
    GPIO_InitTypeDef GPIO_InitStruct;

    // Enable Clocks of all GPIO modules (all on APB2)
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    //all pins as inputs
    GPIO_InitStruct.Pin = GPIO_PIN_All;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
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
