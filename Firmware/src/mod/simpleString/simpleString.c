/***************************************************************************//**
 * @file        simpleString.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 712 $
 *              $LastChangedDate: 2010-06-01 17:10:29 +0200 (Di, 01 Jun 2010) $
 *
 * Einfache Stringfunktionen ohne C libs
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "simpleString.h"

#define INT_TO_STRING_BUF_SIZE  32

u8 intToStringBuf[INT_TO_STRING_BUF_SIZE];

/*******************************************************************************
 * intToString
 ******************************************************************************/
u8* intToString(s32 value, bool cr) {

    u8 index = INT_TO_STRING_BUF_SIZE - 1;

    intToStringBuf[index] = 0;
    index--;

    if (cr) {
        intToStringBuf[index] = '\n';
        index--;
        intToStringBuf[index] = '\r';
        index--;
    }

    bool negative = false;
    s32 tmp = value;

    if (tmp < 0) {
        negative = true;
        tmp = -tmp;
    }

    while (1) {
        intToStringBuf[index] = 48 + (tmp % 10);
        tmp /= 10;

        if (tmp == 0) {
            if (negative) {
                index--;
                intToStringBuf[index] = '-';
            }
            return &intToStringBuf[index];
        }

        index--;
    }
}
