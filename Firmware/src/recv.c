/***************************************************************************//**
 * @file        recv.c
 * @author      Created: CLU
 * @brief       Messung des Empfängersignals
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "recv.h"
#include "interrupt.h"

/* Private define ------------------------------------------------------------*/
#define RECV_MAX_ERROR_COUNTER	   	    5
#define RECV_PULSE_MAX_us  	         2200
#define RECV_PULSE_MIN_us  	          800

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
bool recvFailsafe;
uint16_t  recvRawSignal;
uint16_t  recvErrorCounter;
uint16_t  recvPulseStart;

/*******************************************************************************
* recv_failsafe
*******************************************************************************/
bool recv_failsafe(void) {
    return recvFailsafe;
}

/*******************************************************************************
* recv_rawSignal
*******************************************************************************/
uint16_t recv_rawSignal(void) {
    return recvRawSignal;
}

/*******************************************************************************
* recv_signal
*******************************************************************************/
int16_t recv_signal(void) {
    //1000us = -100%, 2000us = +100%, 1500us = 0%
    return (recvRawSignal-1500)/5;
}

/*******************************************************************************
* interrupt service handler
*******************************************************************************/
void recv_isr(void) {

    //capture interrupt
    if (TIM4->SR & TIM_SR_CC1IF) {
        uint16_t currentPulseTime = TIM4->CCR1;     
        //falling edge   
        if (TIM4->CCER & TIM_CCER_CC1P) {
            if (currentPulseTime < recvPulseStart) currentPulseTime += 0xFFFF;
            uint16_t pulseLength = currentPulseTime - recvPulseStart;
            if ((pulseLength < RECV_PULSE_MAX_us) && (pulseLength > RECV_PULSE_MIN_us)) {
                recvRawSignal = pulseLength;
                recvErrorCounter = 0;
            }
            TIM4->CCER &= ~TIM_CCER_CC1P;
        }
        //rising edge
        else {
            recvPulseStart = currentPulseTime;
            TIM4->CCER |= TIM_CCER_CC1P;
        }
        TIM4->SR &= ~TIM_SR_CC1IF;
    }

    //update interrupt
    if (TIM4->SR & TIM_SR_UIF) {
        recvErrorCounter++;
        if (recvErrorCounter >= RECV_MAX_ERROR_COUNTER) {
            recvErrorCounter = RECV_MAX_ERROR_COUNTER;
            recvFailsafe = true;
        }
        else recvFailsafe = false;
        TIM4->SR &= ~TIM_SR_UIF;
    }
}

/*******************************************************************************
 * recv_init
 ******************************************************************************/
void recv_init(void) {

    //receiver 1 is connected on PB6, TIM4 Channel 1

    // TIM CLock ---------------------------------------------------------------
    __TIM4_CLK_ENABLE();

    // GPIO --------------------------------------------------------------------
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 

    // TIM4 Registers ----------------------------------------------------------
    TIM4->PSC = 71; //TIM Clock is 72MHz, divided by (71+1) gives 1MHz clock
    TIM4->ARR = 0xFFFF;
    TIM4->CCMR1 = TIM_CCMR1_CC1S_0 | TIM_CCMR1_IC1F_3;
    TIM4->CCER = TIM_CCER_CC1E;
    TIM4->CR1 = TIM_CR1_CEN;

    // Interrupt ---------------------------------------------------------------
    HAL_NVIC_SetPriority(TIM4_IRQn, IRQ_PRIORITY_TIM4, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
    TIM4->DIER = TIM_DIER_CC1IE | TIM_DIER_UIE;

    // Variables ---------------------------------------------------------------
    recvFailsafe = true;
}
