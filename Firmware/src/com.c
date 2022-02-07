/***************************************************************************//**
 * @file        com.h
 * @author      Created: Christian Luethi
 * @brief       Simple communication module
 ******************************************************************************/

#include "com.h"

/* Forward declarations ------------------------------------------------------*/
uint8_t com_updateCRC(uint8_t crc, uint8_t c);
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/*******************************************************************************
 * com_handleInit
 ******************************************************************************/
void com_handleInit(comFrame_t* frame) {
    frame->frameStatus = FRAME_STATUS_IDLE;
}

/*******************************************************************************
 * com_update
 ******************************************************************************/
bool com_update(comFrame_t* frame, uint8_t d) {    
    
    switch (frame->frameStatus) {

        case FRAME_STATUS_IDLE: {
            if (d == COM_SOF) {
                frame->crc = COM_CRC_INIT;
                frame->isPadded = false;
                frame->rxCount = 0;
                frame->frameStatus = FRAME_STATUS_WAIT_SCOPE;                
            }
            break;
        }

        case FRAME_STATUS_WAIT_SCOPE:
        case FRAME_STATUS_WAIT_DATA: {
            if (!((d == COM_SOF) || (d == COM_EOF) || (d == COM_PAD)) || (frame->isPadded)) {
                frame->isPadded = false;
                frame->crc = com_updateCRC(frame->crc, d);
                if (frame->frameStatus == FRAME_STATUS_WAIT_SCOPE) {
                    frame->dataScope = d;
                    frame->frameStatus = FRAME_STATUS_WAIT_DATA;
                }
                else frame->data[frame->rxCount++] = d;
            }
            else {
                switch (d) {
                    case COM_SOF: {
                        //this is illegal, marks a restart, restart reception by waiting for scope
                        frame->frameStatus = FRAME_STATUS_WAIT_SCOPE;
                        frame->crc = COM_CRC_INIT;
                        frame->isPadded = false;
                        frame->rxCount = 0;
                        break;
                    }

                    case COM_PAD: {
                        //padding signal
                        frame->isPadded = true;
                        break;
                    }

                    case COM_EOF: {
                        //reception complete, the last received databyte was the crc, check crc (must be 0 here), wait for next start and return if crc ok;
                        frame->rxCount--;                        
                        frame->frameStatus = FRAME_STATUS_IDLE;
                        if (frame->crc == 0) return true;
                        else return false;
                    }
                }
            }
            break;
        }
    }

    return false;
}

/*******************************************************************************
 * com_prepareBuffer
 ******************************************************************************/
uint16_t com_prepareBuffer(uint8_t dataScope, uint8_t dataInLen, uint8_t* dataIn, uint8_t* dataOut) {
    uint16_t cnt = 0;
    uint8_t crc = COM_CRC_INIT;
    dataOut[cnt++] = COM_SOF;

    if ((dataScope == COM_SOF) || (dataScope == COM_PAD) || (dataScope == COM_EOF)) dataOut[cnt++] = COM_PAD;
    dataOut[cnt++] = dataScope;
    crc = com_updateCRC(crc, dataScope);

    for (uint16_t i=0; i<dataInLen; i++) {
        if ((dataIn[i] == COM_SOF) || (dataIn[i] == COM_PAD) || (dataIn[i] == COM_EOF)) dataOut[cnt++] = COM_PAD;
        dataOut[cnt++] = dataIn[i];
        crc = com_updateCRC(crc, dataIn[i]);
    }

    dataOut[cnt++] = crc;
    dataOut[cnt++] = COM_EOF;
    return cnt;
}

/*******************************************************************************
 * com_prepareBuffer
 ******************************************************************************/
uint8_t com_updateCRC(uint8_t crc, uint8_t c) {
   return crc ^ c;
}