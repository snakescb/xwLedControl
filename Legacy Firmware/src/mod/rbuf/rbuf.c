/***************************************************************************//**
 * @file        rbuf.c
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 712 $
 *              $LastChangedDate: 2010-06-01 17:10:29 +0200 (Di, 01 Jun 2010) $
 *
 * Ringbuffers class, creating byte ringbuffers
 ******************************************************************************/
 #include "rbuf.h"

/******************************************************************************/
/* flush                     												  */
/******************************************************************************/
void rbuf_flush(ringbufferHandle_t* h) {
    h->writeIndex = 0;
    h->readIndex = 0;
}

/******************************************************************************/
/* push               												          */
/******************************************************************************/
bool rbuf_push(ringbufferHandle_t* h, u8 e) {
    //calculate the next write index
    u16 next = h->writeIndex + 1;
    if (next >= h->bufsize) {
        next = 0;
    }

    //check if the buffer is full
    if (next == h->readIndex) {
        return false;
    }

    //write data and change index
    h->pBuffer[h->writeIndex] = e;
    h->writeIndex = next;

    return true;
}

/******************************************************************************/
/* pop                       												  */
/******************************************************************************/
bool rbuf_pop(ringbufferHandle_t* h, u8* pE) {
    //check if buffer is empty
    if (h->readIndex == h->writeIndex) {
        return false;
    }
    //copy data to location
    *pE = h->pBuffer[h->readIndex];

    //increment read index
    h->readIndex++;
    if (h->readIndex >= h->bufsize) {
        h->readIndex = 0;
    }
    return true;
}
