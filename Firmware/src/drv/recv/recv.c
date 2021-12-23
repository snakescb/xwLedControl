/***************************************************************************//**
 * @file        recv.c
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 440 $
 *              $LastChangedDate: 2010-05-20 09:59:35 +0200 (Do, 20 Mai 2010) $
 * @brief       Messung der Empfängersignale
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "recv.h"
#include "interrupt.h"
#include "sensor.h"
#include "teco.h"
#include "conf.h"
#include "lowpass.h"

/* Private define ------------------------------------------------------------*/
#define RECV_MAX_ERROR_COUNTER	   	    5
#define RECV_MAX_ERROR_COUNTER_RX2	   10
#define RECV_PPM_ERROR_MAX      	 1250
#define RECV_PPM_ERROR_MIN      	  250

#define RECV_RX2_CENTER_PULSE         759
#define RECV_RX2_GAIN                 210
#define RECV_RX2_LP_LENGTH             32
#define RECV_RX2_LP_SHIFT               5

#define RECV_LINE_2_STATE ((GPIOB->IDR) & GPIO_Pin_11)

/* Private function prototypes -----------------------------------------------*/
void recv_startPPMDecoder(void);
void recv_errorHandler(bool);
void recv2_errorHandler(bool);
u16  recv2_LPBuf[RECV_RX2_LP_LENGTH];
lowpassHandle_t recv2LP = DEFINE_LOWPASS_FILTER(recv2_LPBuf,RECV_RX2_LP_LENGTH,RECV_RX2_LP_SHIFT);

/* Private typedef -----------------------------------------------------------*/
typedef enum {
    PPM_WAIT_PULSE_START = 0,
    PPM_MEASURE_PULSE_LENGTH
} receiverState_t;

/* Private variables ---------------------------------------------------------*/

receiverState_t receiverState;
bool recvFailsafe;
u16  recvRawSignal;
u16  recvErrorCounter;
u16  recvPulsStart;
u16  recvPulsEnde;
u16  r0, r1, r2, r3, r4, r5, r6, r7;

bool rx2OldLineState;
bool recv2Failsafe;
u16  recv2ErrorCounter;
u16  rx2RisingEdge;
s16  rx2Signal;
u16  rx2Raw;

/*******************************************************************************
* Prüft auf Empfängerproblem
*******************************************************************************/
bool recv_failsafe(void) {
    return recvFailsafe;
}

/*******************************************************************************
* Prüft auf Empfängerproblem
*******************************************************************************/
bool recv_failsafe_rx2(void) {
    return recv2Failsafe;
}

/*******************************************************************************
* recv_rx2Signal
*******************************************************************************/
s16 recv_signal_rx2(void) {
    return rx2Signal;
}

/*******************************************************************************
* recv_hisr
*******************************************************************************/
void recv_hisr() {

    bool newLineState = false;
    if(RECV_LINE_2_STATE) newLineState = true;

    if (newLineState ^ rx2OldLineState) {
        rx2OldLineState = newLineState;
        if (newLineState) {
            u16 tmp = TIM4->CNT - rx2RisingEdge;
            if ((tmp > 500) & (tmp < 1200)) {
                recv2_errorHandler(true);
                rx2Raw = lowpass_update(&recv2LP, tmp);
                rx2Signal = ((rx2Raw - RECV_RX2_CENTER_PULSE)*100) / RECV_RX2_GAIN;
            }
            else recv2_errorHandler(false);
        }
        else rx2RisingEdge = TIM4->CNT;
    }
}

/*******************************************************************************
* Gibst das aktuelle unskalierte Empfängersignal zurück
*******************************************************************************/
u16 recv_rawSignal(void) {
    return recvRawSignal;
}

/*******************************************************************************
* Gibst das aktuelle skalierte Empfängersignal zurück
*******************************************************************************/
s16 recv_signal(void) {
    s32 ds = recvRawSignal - boardConfig.receiverCenter;
    s32 dr = 1;

    if (boardConfig.receiverMax > boardConfig.receiverMin) {
        if (ds > 0) dr = boardConfig.receiverMax - boardConfig.receiverCenter;
        else        dr = boardConfig.receiverCenter - boardConfig.receiverMin;
    }
    else {
        if (ds > 0) dr = boardConfig.receiverCenter - boardConfig.receiverMin;
        else        dr = boardConfig.receiverMax - boardConfig.receiverCenter;
    }
    return (s16)((ds*100)/dr);
}


/*******************************************************************************
 * recv_startPPMDecoder
 ******************************************************************************/
void recv_startPPMDecoder(void) {

    //bei ppm wird die länge zwischen zwei steigenden flanken gemessen.
    //Ist die Zeit grösser als 3 ms, wird der kanalzähler zurückgesetzt
	receiverState = PPM_WAIT_PULSE_START;

    //initialisiere timer
	TIM4->SR = 0;
	TIM4->CNT = 0;
	TIM4->ARR = 0xFFFF;

	//capture on rising edge
	TIM4->CCER &= ~(1 << 1);

	TIM4->DIER |= TIM_IT_CC1;
	TIM4->DIER |= TIM_IT_Update;
}


/*******************************************************************************
* interrupt service handler
*******************************************************************************/
void recv_isr(void) {

    u16 irqFlags = (TIM4->SR) & (TIM4->DIER);

    while (irqFlags) {

        /***********************************************************************
		* Flankeninterrupt für Dreahzahlsensor
		***********************************************************************/
        if (irqFlags & TIM_IT_CC3) {
            sensor_callback(TIM4->CCR3);
            TIM4->SR = ~TIM_IT_CC3;
        }

		/***********************************************************************
		* Flankeninterrupt
		***********************************************************************/
        if (irqFlags & TIM_IT_CC1) {
			switch (receiverState) {

				case PPM_WAIT_PULSE_START: {

					recvPulsStart = TIM4->CCR1;
					//next capture on falling edge
					TIM4->CCER |= (1 << 1);
					receiverState = PPM_MEASURE_PULSE_LENGTH;
					break;
				}

				case PPM_MEASURE_PULSE_LENGTH: {
					recvPulsEnde = TIM4->CCR1;
					//next capture on rising edge
					TIM4->CCER &= ~(1 << 1);
					receiverState = PPM_WAIT_PULSE_START;
					//pulslänge berechnen
					u16 pulsLengt = recvPulsEnde - recvPulsStart;

					if ((pulsLengt < RECV_PPM_ERROR_MIN) || (pulsLengt > RECV_PPM_ERROR_MAX)) {
						recv_errorHandler(false);
					}
					else {
						recv_errorHandler(true);

                        r7 = r6;
                        r6 = r5;
                        r5 = r4;
                        r4 = r3;
						r3 = r2;
						r2 = r1;
						r1 = r0;
						r0 = pulsLengt;

						recvRawSignal = (r0 + r1 + r2 + r3 + r4 + r5 + r6 + r7) / 8;
					}
					break;
				}

				default:
					break;
			}

            TIM4->SR = ~TIM_IT_CC1;
        }

        /***********************************************************************
		* Reload Interrupt
		***********************************************************************/
        if (irqFlags & TIM_IT_Update) {
			recv_errorHandler(false);
			recv2_errorHandler(false);
			sensor_timeout();
            TIM4->SR = ~TIM_IT_Update;
        }

        irqFlags = (TIM4->SR) & (TIM4->DIER);
    }
}

/*******************************************************************************
 * recv_init
 ******************************************************************************/
void recv_init(void) {

    // Deinit Tim4 and DMA1 Channel 5 -----------------------------------------
    TIM_DeInit(TIM4);
    DMA_DeInit(DMA1_Channel5);

    // GPIO --------------------------------------------------------------------
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // TIM4 --------------------------------------------------------------------
    // enable clock of TIM4
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    //setup TIM4 zeitbasis, so dass 500kHz timerfrequenz ereicht werden
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    //enable TIM4
    TIM_Cmd(TIM4, ENABLE);

    //TIM4 IRQ -----------------------------------------------------------------
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = (u32) TIM4_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = IRQ_PRIORITY_TIM4;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    //TIM4 CC1 wird zur Flankenerkennung gebraucht -----------------------------
	TIM_ICInitTypeDef   TIM_ICInitStructure;
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
  	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  	TIM_ICInitStructure.TIM_ICFilter = 0x02;
  	TIM_ICInit(TIM4, &TIM_ICInitStructure);

    //clear all flags
    TIM4->SR = 0;

	//init variables
    recvFailsafe = true;
    recv2Failsafe = true;
	recvErrorCounter = 0;
	recv2ErrorCounter = 0;
    recvRawSignal = 0;
    rx2OldLineState = true;
    rx2Signal = 0;

    //start decoding
    recv_startPPMDecoder();
}


/*******************************************************************************
 * recv_errorHandler
 ******************************************************************************/
inline void recv_errorHandler(bool ok) {
 	if (ok) {
        if (recvErrorCounter) recvErrorCounter--;
        else recvFailsafe = false;
	}
	else {
        if (recvErrorCounter < RECV_MAX_ERROR_COUNTER) recvErrorCounter++;
        else recvFailsafe = true;
	}
}
inline void recv2_errorHandler(bool ok) {
 	if (ok) {
        if (recv2ErrorCounter) recv2ErrorCounter--;
        else recv2Failsafe = false;
	}
	else {
        if (recv2ErrorCounter < RECV_MAX_ERROR_COUNTER_RX2) recv2ErrorCounter++;
        else recv2Failsafe = true;
	}
}
