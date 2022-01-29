/***************************************************************************//**
 * @file        statusLed.h
 * @author      Created: CLU
 * @brief       Treiber für die Statusled
 ******************************************************************************/
#include "statusLed.h"
#include "common.h"
//#include "sensor.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CLR_STATUS  (GPIOA->BRR  = GPIO_PIN_4)
#define SET_STATUS  (GPIOA->BSRR = GPIO_PIN_4)

/* Private variables ---------------------------------------------------------*/
eStausLedState status_currentState;
eStausLedMode  status_mode;
bool status_substate;
uint16_t status_counter, status_on, status_off;

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

    if (status_mode == STATUS_LED_MODE_SENSORTEST) {
        //if (sensor_active()) {SET_STATUS;}
        //else {CLR_STATUS;}
        return;
    }

    if (status_on  == 0xFFFF) return;
    if (status_off == 0xFFFF) return;

    status_counter++;
    if (!status_substate) {
        if (status_counter >= status_on) {
            CLR_STATUS;
            status_substate = true;
            status_counter = 0;
        }
    }
    else {
         if (status_counter >= status_off) {
            SET_STATUS;
            status_substate = false;
            status_counter = 0;
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

        if (status_mode == STATUS_LED_MODE_NORMAL) {
            if (status_on > 0) { SET_STATUS; }
            else { CLR_STATUS; }
        }
    }
}

/**************************************************************************//**
 * @brief    Setzt den modus
 *****************************************************************************/
void statusLed_setMode(eStausLedMode mode) {
    status_mode = mode;
    if (mode == STATUS_LED_MODE_NORMAL) {
        eStausLedState s = status_currentState;
        status_currentState = STATUS_LED_OFF;
        CLR_STATUS;
        statusLed_setState(s);
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

    status_mode = STATUS_LED_MODE_NORMAL;
    status_currentState = STATUS_LED_OFF;
    status_substate = false;

    SET_STATUS;
}
