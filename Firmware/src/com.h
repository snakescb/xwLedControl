/***************************************************************************//**
 * @file        com.h
 * @author      Created: Christian Luethi
 * @brief       Simple communication module
 ******************************************************************************/

#ifndef _COM_H_
#define _COM_H_

#include "common.h"

/* Types ------------------------------------------------------------------*/
#define COM_SOF         '{'
#define COM_PAD         '/'
#define COM_EOF         '}'
#define COM_CRC_INIT    0x12
#define COM_MAX_DATA_LEN 256

typedef enum {
    FRAME_STATUS_IDLE = 0,    
    FRAME_STATUS_WAIT_SCOPE,
    FRAME_STATUS_WAIT_DATA
} comFramStatus_t;

typedef struct {
    comFramStatus_t frameStatus;
    uint8_t dataScope;
    uint8_t data[COM_MAX_DATA_LEN];
    uint8_t crc;
    uint8_t rxCount;
    bool isPadded;
} comFrame_t;

/**************************************************************************//**
 * @brief    initialisiere einen com handle
 *****************************************************************************/
void com_handleInit(comFrame_t* frame);

/**************************************************************************//**
 * @brief    update funktion, gibt true zurück wenn ein frame komplett (und korrekt) empfangen wurde
 *****************************************************************************/
bool com_update(comFrame_t* frame, uint8_t d);

/**************************************************************************//**
 * @brief    erstellt den datenstream zum senden eines frames. Gibt die länge der Zeichen zurück, die gesendet werden müssen
 *****************************************************************************/
uint16_t com_prepareBuffer(uint8_t dataScope, uint8_t dataInLen, uint8_t* dataIn, uint8_t* dataOut);

#endif /* _COM_H_ */


