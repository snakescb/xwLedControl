/***************************************************************************//**
 * @file        statusLed.h
 * @author      Created: CLU
 * @brief       Treiber für die Statusled
 ******************************************************************************/
#include "statusLed.h"
#include "common.h"
#include "hwVersion.h"
#include "teco.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CLR_STATUS  (GPIOA->BRR  = GPIO_PIN_4)
#define SET_STATUS  (GPIOA->BSRR = GPIO_PIN_4)
#define STATUS_BRIGTHNESS_MAX      10
#define STATUS_BRIGTHNESS           1 //max for non-pro boards
#define STATUS_BRIGTHNESS_PRO       5 //max for PRO boards

/* Private variables ---------------------------------------------------------*/
eStausLedState status_currentState;
bool status_substate, status_enableLed;
uint16_t status_counter, status_on, status_off;
uint16_t status_brightness_counter;
uint8_t status_maxBrightness;

const uint16_t statusLedDef[] = {
    0x0000, 0xFFFF, //0
    0xFFFF, 0x0000, //1
      1800,    200, //2
       500,    500, //3
       200,   1800  //4
};

/*******************************************************************************
 * statusLed_update
 ******************************************************************************/
void statusLed_update(void) {

    //led brigthness reduction
    if (status_enableLed) {        
        if (status_brightness_counter < status_maxBrightness) SET_STATUS;
        else CLR_STATUS;
        status_brightness_counter++;
        if (status_brightness_counter >= STATUS_BRIGTHNESS_MAX) status_brightness_counter = 0;
    }
    else CLR_STATUS;


    //led blink control
    if (status_on  == 0xFFFF) return;
    if (status_off == 0xFFFF) return;

    status_counter++;
    if (!status_substate) {
        if (status_counter >= status_on) {
            status_substate = true;
            status_counter = 0;
            status_enableLed = false;
        }
    }
    else {
         if (status_counter >= status_off) {            
            status_substate = false;
            status_counter = 0;
            status_enableLed = true;
        }
    }

}

/*******************************************************************************
 * statusLed_setState
 ******************************************************************************/
void statusLed_setState(eStausLedState state) {
    if ((state != status_currentState) && (state < STATUS_NUM_STATES)) {
        status_currentState = state;
        status_counter = 0;
        status_substate = false;

        status_on  = statusLedDef[2*state];
        status_off = statusLedDef[2*state + 1];

        if (state == STATUS_LED_ON)  status_enableLed = true;
        if (state == STATUS_LED_OFF) status_enableLed = false;
    }
}

/*******************************************************************************
 * statusLed_init
 ******************************************************************************/
void statusLed_init(void) {

    //definiere pins für die status led
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 

    status_currentState = STATUS_LED_OFF;
    status_substate = false;
    status_off = 0xFFFF;
    status_on  = 0x0000;
    status_enableLed = false;
    CLR_STATUS;    

    if (hwType == HW_TYPE_XWLEDCONTROL_PRO) status_maxBrightness = STATUS_BRIGTHNESS_PRO;
    else status_maxBrightness = STATUS_BRIGTHNESS;
}
