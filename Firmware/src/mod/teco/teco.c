/***************************************************************************//**
 * @file        teco.cpp
 * @author      Created: DWY
 *              $Author: CLU $
 *              $LastChangedRevision: 876 $
 *              $LastChangedDate: 2010-06-16 01:46:51 +0200 (Mi, 16 Jun 2010) $
 * @brief       Test and config module
 ******************************************************************************/

/******************************************************************************
 Imported Include References
******************************************************************************/
#include "teco.h"
#include "uart1.h"
#include "ledControl.h"
#include "statusLed.h"
#include "conf.h"
#include "recv.h"
#include "hwVersion.h"
#include "sensor.h"
#include "simpleString.h"
#include "dreamExtension.h"

/**************************************************************************//**
* @brief    There are the declarations for the hdlc receive service.
******************************************************************************/
#define TECO_HDLC_RX_IFIELD_MAX_LENGTH 256
static u8 tecoHdlcIFieldRx[TECO_HDLC_RX_IFIELD_MAX_LENGTH];
static hdlc_frameHandle_t tecoHdlcHandleRx;

/**************************************************************************//**
* @brief    There are the declarations for the hdlc transmit service.
******************************************************************************/
#define TECO_HDLC_TX_IFIELD_MAX_LENGTH 256
static u8 tecoHdlcIFieldTx[TECO_HDLC_TX_IFIELD_MAX_LENGTH];
static hdlc_frameHandle_t tecoHdlcHandleTx;

/**************************************************************************//**
* @brief    There are the declarations for the hdlc trace service.
******************************************************************************/
#define TECO_TRACE_STRING_MAX_LENGTH 256
static hdlc_frameHandle_t hdlcHandleTxTrce;
char   formatStringBuffer[TECO_TRACE_STRING_MAX_LENGTH];

/*****************************************************************************
  Function:     teco_init
*****************************************************************************/
void teco_init(void) {
    hdlc_handleInit(&tecoHdlcHandleRx, tecoHdlcIFieldRx, TECO_HDLC_RX_IFIELD_MAX_LENGTH, NULL);
    hdlc_handleInit(&tecoHdlcHandleTx, tecoHdlcIFieldTx, TECO_HDLC_TX_IFIELD_MAX_LENGTH, &uart1_putChar);
    hdlc_handleInit(&hdlcHandleTxTrce, tecoHdlcIFieldTx, TECO_TRACE_STRING_MAX_LENGTH, &uart1_putChar);
}

/*****************************************************************************
  Function:     teco_hisr
*****************************************************************************/
void teco_hisr(void) {
    // local variable definition
    u8 rxData;
    while (uart1_getChar(&rxData)) {

        if(hdlc_collectAndComputeData(&tecoHdlcHandleRx, rxData)) {

            //check to frames DLCI to distinguish between handlers
            switch(tecoHdlcHandleRx.frame.ADDR.field.DLCI) {

                case DLCI_COMMAND: {
                    teco_commandHandler(&tecoHdlcHandleRx);
                    break;
                }

                case DLCI_CONFIG: {
                    teco_configHandler(&tecoHdlcHandleRx);
                    break;
                }

                case DLCI_LED: {
                    teco_ledHandler(&tecoHdlcHandleRx);
                    break;
                }

                default: {
                    TRACE("Invalid DLCI\r\n");
                    break;
                }
            }
        }
    }
}

/*****************************************************************************
  Function:     teco_commandHandler
*****************************************************************************/
void teco_commandHandler(hdlc_frameHandle_t* handle) {
    //check the command
    u8 commandId = handle->frame.pIfield[0];

    switch(commandId) {

        case COMMAND_RESET: {
            NVIC_SystemReset();
            break;
        }

        case COMMAND_GET_STATIC_INFO: {
            u8  rsp[20];
            u8* pUID = (u8*)0x1FFFF7E8;

            rsp[0] = COMMAND_RESPONSE_STATIC_INFO;
            for (u8 i=0; i<12; i++) rsp[i+1] = pUID[i];
            rsp[13] = (u8)(configConfigSize >> 24);
            rsp[14] = (u8)(configConfigSize >> 16);
            rsp[15] = (u8)(configConfigSize >>  8);
            rsp[16] = (u8)(configConfigSize);
            rsp[17] = hwVersion_read();
            rsp[18] = SW_VER_MAJOR;
            rsp[19] = SW_VER_MINOR;

            teco_send(DLCI_COMMAND, 20, rsp);
            break;
        }

        case COMMAND_GET_VARIABLE_INFO: {
            extern u8  numSequences;
            extern selectionMode_e selectionMode;
            extern u32 ledControl_configSize;

            u8  rsp[8];

            rsp[0] = COMMAND_RESPONSE_VARIABLE_INFO;
            rsp[1] = (u8)(ledControl_configSize >> 24);
            rsp[2] = (u8)(ledControl_configSize >> 16);
            rsp[3] = (u8)(ledControl_configSize >>  8);
            rsp[4] = (u8)(ledControl_configSize);
            rsp[5] = numSequences;
            rsp[6] = (u8)selectionMode;
            rsp[7] = boardConfig.batteryMinVoltage;

            teco_send(DLCI_COMMAND, 8, rsp);
            break;
        }

        case COMMAND_GET_DYNAMIC_INFO: {
            extern u8   currentSequence;
            extern u8   numSequences;
            extern u16  auxSignal;
            extern u8   battVoltageRounded;
            extern bool batteryWarning;
            extern u32  speedInfoReport;
            extern u8   numOutputsReport;
            extern u8   sequenceDimReport;
            extern runMode_e runMode;

            s16 rxScaled = recv_signal();
            u8  dim = auxSignal / 10;
            u16 rpmReport = sensor_getRPM();
            u8 rsp[20];

            rsp[0]  = COMMAND_RESPONSE_DYNAMIC_INFO;
            rsp[1]  = (u8)recv_failsafe();
            rsp[2]  = (u8)(rxScaled >> 8);
            rsp[3]  = (u8)(rxScaled);
            rsp[4]  = dim;
            rsp[5]  = (u8)batteryWarning;
            rsp[6]  = battVoltageRounded;
            rsp[7]  = currentSequence;
            rsp[8]  = numOutputsReport;
            rsp[9]  = (u8)(rpmReport >> 8);
            rsp[10] = (u8)(rpmReport);
            rsp[11] = (u8)(speedInfoReport >> 24);
            rsp[12] = (u8)(speedInfoReport >> 16);
            rsp[13] = (u8)(speedInfoReport >>  8);
            rsp[14] = (u8)(speedInfoReport);
            rsp[15] = sequenceDimReport;
            rsp[16] = (u8)runMode;

            s16 rx2 = recv_signal_rx2();
            rsp[17]  = (u8)(rx2 >> 8);
            rsp[18]  = (u8)(rx2);
            rsp[19]  = (u8)recv_failsafe_rx2();

            teco_send(DLCI_COMMAND, 20, rsp);
            break;
        }

        case COMMAND_STATUS_LED_MODE: {
            u8 enable = handle->frame.pIfield[1];
            if (enable) statusLed_setMode(STATUS_LED_MODE_SENSORTEST);
            else statusLed_setMode(STATUS_LED_MODE_NORMAL);
            break;
        }

        default: {
            //ignore unsupported message types
            TRACE("Unknown command\r\n");
            break;
        }
    }
}

/*****************************************************************************
  Function:     teco_configHandler
*****************************************************************************/
void teco_configHandler(hdlc_frameHandle_t* handle) {

    //check the command
    u8 commandId = handle->frame.pIfield[0];

    switch(commandId) {


        case CONFIG_GET_FRAME_MAX_SIZE: {
            u8 rsp[] = {CONFIG_RESPONSE_FRAME_MAX_SIZE, (u8)(configMaxFrameSize >> 8), (u8)configMaxFrameSize};
            teco_send(DLCI_CONFIG, 3, rsp);
            break;
        }

        case CONFIG_RESET_POINTER: {
            config_resetPointers();
            u8 rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, true};
            teco_send(DLCI_CONFIG, 2, rsp);
            break;
        }

        case CONFIG_ERASE_CONFIG: {
            bool ret = config_erase();
            u8 rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, ret};
            teco_send(DLCI_CONFIG, 2, rsp);
            break;
        }

        case CONFIG_SET_FRAME: {
            u8 frameSize = handle->frame.pIfield[1];
            bool ret = config_write(frameSize, &(handle->frame.pIfield[2]));
            u8 rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, ret};
            teco_send(DLCI_CONFIG, 2, rsp);
            break;
        }

        case CONFIG_GET_FRAME: {
            u8 frameSize = handle->frame.pIfield[1];
            u8* read = config_readBytes(frameSize);
            if (read == null) {
                u8 rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, false};
                teco_send(DLCI_CONFIG, 2, rsp);
            }
            else {
                u8 rsp[TECO_HDLC_RX_IFIELD_MAX_LENGTH];
                rsp[0] = CONFIG_RESPONSE_FRAME;
                rsp[1] = frameSize;
                for (u8 i=0; i<frameSize; i++) rsp[i+2] = read[i];
                teco_send(DLCI_CONFIG, frameSize+2, rsp);
            }
            break;
        }

        case CONFIG_REINITIALIZE: {
            config_init();
            ledControl_init();
            u8 rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, true};
            teco_send(DLCI_CONFIG, 2, rsp);
            break;
        }

        case CONFIG_SET_SELECTION_MODE: {
            u8 newMode = handle->frame.pIfield[1];
            boardConfig.modeSelection = newMode;
            config_boardConfigWrite();
            ledControl_init();
            u8 rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, true};
            teco_send(DLCI_CONFIG, 2, rsp);
            break;
        }

        case CONFIG_GET_SELECTION_MODE: {
            u8 rsp[] = {CONFIG_RESPONSE_SELECTION_MODE, boardConfig.modeSelection};
            teco_send(DLCI_CONFIG, 2, rsp);
            break;
        }

        case CONFIG_SET_THRESHOLD_VOLTAGE: {
            u8 v = handle->frame.pIfield[1];
            boardConfig.batteryMinVoltage = v;
            config_boardConfigWrite();
            ledControl_init();
            u8 rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, true};
            teco_send(DLCI_CONFIG, 2, rsp);
            break;
        }

        case CONFIG_GET_THRESHOLD_VOLTAGE: {
            u8 rsp[] = {CONFIG_RESPONSE_THRESHOLD_VOLTAGE, boardConfig.batteryMinVoltage};
            teco_send(DLCI_CONFIG, 2, rsp);
            break;
        }

        case CONFIG_SET_FACTORY_DEFAULTS: {
            config_erase();
            config_boardConfigFactoryDefault();
            ledControl_init();
            u8 rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, true};
            teco_send(DLCI_CONFIG, 2, rsp);
            break;
        }

        case CONFIG_SET_BOARD_DEFAULTS: {
            config_boardConfigFactoryDefault();
            ledControl_init();
            u8 rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, true};
            teco_send(DLCI_CONFIG, 2, rsp);
            break;
        }

        case CONFIG_RECEIVER_CALIBRATION: {
            u8 rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, true};
            u8 mode = handle->frame.pIfield[1];
            if (recv_failsafe()) {
                rsp[1] = false;
            }
            else {
                if      (mode == 1) boardConfig.receiverCenter = recv_rawSignal();
                else if (mode == 0) boardConfig.receiverMax = recv_rawSignal();
                else if (mode == 2) boardConfig.receiverMin = recv_rawSignal();
                else rsp[1] = false;
                config_boardConfigWrite();
            }
            teco_send(DLCI_CONFIG, 2, rsp);
            break;
        }

        default: {
            //ignore unsupported message types
            TRACE("Unknown config command\r\n");
            break;
        }
    }
}

/*****************************************************************************
  Function:     teco_ledHandler
*****************************************************************************/
void teco_ledHandler(hdlc_frameHandle_t* handle) {
    //check the command
    u8 commandId = handle->frame.pIfield[0];

    switch(commandId) {

        case LED_ENABLE_DISABLE: {
            u8 rsp[] = {LED_RESPONSE_ACKNOWLEDGE, true};
            ledControl_activate(handle->frame.pIfield[1]);
            teco_send(DLCI_LED, 2, rsp);
            break;
        }

        case LED_MANUAL_BRIGHTNESS: {
            u8 rsp[] = {LED_RESPONSE_ACKNOWLEDGE, true};
            ledControl_setPWM(&(handle->frame.pIfield[1]));
            teco_send(DLCI_LED, 2, rsp);
            break;
        }

        case LED_SIM_SET_OBJECTS: {
            ledControl_setSimObjects(handle->frame.pIfield[1], handle->frame.pIfield[2], handle->frame.pIfield + 3);
            break;
        }

        case LED_SIM_GET_BUFFER_INFO: {
            u8 rsp[4];
            ledControl_getBufferInfo(rsp);
            teco_send(DLCI_LED, 4, rsp);
            break;
        }

        case LED_SIM_GET_BUFFER_STATE: {
            u8 rsp[25];
            ledControl_getBufferState(rsp);
            teco_send(DLCI_LED, 25, rsp);
            break;
        }

        case LED_START_SIM: {
            u8 rsp[] = {LED_RESPONSE_ACKNOWLEDGE, true};
            u32 s = 0;
            u8  d = 0;

            s += handle->frame.pIfield[1] << 24;
            s += handle->frame.pIfield[2] << 16;
            s += handle->frame.pIfield[3] <<  8;
            s += handle->frame.pIfield[4];
            d  = handle->frame.pIfield[5];

            ledControl_startSim(s, d, false, null, null);
            teco_send(DLCI_LED, 2, rsp);
            break;
        }

        case LED_START_SIM_WITH_OFFSET: {
            u8 rsp[] = {LED_RESPONSE_ACKNOWLEDGE, true};
            u32 s = 0;
            u8  d = 0;

            s += handle->frame.pIfield[1] << 24;
            s += handle->frame.pIfield[2] << 16;
            s += handle->frame.pIfield[3] <<  8;
            s += handle->frame.pIfield[4];
            d  = handle->frame.pIfield[5];

            ledControl_startSim(s, d, true, &(handle->frame.pIfield[6]), &(handle->frame.pIfield[10]));
            teco_send(DLCI_LED, 2, rsp);
            break;
        }

        case LED_AUX_SIM: {
            extern u16 auxSim;
            u8 rsp[] = {LED_RESPONSE_ACKNOWLEDGE, true};
            auxSim = handle->frame.pIfield[1]*10;
            teco_send(DLCI_LED, 2, rsp);
            break;
        }

        default: {
            //ignore unsupported message types
            TRACE("Unknown LED command\r\n");
            break;
        }
    }
}

/*****************************************************************************
  Function:     teco_send
*****************************************************************************/
void teco_send(u8 hdlcDlci, u16 dataLen, u8* pData) {
    for (u16 i=0; i<dataLen; i++) tecoHdlcHandleTx.frame.pIfield[i] = pData[i];
    hdlc_Transmit(&tecoHdlcHandleTx, hdlcDlci, HDLC_ADDR_CR_COMMAND, HDLC_CTRL_FRAME_TYPE_UI, dataLen);
}

/*****************************************************************************
  Function:     teco_stringTrace
*****************************************************************************/
void teco_stringTrace(u8* pData) {

    if(pData[0] == 0) return;

    //l�nge bestimmen
    u8 dataLen = 0;
    while (dataLen < TECO_TRACE_STRING_MAX_LENGTH) {
        dataLen++;
        if (pData[dataLen] == 0) {
            break;
        }
    }

    hdlcHandleTxTrce.frame.pIfield = pData;
    hdlc_Transmit(&hdlcHandleTxTrce, DLCI_TRACE, HDLC_ADDR_CR_COMMAND, HDLC_CTRL_FRAME_TYPE_UIH, dataLen);
}
