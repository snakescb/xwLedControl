/***************************************************************************//**
 * @file        skyBus.c
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 712 $
 *              $LastChangedDate: 2010-06-01 17:10:29 +0200 (Di, 01 Jun 2010) $
 *
 * Inter Board Protokoll
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "skyBus.h"
#include "uart3.h"

/* Defines --------------------------------------------------------------------*/
#define SKYBUS_SOF          0xC1
#define SKYBUS_EOF          0xC2
#define SKYBUS_PAD          0xC3
#define SKYBUS_PAD_OFFSET   0x10
#define SKYBUS_RX_BUF_LEN    128

/* internal types -------------------------------------------------------------*/
typedef enum {
    SKYBUS_TX_STATE_UNUSED = 0,
    SKYBUS_TX_STATE_READY
} skyBus_TxState_e;

typedef enum {
    SKYBUS_RX_STATE_IDLE = 0,
    SKYBUS_RX_STATE_START_RECEIVED,
    SKYBUS_RX_STATE_PORT_RECEIVED,
    SKYBUS_RX_STATE_LENGTH_RECEIVED,
    SKYBUS_RX_STATE_DATA_RECEIVED,
    SKYBUS_RX_STATE_CHECKSUM_RECEIVED,
} skyBus_RxState_e;

/* Function prototypes --------------------------------------------------------*/
u8 skyBus_putPaddedChar(u8 d, u8 chkSumOld);

/* Variables ------------------------------------------------------------------*/
skybusCallback_t skyBus_callBack;
skyBus_TxState_e skyBus_txState;
skyBus_RxState_e skyBus_rxState;

u8 skyBus_rxChecksum;
u8 skyBus_rxPort;
u8 skyBus_rxLen;
u8 skyBus_rxDataCounter;
u8 skyBus_rxBuffer[SKYBUS_RX_BUF_LEN];

bool skyBus_padReceived;
bool skyBus_rxChecksumOK;

/*******************************************************************************
 * skyBus_send
 ******************************************************************************/
void skyBus_send(u8 port, u8 len, u8* data) {
    if (skyBus_txState != SKYBUS_TX_STATE_UNUSED ) {

        u8 chksum = 0;

        uart3_putChar(SKYBUS_SOF);
        chksum = skyBus_putPaddedChar(port, chksum);
        chksum = skyBus_putPaddedChar(len, chksum);

        for (u8 i=0; i<len; i++) chksum = skyBus_putPaddedChar(data[i], chksum);

        chksum = skyBus_putPaddedChar(chksum, chksum);
        uart3_putChar(SKYBUS_EOF);

    }
}

/*******************************************************************************
 * skyBus_putPaddedChar
 ******************************************************************************/
u8 skyBus_putPaddedChar(u8 d, u8 chkSumOld) {

    if ((d == SKYBUS_SOF) || (d == SKYBUS_EOF) || (d == SKYBUS_PAD)) {
        uart3_putChar(SKYBUS_PAD);
        uart3_putChar(d + SKYBUS_PAD_OFFSET);
    }
    else uart3_putChar(d);

    return chkSumOld + d;
}


/*******************************************************************************
 * skBus_init
 ******************************************************************************/
void skyBus_init(u8 skyBusMode) {

    if (skyBusMode == SKYBUS_MODE_RX_ONLY) {
        uart3_init(UART3_RX);
        skyBus_txState = SKYBUS_TX_STATE_UNUSED;
    }
    else {
        uart3_init(UART3_RX | UART3_TX);
        skyBus_txState = SKYBUS_TX_STATE_READY;
    }

    skyBus_callBack = null;
    skyBus_rxState = SKYBUS_RX_STATE_IDLE;

    uart3_txFlush();
    uart3_rxFlush();
}

/*******************************************************************************
 * skyBus_registerCallback
 ******************************************************************************/
void skyBus_registerCallback(skybusCallback_t f) {
    skyBus_callBack = f;
}

/*******************************************************************************
 * skyBus_hisr
 ******************************************************************************/
void skyBus_hisr(void) {

    //versuche zeichen zu empfangen
    u8 rxByte;
    if (uart3_getChar(&rxByte)) {

        if (rxByte == SKYBUS_SOF) {
            skyBus_rxState = SKYBUS_RX_STATE_START_RECEIVED;
            skyBus_rxChecksum = 0;
            skyBus_rxChecksumOK = false;
            skyBus_padReceived = false;
        }
        else {
            if (rxByte == SKYBUS_EOF) {
                if (skyBus_rxChecksumOK) if (skyBus_callBack != null) skyBus_callBack(skyBus_rxPort, skyBus_rxLen, skyBus_rxBuffer);
                skyBus_rxState = SKYBUS_RX_STATE_IDLE;
            }
            else {
                if (rxByte == SKYBUS_PAD) skyBus_padReceived = true;
                else {

                    u8 actualData;
                    if (skyBus_padReceived) actualData = rxByte - SKYBUS_PAD_OFFSET;
                    else actualData = rxByte;
                    skyBus_padReceived = false;

                    switch (skyBus_rxState) {

                        case SKYBUS_RX_STATE_START_RECEIVED: {
                            skyBus_rxChecksum += actualData;
                            skyBus_rxPort = actualData;
                            skyBus_rxState = SKYBUS_RX_STATE_PORT_RECEIVED;
                            break;
                        }

                        case SKYBUS_RX_STATE_PORT_RECEIVED: {
                            skyBus_rxChecksum += actualData;
                            skyBus_rxLen = actualData;
                            skyBus_rxDataCounter = 0;
                            skyBus_rxState = SKYBUS_RX_STATE_LENGTH_RECEIVED;
                            break;
                        }

                        case SKYBUS_RX_STATE_LENGTH_RECEIVED: {
                            skyBus_rxChecksum += actualData;
                            skyBus_rxBuffer[skyBus_rxDataCounter] = actualData;
                            skyBus_rxDataCounter++;
                            if (skyBus_rxDataCounter >= skyBus_rxLen) {
                                skyBus_rxState = SKYBUS_RX_STATE_DATA_RECEIVED;
                            }
                            break;
                        }

                        case SKYBUS_RX_STATE_DATA_RECEIVED: {
                            if (skyBus_rxChecksum == actualData) skyBus_rxChecksumOK = true;
                            skyBus_rxState = SKYBUS_RX_STATE_CHECKSUM_RECEIVED;
                            break;
                        }

                        default: {
                            skyBus_rxState  = SKYBUS_RX_STATE_IDLE;
                            break;
                        }
                    }
                }
            }
        }
    }
}

/*******************************************************************************
 * skyBus_tx_busy
 ******************************************************************************/
bool skyBus_tx_busy() {
    if (uart3_txSending()) return true;
    return false;
}
