/***************************************************************************//**
 * @file        rbuf.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 712 $
 *              $LastChangedDate: 2010-06-01 17:10:29 +0200 (Di, 01 Jun 2010) $
 *
 * Ringbuffers class, creating byte ringbuffers
 ******************************************************************************/

#ifndef RBUF_H
#define RBUF_H

#include "common.h"

///this macro can be used to initializ e aringbuffer handle
#define DEFINE_RINGBUFFER(p,s) {p,s,0,0}

/*******************************************************************//**
* @brief    Defines the handle which has to be provided to the ringbuffer
*
* To use a ringbugger, create such a handle and a local buffer. Then
* set the pBufffer pointer and bufsize, and then call rbuf_flush(). Then
* the buffer is ready to use with the ringbuffer functions.
* Alternatively you can use the DEFINE_RINGBUFFER macro to define and
* initialize a handle.
***********************************************************************/
typedef struct {
    u8* pBuffer;
    u16 bufsize;
    u16 writeIndex;
    u16 readIndex;
} ringbufferHandle_t;

/*******************************************************************//**
* @brief    Store one data item on the buffer
*
* One sample of type u8 is stored in the buffer by using this function.
* The data is stored only when the buffer is not full. When the data is
* stored in the buffer the function returns true, else is returns false.
*
* @param    e :  element to push into the ring of template type u8
* @return   true if storage ok, false if buffer is full
* @warning  Not thread safe
***********************************************************************/
bool rbuf_push(ringbufferHandle_t*, u8);

/*******************************************************************//**
* @brief    Read one data item from the buffer
*
* When data is readed but the buffer is empty, this function returns
* false. Whe the read was valid true is returned and the data is copied
* to the desired location.
*
* @param    pE :  address where the data element should be stored
* @return   true if read ok, false if buffer is empty
* @warning  Not thread safe
***********************************************************************/
bool rbuf_pop(ringbufferHandle_t*, u8*);

/*******************************************************************//**
* @brief    Clears and initializes the buffer
***********************************************************************/
void rbuf_flush(ringbufferHandle_t*);

#endif //RBUF_H
