/***************************************************************************//**
 * @file        teco.cpp
 * @author      Created: Christian Luethi
 * @brief       Test and config module
 ******************************************************************************/

#include "teco.h"
#include "uart1.h"
//#include "ledControl.h"
#include "statusLed.h"
#include "conf.h"
//#include "recv.h"
#include "hwVersion.h"
//#include "sensor.h"
//#include "simpleString.h"
//#include "dreamExtension.h"

/* Definitions -------------------------------------------------------------*/
#define TECO_TRACE_STRING_MAX_LENGTH 200

/* Forward declarations ----------------------------------------------------*/
void teco_commandHandler(comFrame_t* frame);
void teco_configHandler(comFrame_t* frame);
void teco_ledHandler(comFrame_t* frame);

/* Private variables -------------------------------------------------------*/
uint8_t tecoTxBuffer[COM_MAX_DATA_LEN*2 + 5];
comFrame_t comRxFrame;

/*****************************************************************************
  Function:     teco_init
*****************************************************************************/
void teco_init(void) {
    uart1_init();
    com_handleInit(&comRxFrame);
}

/*****************************************************************************
  Function:     teco_hisr
*****************************************************************************/
void teco_hisr(void) {
    
    uint8_t rxData;
    while (uart1_getChar(&rxData)) {

        if(com_update(&comRxFrame, rxData)) {                                          

            //check to frames DLCI to distinguish between handlers
            switch(comRxFrame.dataScope) {

                case SCOPE_COMMAND: {
                    teco_commandHandler(&comRxFrame);
                    break;
                }

                case SCOPE_CONFIG: {
                    teco_configHandler(&comRxFrame);
                    break;
                }

                case SCOPE_LED: {
                    teco_ledHandler(&comRxFrame);
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
void teco_commandHandler(comFrame_t* frame) {
    //check the command
    uint8_t commandId = frame->data[0];

    switch(commandId) {

        case COMMAND_RESET: {
            NVIC_SystemReset();
            break;
        }

        case COMMAND_GET_STATIC_INFO: {
            uint8_t  rsp[20];
            uint8_t* pUID = (uint8_t*)0x1FFFF7E8;

            rsp[0] = COMMAND_RESPONSE_STATIC_INFO;
            for (uint8_t i=0; i<12; i++) rsp[i+1] = pUID[i];
            rsp[13] = (uint8_t)(configConfigSize >> 24);
            rsp[14] = (uint8_t)(configConfigSize >> 16);
            rsp[15] = (uint8_t)(configConfigSize >>  8);
            rsp[16] = (uint8_t)(configConfigSize);
            rsp[17] = hwVersion_read();
            rsp[18] = SW_VER_MAJOR;
            rsp[19] = SW_VER_MINOR;

            teco_send(SCOPE_COMMAND, 20, rsp);
            break;
        }

        case COMMAND_GET_VARIABLE_INFO: {

            uint8_t rsp[] = {COMMAND_RESPONSE_VARIABLE_INFO};
            teco_send(SCOPE_COMMAND, 1, rsp);

            /*
            extern uint8_t  numSequences;
            extern selectionMode_e selectionMode;
            extern uint32_t ledControl_configSize;

            uint8_t  rsp[8];
            
            rsp[0] = COMMAND_RESPONSE_VARIABLE_INFO;
            rsp[1] = (uint8_t)(ledControl_configSize >> 24);
            rsp[2] = (uint8_t)(ledControl_configSize >> 16);
            rsp[3] = (uint8_t)(ledControl_configSize >>  8);
            rsp[4] = (uint8_t)(ledControl_configSize);
            rsp[5] = numSequences;
            rsp[6] = (uint8_t)selectionMode;
            rsp[7] = boardConfig.batteryMinVoltage;
            

            teco_send(DLCI_COMMAND, 8, rsp);
            */
            break;
        }

        case COMMAND_GET_DYNAMIC_INFO: {

            uint8_t rsp[] = {COMMAND_RESPONSE_DYNAMIC_INFO};
            teco_send(SCOPE_COMMAND, 1, rsp);

            /*
            extern uint8_t   currentSequence;
            extern uint8_t   numSequences;
            extern uint16_t  auxSignal;
            extern uint8_t   battVoltageRounded;
            extern bool      batteryWarning;            
            extern uint8_t   numOutputsReport;
            extern uint8_t   sequenceDimReport;
            extern runMode_e runMode;

            int16_t rxScaled = recv_signal();
            uint8_t  dim = auxSignal / 10;
            uint8_t rsp[10];

            
            rsp[0]  = COMMAND_RESPONSE_DYNAMIC_INFO;
            rsp[1]  = (uint8_t)recv_failsafe();
            rsp[2]  = (uint8_t)(rxScaled >> 8);
            rsp[3]  = (uint8_t)(rxScaled);
            rsp[4]  = dim;
            rsp[5]  = (uint8_t)batteryWarning;
            rsp[6]  = battVoltageRounded;
            rsp[7]  = currentSequence;
            rsp[8]  = numOutputsReport;
            rsp[9]  = sequenceDimReport;
            rsp[10] = (uint8_t)runMode;

            teco_send(DLCI_COMMAND, 10, rsp);
            */
            break;
        }

        case COMMAND_STATUS_LED_MODE: {
            //uint8_t enable = handle->frame.pIfield[1];
            //if (enable) statusLed_setMode(STATUS_LED_MODE_SENSORTEST);
            //else statusLed_setMode(STATUS_LED_MODE_NORMAL);
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
void teco_configHandler(comFrame_t* frame) {

    //check the command
    uint8_t commandId = frame->data[0];

    switch(commandId) {


        case CONFIG_GET_FRAME_MAX_SIZE: {
            uint8_t rsp[] = {CONFIG_RESPONSE_FRAME_MAX_SIZE, (uint8_t)(configMaxFrameSize >> 8), (uint8_t)configMaxFrameSize};
            teco_send(SCOPE_CONFIG, 3, rsp);
            break;
        }

        case CONFIG_RESET_POINTER: {
            config_resetPointers();
            uint8_t rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, true};
            teco_send(SCOPE_CONFIG, 2, rsp);
            break;
        }

        case CONFIG_ERASE_CONFIG: {
            bool ret = config_erase();
            uint8_t rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, ret};
            teco_send(SCOPE_CONFIG, 2, rsp);
            break;
        }

        case CONFIG_SET_FRAME: {
            uint8_t frameSize = frame->data[1];
            bool ret = config_write(frameSize, &(frame->data[2]));
            uint8_t rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, ret};
            teco_send(SCOPE_CONFIG, 2, rsp);
            break;
        }

        case CONFIG_GET_FRAME: {            
            uint8_t frameSize = frame->data[1];
            uint8_t* read = config_readBytes(frameSize);
            if (read == null) {
                uint8_t rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, false};
                teco_send(SCOPE_CONFIG, 2, rsp);
            }
            else {
                uint8_t rsp[COM_MAX_DATA_LEN];
                rsp[0] = CONFIG_RESPONSE_FRAME;
                rsp[1] = frameSize;
                for (uint8_t i=0; i<frameSize; i++) rsp[i+2] = read[i];
                teco_send(SCOPE_CONFIG, frameSize+2, rsp);
            }
            
            break;
        }

        case CONFIG_REINITIALIZE: {
            config_init();
            //ledControl_init();
            uint8_t rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, true};
            teco_send(SCOPE_CONFIG, 2, rsp);
            break;
        }

        case CONFIG_SET_SELECTION_MODE: {
            /*
            uint8_t newMode = handle->frame.pIfield[1];
            boardConfig.modeSelection = newMode;
            config_boardConfigWrite();
            ledControl_init();
            uint8_t rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, true};
            teco_send(DLCI_CONFIG, 2, rsp);
            */
            break;
        }

        case CONFIG_GET_SELECTION_MODE: {
            //uint8_t rsp[] = {CONFIG_RESPONSE_SELECTION_MODE, boardConfig.modeSelection};
            //teco_send(DLCI_CONFIG, 2, rsp);
            break;
        }

        case CONFIG_SET_THRESHOLD_VOLTAGE: {
            /*
            uint8_t v = handle->frame.pIfield[1];
            boardConfig.batteryMinVoltage = v;
            config_boardConfigWrite();
            ledControl_init();
            uint8_t rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, true};
            teco_send(DLCI_CONFIG, 2, rsp);
            */
            break;
        }

        case CONFIG_GET_THRESHOLD_VOLTAGE: {
            //uint8_t rsp[] = {CONFIG_RESPONSE_THRESHOLD_VOLTAGE, boardConfig.batteryMinVoltage};
            //teco_send(DLCI_CONFIG, 2, rsp);
            break;
        }

        case CONFIG_SET_FACTORY_DEFAULTS: {
            /*
            config_erase();
            config_boardConfigFactoryDefault();
            ledControl_init();
            uint8_t rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, true};
            teco_send(DLCI_CONFIG, 2, rsp);
            */
            break;
        }

        case CONFIG_SET_BOARD_DEFAULTS: {
            /*
            config_boardConfigFactoryDefault();
            ledControl_init();
            uint8_t rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, true};
            teco_send(DLCI_CONFIG, 2, rsp);
            */
            break;
        }

        case CONFIG_RECEIVER_CALIBRATION: {
            /*
            uint8_t rsp[] = {CONFIG_RESPONSE_ACKNOWLEDGE, true};
            uint8_t mode = handle->frame.pIfield[1];
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
            */
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
void teco_ledHandler(comFrame_t* frame) {
    //check the command
    uint8_t commandId = frame->data[0];

    switch(commandId) {

        case LED_ENABLE_DISABLE: {
            //uint8_t rsp[] = {LED_RESPONSE_ACKNOWLEDGE, true};
            //ledControl_activate(handle->frame.pIfield[1]);
            //teco_send(DLCI_LED, 2, rsp);
            break;
        }

        case LED_MANUAL_BRIGHTNESS: {
            //uint8_t rsp[] = {LED_RESPONSE_ACKNOWLEDGE, true};
            //ledControl_setPWM(&(handle->frame.pIfield[1]));
            //teco_send(DLCI_LED, 2, rsp);
            break;
        }

        case LED_SIM_SET_OBJECTS: {
            //ledControl_setSimObjects(handle->frame.pIfield[1], handle->frame.pIfield[2], handle->frame.pIfield + 3);
            break;
        }

        case LED_SIM_GET_BUFFER_INFO: {
            //int8_t rsp[4];
            //ledControl_getBufferInfo(rsp);
            //teco_send(DLCI_LED, 4, rsp);
            break;
        }

        case LED_SIM_GET_BUFFER_STATE: {
            //uint8_t rsp[25];
            //ledControl_getBufferState(rsp);
            //teco_send(DLCI_LED, 25, rsp);
            break;
        }

        case LED_START_SIM: {
            /*
            uint8_t rsp[] = {LED_RESPONSE_ACKNOWLEDGE, true};
            uint32_t s = 0;
            uint8_t  d = 0;

            s += handle->frame.pIfield[1] << 24;
            s += handle->frame.pIfield[2] << 16;
            s += handle->frame.pIfield[3] <<  8;
            s += handle->frame.pIfield[4];
            d  = handle->frame.pIfield[5];

            ledControl_startSim(s, d, false, null, null);
            teco_send(DLCI_LED, 2, rsp);
            */
            break;
        }

        case LED_START_SIM_WITH_OFFSET: {
            /*
            uint8_t rsp[] = {LED_RESPONSE_ACKNOWLEDGE, true};
            uint32_t s = 0;
            uint8_t  d = 0;

            s += handle->frame.pIfield[1] << 24;
            s += handle->frame.pIfield[2] << 16;
            s += handle->frame.pIfield[3] <<  8;
            s += handle->frame.pIfield[4];
            d  = handle->frame.pIfield[5];

            ledControl_startSim(s, d, true, &(handle->frame.pIfield[6]), &(handle->frame.pIfield[10]));
            teco_send(DLCI_LED, 2, rsp);
            */
            break;
        }

        case LED_AUX_SIM: {
            /*
            extern uint16_t auxSim;
            uint8_t rsp[] = {LED_RESPONSE_ACKNOWLEDGE, true};
            auxSim = handle->frame.pIfield[1]*10;
            teco_send(DLCI_LED, 2, rsp);
            */
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
void teco_send(uint8_t dataScope, uint16_t dataLen, uint8_t* pData) {
    uint16_t txLen = com_prepareBuffer(dataScope, dataLen, pData, tecoTxBuffer);
    for (uint16_t i=0; i<txLen; i++) uart1_putChar(tecoTxBuffer[i]);     
}

/*****************************************************************************
  Function:     teco_stringTrace
*****************************************************************************/
void teco_stringTrace(uint8_t* s) {

    if(s[0] == 0) return;

    //länge bestimmen
    uint8_t dataLen = 0;
    while (dataLen < TECO_TRACE_STRING_MAX_LENGTH) {
        if (s[dataLen] == 0) break;
        else dataLen++;
    }

    teco_send(SCOPE_TRACE, dataLen, s);
}