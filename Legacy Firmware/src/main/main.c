/***************************************************************************//**
 * @file        main.cpp
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 980 $
 *              $LastChangedDate: 2010-06-17 17:31:00 +0200 (Do, 17 Jun 2010) $
 * @brief       Project Mainfile
 ******************************************************************************/

#include "common.h"
#include "uart1.h"
#include "teco.h"
#include "conf.h"
#include "statusLed.h"
#include "ledControl.h"
#include "recv.h"
#include "simpleString.h"
#include "adc.h"
#include "hwVersion.h"
#include "sensor.h"
#include "skyBus.h"

/*******************************************************************************
 * Globale variabeln
 ******************************************************************************/

/***************************************************************************//**
 * @brief  GPIO init
 *
 * is called during system initialization. This function enables the clock of
 * all GPIO peripherials and enables all pins as input floating. The hardware
 * drivers are called afterwards and redefine the pins if needed.
 ******************************************************************************/
void gpio_init(void);

/***************************************************************************//**
 * @brief  Program Main.
 *
 * This function is called after a reset condition.
 ******************************************************************************/
int main(void) {

    /***************************************************************************
    * SYSTEM KONFIGURIEREN
    ***************************************************************************/
    //Initialize the system clocks
    SystemInit();
    //Setze Vektortabelle
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00);
    //use priority group 4, using 4 bits for preemption priority
    //make sure not to change this in the code
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    //Setup SysTick Timer for 0.1 msec interrupts
    SysTick_Config(SystemFrequency / 1000);
    //initialize gpios
    gpio_init();

    /***************************************************************************
    * SOFTWARE MODULE INITIALISIEREN
    ***************************************************************************/
    uart1_init();
    teco_init();
    config_init();

    TRACE("XLed Control");

    if(!config_boardConfigRead()) {
        TRACE("Boardconfig Error!!");
        config_boardConfigFactoryDefault();
    }
    else TRACE("Boardconfig OK");

    extern u8 crystal_used;
    if (crystal_used) TRACE("Using external crystal");

    hwVersion_init();
    adc_init();
    statusLed_init();
    ledControl_init();

    /***************************************************************************
    * HAUPTPROGRAMM
    ***************************************************************************/

    while(1) {
        teco_hisr();
        sensor_hisr();
        skyBus_hisr();
        recv_hisr();

        static u32 tmp1 = 0;
        tmp1++;
        if (tmp1 >= 100000) {
            tmp1 = 0;
            //extern u16 triggerDebug;
            //extern u32 ir_shutterCount;
            //TRACE(intToString(crystal_used, false));
            //TRACE(intToString(recvTest, false));
            //TRACE(intToString(recv_rawSignal(), false));
            //TRACE(intToString(recv_signal(), false));
            //TRACE(intToString(sensor_getRPM(), false));
            //extern u32 variableTimeIncrement;
            //TRACE(intToString(variableTimeIncrement, false));
            //TRACE(intToString(recv_rawSignal(), false));
            extern u16 rx2Raw;
            TRACE(intToString(rx2Raw, false));
        }

    }
}

/******************************************************************************
* implementation of gpio init (forward declared previously)
******************************************************************************/
void gpio_init(void)  {
    // GPIO init structure for port setting
    GPIO_InitTypeDef GPIO_InitStruct;

    // Enable Clocks of all GPIO modules (all on APB2)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    //all pins as inputs
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_Init(GPIOD, &GPIO_InitStruct);
    GPIO_Init(GPIOE, &GPIO_InitStruct);
}
