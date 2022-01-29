/***************************************************************************//**
 * @file        dreamExtension.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 712 $
 *              $LastChangedDate: 2010-06-01 17:10:29 +0200 (Di, 01 Jun 2010) $
 *
 * Protokoll für dreamExtensions
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "dreamExtension.h"
#include "skyBus.h"

/* Defines --------------------------------------------------------------------*/
#define DREAMEXTENSION_SKYBUS_PORT  0xA2
#define DREAMEXTENSION_BUFFER_LEN     32
#define DREAMEXTENSION_NUM_OPTIONS     5

#define DREAMEXTENSION_COMMAND_START_EFFECT     0
#define DREAMEXTENSION_COMMAND_STOP_IMMEDIATE   1
#define DREAMEXTENSION_COMMAND_STOP             2

/* internal types -------------------------------------------------------------*/
/* Function prototypes --------------------------------------------------------*/
/* Variables ------------------------------------------------------------------*/
u8 skyBusBuffer[DREAMEXTENSION_BUFFER_LEN];

/*******************************************************************************
 * dreamEffect_Start
 ******************************************************************************/
void dreamEffect_Start(u8 boardAddress, u16 numPixels, u16 runTime, u16 fadeTime, u8 brightness, u8 effectNumber, u8* pOptionBytes) {

    skyBusBuffer[0] = boardAddress;
    skyBusBuffer[1] = DREAMEXTENSION_COMMAND_START_EFFECT;
    skyBusBuffer[2] = (u8)(numPixels >> 8);
    skyBusBuffer[3] = (u8)(numPixels);
    skyBusBuffer[4] = (u8)(runTime >> 8);
    skyBusBuffer[5] = (u8)(runTime);
    skyBusBuffer[6] = (u8)(fadeTime >> 8);
    skyBusBuffer[7] = (u8)(fadeTime);
    skyBusBuffer[8] = brightness;
    skyBusBuffer[9] = effectNumber;

    for (u8 i=0; i<DREAMEXTENSION_NUM_OPTIONS; i++) skyBusBuffer[10 + i] = pOptionBytes[i];

    skyBus_send(DREAMEXTENSION_SKYBUS_PORT, 10 + DREAMEXTENSION_NUM_OPTIONS, skyBusBuffer);
}

/*******************************************************************************
 * dreamEffect_StopImmediate
 ******************************************************************************/
void dreamEffect_StopImmediate(u8 boardAddress) {

    skyBusBuffer[0] = boardAddress;
    skyBusBuffer[1] = DREAMEXTENSION_COMMAND_STOP_IMMEDIATE;

    skyBus_send(DREAMEXTENSION_SKYBUS_PORT, 2, skyBusBuffer);
}

/*******************************************************************************
 * dreamEffect_Stop
 ******************************************************************************/
void dreamEffect_Stop(u8 boardAddress) {

    skyBusBuffer[0] = boardAddress;
    skyBusBuffer[1] = DREAMEXTENSION_COMMAND_STOP;

    skyBus_send(DREAMEXTENSION_SKYBUS_PORT, 2, skyBusBuffer);
}
