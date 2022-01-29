/***************************************************************************//**
 * @file        hdlc.c
 * @author      Created: DWY
 *              $Author: CLU $
 *              $LastChangedRevision: 440 $
 *              $LastChangedDate: 2010-05-20 09:59:35 +0200 (Do, 20 Mai 2010) $
 * @brief       EOS memory pool module.
 ******************************************************************************/ 

/*****************************************************************************
 Imported Include References
*****************************************************************************/
#include "common.h"
#include "hdlc.h"

/*****************************************************************************
 Global Variable Definitions
*****************************************************************************/
#define HDLC_SYNC_BYTE_PATTERN       0xf9  /* see 3GPP 27.010 for definition */
#define HDLC_GET_FRAME_TYPE(pHdlcHandle) (pHdlcHandle->frame.CTRL.value & ~HDLC_CTRL_PF_BIT)
#define HDLC_GET_LENGTH_INDICATOR(pHdlcHandle) ((hdlc_LEN_t)((pHdlcHandle->frame.LEN_L.field.lenL) | (pHdlcHandle->frame.LEN_H.field.lenH<<7)))          

/**************************************************************************//**
* @brief    CRC table for FCS
*
* From ETSI TS 127 010 V8.0.0 Annex B.3.5
******************************************************************************/
const u8 crctable[256] = {//reversed, 8-bit, poly=0x07
    0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75,  0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
    0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69,  0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
    0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D,  0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
    0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51,  0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,
    
    0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05,  0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
    0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19,  0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
    0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D,  0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
    0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21,  0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,
    
    0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95,  0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
    0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89,  0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
    0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD,  0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
    0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1,  0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,
    
    0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5,  0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
    0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9,  0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
    0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD,  0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
    0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1,  0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF
};

/*****************************************************************************
 Local Function Prototypes
*****************************************************************************/

/*****************************************************************************
 Global procedures
*****************************************************************************/

/*****************************************************************************
  Function:     hdlc_handleInit
 *****************************************************************************
  Remarks:      see include file for details
*****************************************************************************/
BOOL hdlc_handleInit(hdlc_frameHandle_t* pHdlcHandle,
                     u8*                 pIField, 
                     u16                 iFieldMaxLength,
                     f_uartTxPutChar_t   f_uartTxPutChar)
{
    
    // verify parameters
    if(pHdlcHandle == NULL)
    {
        return FALSE;
    }
    // - I field data
    if((pIField == NULL)&&
       (iFieldMaxLength > 0))
    {
        return FALSE;
    }
    
    // - max length
    if(iFieldMaxLength > HDLC_I_FIELD_MAX_LENGTH)
    {
        return FALSE;
    }

    
    // Initialize parameters
    pHdlcHandle->iFieldMaxLength = iFieldMaxLength;
    pHdlcHandle->iFieldCount = 0;
    pHdlcHandle->frame.pIfield = pIField;
    pHdlcHandle->f_uartTxPutChar = f_uartTxPutChar;
    
    // Initialization successful
    return TRUE;
}

/*****************************************************************************
  Function:     hdlc_collectAndComputeData
 *****************************************************************************
  Remarks:      see include file for details
*****************************************************************************/
BOOL hdlc_collectAndComputeData(hdlc_frameHandle_t* pHdlcHandle, 
                                hdlc_data_t         data)
{
    BOOL isValidDataAvailable = FALSE;
    

    if(pHdlcHandle == NULL)
    {
        return FALSE;
    }
    
    if((pHdlcHandle->frame.pIfield == NULL)&&
       (pHdlcHandle->iFieldMaxLength > 0))
    {
        return FALSE;
    }
    
    switch (pHdlcHandle->state)
    {
     // this is a state machine that syncs to the stream

    /*************************************************************************
      HDLC_STATE_PHASE_SYNC_LOST search for the sync pattern
    **************************************************************************/
        case HDLC_STATE_PHASE_VALID_FRAME:
        case HDLC_STATE_PHASE_SYNC_LOST:
            if (data == HDLC_SYNC_BYTE_PATTERN)
            { 
                /* initialize the variables and change state */
                pHdlcHandle->iFieldCount       = 0;                 
                pHdlcHandle->frame.SYNC        = HDLC_SYNC_BYTE_PATTERN;
                pHdlcHandle->frame.CTRL.value  = 0;
                pHdlcHandle->frame.LEN_L.value = 0;
                pHdlcHandle->frame.LEN_H.value = 0;                
                pHdlcHandle->frame.FCS         = 0xFF;

                /* only one transaction ID is assumed to be used. */
                /* To be changed if more than one is used. */
                pHdlcHandle->state = HDLC_STATE_PHASE_DLCI;
            }
            else
            { 
                // do noting. wait for the next byte
            }
        break;

    /*************************************************************************
      HDLC_STATE_PHASE_ADDR: listening for the DLCI byte
    **************************************************************************/
        case HDLC_STATE_PHASE_DLCI:
            if (data == HDLC_SYNC_BYTE_PATTERN)
            {
                // This is allowed, on this case just ignore it
            }
            else
            {
                pHdlcHandle->frame.FCS = crctable[pHdlcHandle->frame.FCS^data];
                pHdlcHandle->frame.ADDR.value = data;
                
                pHdlcHandle->state = HDLC_STATE_PHASE_CTRL;                
            }
        break;

    /*************************************************************************
      HDLC_STATE_PHASE_CTRL: listening for the CTRL byte
    **************************************************************************/
        case HDLC_STATE_PHASE_CTRL:
            pHdlcHandle->frame.FCS = crctable[pHdlcHandle->frame.FCS^data];
            pHdlcHandle->frame.CTRL.value = data;
            pHdlcHandle->state = HDLC_STATE_PHASE_LEN_L;
        break;

    /*************************************************************************
      HDLC_STATE_PHASE_LEN_L: listening for the LEN_L (lower byte)
    **************************************************************************/
        case HDLC_STATE_PHASE_LEN_L:
            pHdlcHandle->frame.FCS = crctable[pHdlcHandle->frame.FCS^data];
            pHdlcHandle->frame.LEN_L.value = data;
            if (pHdlcHandle->frame.LEN_L.field.EA == 0)
            {
                // we have the additional length byte
                pHdlcHandle->state = HDLC_STATE_PHASE_LEN_H;
            }
            else
            { 
                // We do not have extended length byte
                
                // Set the next state
                if (HDLC_GET_LENGTH_INDICATOR(pHdlcHandle) > 0)
                { 
                    // start capturing I-field by invoking next state
                    pHdlcHandle->state = HDLC_STATE_PHASE_I_FIELD;
                }
                else
                { 
                    // no data. skip I-field
                    pHdlcHandle->state = HDLC_STATE_PHASE_FCS;
                }
            }            
        break;
    /*************************************************************************
      HDLC_STATE_PHASE_LEN_H: listening for the LEN_H (higher byte)
    **************************************************************************/
        case HDLC_STATE_PHASE_LEN_H:
            pHdlcHandle->frame.FCS = crctable[pHdlcHandle->frame.FCS^data];
            pHdlcHandle->frame.LEN_H.value = data;

            // Set the next state
            if (HDLC_GET_LENGTH_INDICATOR(pHdlcHandle) > 0)
            { 
                // start capturing I-field by invoking next state
                pHdlcHandle->state = HDLC_STATE_PHASE_I_FIELD;
            }
            else
            { 
                // no data. skip I-field
                pHdlcHandle->state = HDLC_STATE_PHASE_FCS;
            }            
            break;       
    /*************************************************************************
      HDLC_STATE_PHASE_I_FIELD: listening for the I-field
    **************************************************************************/
        case HDLC_STATE_PHASE_I_FIELD:
            // save data up to the max space allowed
            if (pHdlcHandle->iFieldCount < pHdlcHandle->iFieldMaxLength)
            {
                // In case of UIH frame the I Field is excluded from FCS calculation.
                // Reference: chap 5.2.16 of ETSI TS 127 010 V8.0.0
                if (HDLC_GET_FRAME_TYPE(pHdlcHandle) != HDLC_CTRL_FRAME_TYPE_UIH)
                {
                    pHdlcHandle->frame.FCS = crctable[pHdlcHandle->frame.FCS^data];
                }
                pHdlcHandle->frame.pIfield[pHdlcHandle->iFieldCount] = data;

                pHdlcHandle->iFieldCount++;
               
            }
            else
            { 
                // message too long. Flush it
                pHdlcHandle->state = HDLC_STATE_PHASE_SYNC_LOST;
            }

            if (pHdlcHandle->iFieldCount == HDLC_GET_LENGTH_INDICATOR(pHdlcHandle))
            { 
                // end of I-field reached.
                pHdlcHandle->state = HDLC_STATE_PHASE_FCS;
            }
            break;

    /*************************************************************************
      HDLC_STATE_PHASE_FCS: listening for the Frame Check Sum
    **************************************************************************/
        case HDLC_STATE_PHASE_FCS:
            pHdlcHandle->frame.FCS = crctable[pHdlcHandle->frame.FCS^data];
            /*0xCF is the reversed order of 11110011.*/
            if (pHdlcHandle->frame.FCS==0xCF) /* check sum is ok */
            {
                // FCS is OK
                
                //DWY: the original code was reporting only in case of data present
                //     we should verifiy if this is ok or not.
                if (pHdlcHandle->iFieldCount)
                {
                     pHdlcHandle->state = HDLC_STATE_PHASE_CLOSING_FLAG;
                }
                else
                {
                    // ignore
                    pHdlcHandle->state = HDLC_STATE_PHASE_SYNC_LOST;
                }
            }
            else
            {
                // there was an error in the FCS
                pHdlcHandle->state = HDLC_STATE_PHASE_SYNC_LOST;
            }
            break;

            
            
    /*************************************************************************
      HDLC_STATE_PHASE_FCS: Waiting Closing FLAG
    **************************************************************************/
        case HDLC_STATE_PHASE_CLOSING_FLAG:
            if (data == HDLC_SYNC_BYTE_PATTERN)
            {
                // Frame received correctly
                 // inform caller about received data */
                 isValidDataAvailable = TRUE;
                 pHdlcHandle->state = HDLC_STATE_PHASE_VALID_FRAME;
            }
            else
            { 
                // there was an error in the CLOSING_FLAG
                pHdlcHandle->state = HDLC_STATE_PHASE_SYNC_LOST;
            }
            break;
            
    /*************************************************************************
      default: an error in SW could cause this only. Return to init state.
    **************************************************************************/
        default:
            // should never reach this point
            pHdlcHandle->state = HDLC_STATE_PHASE_SYNC_LOST;            
            break;
    }

    return isValidDataAvailable;
}

/*****************************************************************************
  Function:     hdlc_fillL1Info
 *****************************************************************************
  Remarks:      see include file for details
*****************************************************************************/
void hdlc_fillL1Info(hdlc_frameHandle_t* pHdlcHandle)
{

    /*************************************************************************
    init local variables
    *************************************************************************/
    hdlc_LEN_t dataIndex;

    // init FCS according 3GP 27 010
    pHdlcHandle->frame.FCS=0xFF;     

    /*************************************************************************
    fill the dpr frame header and update the FCS for each step.
    (Remark: this data is in the variable range of the RTK message
    and not in the RTK message header)
    *************************************************************************/

    /*************************************************************************
    SYNC byte
    *************************************************************************/
    pHdlcHandle->frame.SYNC = HDLC_SYNC_BYTE_PATTERN;
    /* Remark: the SNYC byte is not taken to calculate the FCS */

    /*************************************************************************
    ADDR byte
    the ADDR byte is relevant for the FCS.
    *************************************************************************/
    pHdlcHandle->frame.FCS = crctable[pHdlcHandle->frame.FCS ^ pHdlcHandle->frame.ADDR.value];

    /*************************************************************************
    CTRL byte
    the CTRL byte is relevant for the FCS.
    *************************************************************************/
    pHdlcHandle->frame.FCS = crctable[pHdlcHandle->frame.FCS ^ pHdlcHandle->frame.CTRL.value];

    /*************************************************************************
    LEN
    *************************************************************************/
    // LEN_L
    pHdlcHandle->frame.FCS = crctable[pHdlcHandle->frame.FCS ^ pHdlcHandle->frame.LEN_L.value];
    
    // if needed also LEN_H
    if(pHdlcHandle->frame.LEN_L.field.EA == 0)
    {
        pHdlcHandle->frame.FCS = crctable[pHdlcHandle->frame.FCS ^ pHdlcHandle->frame.LEN_H.value];
    }

    /*************************************************************************
    I-field
    array of u8 that contains the pay load.
    Length of array is v_Len.
    The I-field is NOT relevant for the FCS -8for a UIH frame).
    *************************************************************************/
    for (dataIndex = 0; dataIndex < HDLC_GET_LENGTH_INDICATOR(pHdlcHandle) ; dataIndex++)
    {
        // In case of UIH frame the I Field is excluded from FCS calculation.
        // Reference: chap 5.2.16 of ETSI TS 127 010 V8.0.0
        if (HDLC_GET_FRAME_TYPE(pHdlcHandle) != HDLC_CTRL_FRAME_TYPE_UIH)
        {
            pHdlcHandle->frame.FCS = crctable[pHdlcHandle->frame.FCS ^ pHdlcHandle->frame.pIfield[dataIndex]];
        }
    }

    /*************************************************************************
    FCS
    The frame check sum is calculated and stored as
    last byte to the output stream
    ************************************************************************/
    pHdlcHandle->frame.FCS = (hdlc_FCS_t)(0xFF-pHdlcHandle->frame.FCS);

    
    /*************************************************************************
    FLAG
    The closing FLAG. 
    Note: The closing flag may also be the opening flag of the following frame. 
          This "feature" is not supported by this implementation. 
    ************************************************************************/
    pHdlcHandle->frame.CLOSING_FLAG = HDLC_SYNC_BYTE_PATTERN;
        
   /*************************************************************************
    the frame is ready to be sent.
    *************************************************************************/
}

/*****************************************************************************
  Function:     hdlc_Transmit
 *****************************************************************************
  Remarks:      see include file for details
*****************************************************************************/
void hdlc_Transmit( hdlc_frameHandle_t*  pHdlcHandle,
                    hdlc_DLCI_t          dlci,
                    hdlc_addrCR_t        commandResponse,
                    hdlc_ctrlFrameType_t frameType,
                    sizeT                dataLen)
{
    u16 sendByteIndex;

    hdlc_fillL2Info(pHdlcHandle,dlci,commandResponse,frameType,dataLen);
    hdlc_fillL1Info(pHdlcHandle);

    if(pHdlcHandle->f_uartTxPutChar == NULL)
    {
        return;
    }

    pHdlcHandle->f_uartTxPutChar(pHdlcHandle->frame.SYNC);
    pHdlcHandle->f_uartTxPutChar(pHdlcHandle->frame.ADDR.value);
    pHdlcHandle->f_uartTxPutChar(pHdlcHandle->frame.CTRL.value);
    pHdlcHandle->f_uartTxPutChar(pHdlcHandle->frame.LEN_L.value);
    if(pHdlcHandle->frame.LEN_L.field.EA == 0)
    {
         pHdlcHandle->f_uartTxPutChar(pHdlcHandle->frame.LEN_H.value);
    }

    for(sendByteIndex = 0;
        sendByteIndex < HDLC_GET_LENGTH_INDICATOR(pHdlcHandle);
        sendByteIndex++)
    {
        pHdlcHandle->f_uartTxPutChar(pHdlcHandle->frame.pIfield[sendByteIndex]);
    }

    pHdlcHandle->f_uartTxPutChar(pHdlcHandle->frame.FCS);
    
    pHdlcHandle->f_uartTxPutChar(pHdlcHandle->frame.CLOSING_FLAG);    
}

/*****************************************************************************
  Function:     hdlc_fillL2Info
 *****************************************************************************
  Remarks:      see include file for details
*****************************************************************************/
void hdlc_fillL2Info( hdlc_frameHandle_t*  pHdlcHandle,
                      hdlc_DLCI_t          dlci,
                      hdlc_addrCR_t        commandResponse,
                      hdlc_ctrlFrameType_t frameType,
                      sizeT                dataLen)
{

    /*************************************************************************
    ADDR byte    
    *************************************************************************/
    
    // ADDR
    // - DLCI
    pHdlcHandle->frame.ADDR.field.DLCI = dlci;
    // - End bit. no more ADDR bytes follow. From standard assumed to be 1. 
    //   For details see end of chapter 5.2.1.2 of ETSI TS 127 010 V8.0.0.
    pHdlcHandle->frame.ADDR.field.EA   = 1;
    // - Command response: maybe to look more close for correct handling
    pHdlcHandle->frame.ADDR.field.CR   = (u8)commandResponse;

   
    /*************************************************************************
    New: CTRL byte
    Remark: coding accoring 3GPP 27.010. This CTRL value is fixed and will
    create only UI or UIH frames (unnumbered information with header and body check).
    *************************************************************************/
    pHdlcHandle->frame.CTRL.value = (u8)frameType;
    pHdlcHandle->frame.CTRL.field.PF = 1;

    /*************************************************************************
    LEN_L and LEN_H byte.
    *************************************************************************/

    if(dataLen > HDLC_LEN_L_MAX_NUM_OF_BYTES)
    {
        // We need the extended length
        pHdlcHandle->frame.LEN_L.field.EA = 0;
        
        pHdlcHandle->frame.LEN_L.field.lenL = 
            (u8)(dataLen & HDLC_LEN_L_MAX_NUM_OF_BYTES);
        
        pHdlcHandle->frame.LEN_H.field.lenH = (u8)(dataLen>>7);            
    }
    else
    {
        // We don't need the extended length
        pHdlcHandle->frame.LEN_L.field.EA = 1;
        pHdlcHandle->frame.LEN_L.field.lenL = (u8)dataLen;
        pHdlcHandle->frame.LEN_H.field.lenH = 0;
    }
    
}

/*****************************************************************************
                        End of Module
*****************************************************************************/

