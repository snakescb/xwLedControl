/***************************************************************************//**
 * @file        hdlc.h
 * @author      Created: DWY
 *              $Author: CLU $
 *              $LastChangedRevision: 440 $
 *              $LastChangedDate: 2010-05-20 09:59:35 +0200 (Do, 20 Mai 2010) $
 * @brief       HDLC module.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @defgroup    HDLC HDLC services
 *
 *  This protocol provides a light solution to fulfill the those requirements:
 *  - Do not load the data stream with large header functions
 *   (less header information shortens the amount of data transferred)
 *  - Do not use hardware handshake and software handshake.
 *  - Offer a resynchronization to the stream if sync is lost.
 *  - The external host must be fast enough to service all the data because
 *    there is no handshake.
 *  - The physical connection, the packet structure, the synchronization
 *    search algorithm and the packet types are fully symmetrical and therefore
 *    identical for both directions.
 *
 *  Those requirements can be fulfilled by using the HDLC frame protocol
 *  as described in ETSI TS 127 010 V8.0.0.\n
 *  The complete protocol is an overkill for the current application,
 *  Therefore only the HDLC frame structure is used. To ease the "basic" frame.
 */
/*@{
 ******************************************************************************/

#ifndef HDLC_H
#define HDLC_H


/******************************************************************************
 Imported Include References
******************************************************************************/
#include "common.h"

 #ifndef BOOL
 typedef bool BOOL;
 #endif

 // for Kleinbaustelle if cplusplus there is no redefine of sizeT
 #ifndef sizeT
 typedef u32 sizeT;
 #endif


 #ifndef bitField8_t
 typedef unsigned char   bitField8_t;
 #endif

/*****************************************************************************
                             PUBLIC DEFINITIONS
*****************************************************************************/

/******************************************************************************
 Public Named Constants Definitions
******************************************************************************/
#define HDLC_I_FIELD_MAX_SIZE ((sizeT)0x7FFF)

/*****************************************************************************
 Public Type Definitions
*****************************************************************************/


/**************************************************************************//**
* @brief    HDLC Frame ADDR field
*
* The coding of this byte is as follow
  @verbatim
          -----------------------------------------------------------------
    Bit:  |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
          -----------------------------------------------------------------
          |                     DLCI                      |  CR   |  E/A  |
          ----------------------------------------------------------------- @endverbatim
*
******************************************************************************/
typedef struct
{
    /// The End / Append bit and is set always to 1 to show the end of data.\n
    /// In any case no more ADDR bytes follow, HDLC does not support it (see chap 5.2.1.2).
    bitField8_t EA    :1;
    /// The Command/Response bit and set always to 1 (simplification against specs).
    bitField8_t CR    :1;
    /// Value to define the used channel.
    bitField8_t DLCI  :6;
}hdlc_ADDR_t;

typedef union
{
    hdlc_ADDR_t field;
    u8          value;
}hdlc_ADDR_u;

/**************************************************************************//**
* @brief    HDLC Frame CTRL field
*
  @verbatim
          -----------------------------------------------------------------
    Bit:  |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
          -----------------------------------------------------------------
          |       Frame type      |  P/F  |           Frame type          |
          ----------------------------------------------------------------- @endverbatim
*
******************************************************************************/
typedef struct
{
    /// First part of Frame type
    bitField8_t frameTypesPartA :4;
    /// P/F is the Poll / Final bit.
    bitField8_t PF              :1;
    /// Second part of Frame type
    bitField8_t frameTypesPartB :3;
}hdlc_CTRL_t;

typedef union
{
    hdlc_CTRL_t field;
    u8          value;
}hdlc_CTRL_u;


/**************************************************************************//**
* @brief    HDLC Frame LEN_L field
*
* This field is defines as following\n
  @verbatim
          -----------------------------------------------------------------
    Bit:  |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
          -----------------------------------------------------------------
          |                          LEN_L                        |  E/A  |
          ----------------------------------------------------------------- @endverbatim
*
******************************************************************************/
typedef struct
{
    /// This defines if the Length is finished or the additional byte will follow.
    /// - 1 means no additional byte.
    /// - 0 means that the LEN_H will follow
    bitField8_t EA    :1;
    /// I Frame length Low.
    bitField8_t lenL  :7;
}hdlc_LENL_t;

typedef union
{
    hdlc_LENL_t field;
    u8          value;
}hdlc_LENL_u;

/**************************************************************************//**
* @brief    HDLC Frame LEN_H field
*
* This field is defines as following\n
  @verbatim
          -----------------------------------------------------------------
    Bit:  |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
          -----------------------------------------------------------------
          |                             LEN_H                             |
          ----------------------------------------------------------------- @endverbatim
*
******************************************************************************/
typedef struct
{
    /// This is the upper part of length
    bitField8_t lenH  :8;
}hdlc_LENH_t;

typedef union
{
    hdlc_LENH_t field;
    u8          value;
}hdlc_LENH_u;

/**************************************************************************//**
* @brief    HDLC Frame Check Sum
*
* This is calculated according to chap 5.2.1.6
* (for UI frames to all fields including data)
******************************************************************************/
typedef u8  hdlc_FCS_t;

/**************************************************************************//**
* @brief    HDLC Frame SYNC FLAG
*
*  This is the sync pattern. It might also occur as data.
*  This has to be taken into account if the receiver falls out of sync.
*  It enhances the time to re-sync. Because this application uses wires to for
*  communication a sync lost will happen very seldom and should not taken as
*  reason to blow up the coding to avoid a sync pattern character in the rest
*  of the stream.
******************************************************************************/
typedef u8  hdlc_SYNC_FLAG_t;

// length of I field
typedef u16 hdlc_LEN_t;
// DLCI
typedef u16 hdlc_DLCI_t;
// one char


/**************************************************************************//**
* @brief    This is an element of the I FIELD.
*
* The I_FIELD, this is the pay load (maximum length=(2^15)-1).
* Coding of the I-field depends on the DLCI. The coding is always symmetrical.
* This means the type used in one direction must be the same for the
* other direction.
******************************************************************************/
typedef u8  hdlc_data_t;


/**************************************************************************//**
* @brief    HDLC Frame Format
*
  @verbatim
  Byte:
  ---------------------------------------------------------------------------------
  |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |   9   |
  |   0   |   1   |   2   |   3   |   4   |  4+1  |  4+2  |  4+n  | 4+n+1 | 4+n+2 |
  ---------------------------------------------------------------------------------
  | FLAG  | ADDR  | CTRL  | LEN_L | LEN_H | PL[0] | PL[1] |PL[n-1]|  FCS  |  FLAG |
  | 0xF9  |       |       |       |       |         DATA          |       |  0xF9 |
  ---------------------------------------------------------------------------------@endverbatim
******************************************************************************/
typedef struct
{
    u8            SYNC;
    hdlc_ADDR_u   ADDR;
    hdlc_CTRL_u   CTRL;
    hdlc_LENL_u   LEN_L;
    hdlc_LENH_u   LEN_H;
    u8*           pIfield;
    hdlc_FCS_t    FCS;
    u8            CLOSING_FLAG;
} hdlc_frame_t;



/**************************************************************************//**
* @brief    maximal length possible using only LEN_L (7bits)
******************************************************************************/
#define HDLC_LEN_L_MAX_NUM_OF_BYTES ((u8)(0x7F))

/**************************************************************************//**
* @brief    maximal length possible using  LEN_L (7bits) and LEN_H (8bits)
******************************************************************************/
#define HDLC_I_FIELD_MAX_LENGTH  ((u16)(HDLC_LEN_L_MAX_NUM_OF_BYTES + (0xFF<<7)))



/**************************************************************************//**
* @brief    Control frame types
*
* From ETSI TS 127 010 V8.0.0 chap 5.2.1.3
*
* @note  Currently only HDLC_CTRL_FRAME_TYPE_UI and HDLC_CTRL_FRAME_TYPE_UIH
*        frames are supported.
******************************************************************************/
typedef enum
{
    /// 00101111 Set Asynchronous Balanced Mode
    HDLC_CTRL_FRAME_TYPE_SABM = 0x2F,
    /// 01100011 Unnumbered Acknowledgement
    HDLC_CTRL_FRAME_TYPE_UA = 0x63,
    /// 00001111 Disconnected Mode
    HDLC_CTRL_FRAME_TYPE_DM = 0x0F,
    /// 01000011 Disconnect
    HDLC_CTRL_FRAME_TYPE_DISC = 0x43,
    /// 11101111 Unnumbered information with header check
    HDLC_CTRL_FRAME_TYPE_UIH = 0xEF,
    /// 00000111 Unnumbered information (with header and body check)
    HDLC_CTRL_FRAME_TYPE_UI = 0x03
} hdlc_ctrlFrameType_t;

/**************************************************************************//**
* @brief    Control frame P/F bit
*
* From ETSI TS 127 010 V8.0.0 chap 5.2.1.3 and 5.4.4
*
* The poll (P) bit set to 1 shall be used by a station to solicit (poll) a
* response or sequence of responses from the other station.
* The final (F) bit set to 1 shall be used by a station to indicate the response
* frame transmitted as the result of a soliciting (poll) command.
* The poll/final (P/F) bit shall serve a function in both command frames and
* response frames. (In command frames, the P/F bit is referred to as the P bit.
* In response frames, it is referred to as the F bit).
* More informations can be found in chapter 5.4.4.1 and 5.4.4.2
******************************************************************************/
#define HDLC_CTRL_PF_BIT   ((u8)0x10)    // 00010000



typedef enum
{
    HDLC_ADDR_CR_RESPONSE = 0,
    HDLC_ADDR_CR_COMMAND
} hdlc_addrCR_t;



/**************************************************************************//**
* @brief    Definition of HDLC states
******************************************************************************/
typedef enum
{
    /// Lost the sync, wating for SYNC_FLAG
    HDLC_STATE_PHASE_SYNC_LOST,
    /// Waiting for DLCI field
    HDLC_STATE_PHASE_DLCI,
    /// Waiting for CTRL field
    HDLC_STATE_PHASE_CTRL,
    /// Waiting for LEN_L field
    HDLC_STATE_PHASE_LEN_L,
    /// Waiting for LEN_H field, not yet supported
    HDLC_STATE_PHASE_LEN_H,
    /// Waiting for DATA
    HDLC_STATE_PHASE_I_FIELD,
    /// Waiting for FCS field
    HDLC_STATE_PHASE_FCS,
    /// Waiting Closing FLAG
    HDLC_STATE_PHASE_CLOSING_FLAG,
    /// a full and valid frame is recived
    HDLC_STATE_PHASE_VALID_FRAME
}hdlc_state_t;




/**************************************************************************//**
* @brief    uartTxPutChar function prototype pointer
******************************************************************************/
typedef BOOL (*f_uartTxPutChar_t)(u8 ch);

/**************************************************************************//**
* @brief    HDLC Frame Handle
******************************************************************************/
typedef struct
{
    /// HDLC state
    hdlc_state_t state;
    /// Maximal length of the iField allocated memory
    u16          iFieldMaxLength;
    /// Count of the data available on the iField
    u16          iFieldCount;
    /// The function that has to be used to transmit
    f_uartTxPutChar_t f_uartTxPutChar;
    /// Frame data
    hdlc_frame_t frame;
}hdlc_frameHandle_t;

/******************************************************************************
 Public Macro Definitions
******************************************************************************/


/******************************************************************************
 Public procedures
******************************************************************************/


/**************************************************************************//**
* @brief    Initialize the HDLC handle.
*
* @param    pHdlcHandle      address of the handle to be initialized.\n
*                            Allowed values: all exepted NULL.
* @param    pIField          pointer to the data memory.\n
*                            Allowed values: all including NULL (in case of frames without I field.)
* @param    iFieldMaxLength  The maximal length needed.\n
*                            Allowed values: from 0 to HDLC_I_FIELD_MAX_LENGTH
* @param    f_uartTxPutChar  address of the function pointer used to transmit.\n
*                            Allowed values: all including NULL (in case HDLC is used only for RX)
*
* @return   -TRUE if init successful\n
*           -FALSE if not successfull
******************************************************************************/
BOOL hdlc_handleInit(hdlc_frameHandle_t* pHdlcHandle,
                     u8*                 pIField,
                     u16                 iFieldMaxLength,
                     f_uartTxPutChar_t   f_uartTxPutChar);


/**************************************************************************//**
* @brief    Function that computes the data and evaluates the frame validity.
*
* @param    pHdlcHandle  address of the handle to be initialized.\n
*                        Allowed values: all exepted NULL.
*
* @param    data  The byte to consume.\n
*                 Allowed values from 0x00 to 0xFF.
*
* @return   BOOL  TRUE if a complete and valid frame is received
*                 FALSE for all the other cases
******************************************************************************/
BOOL hdlc_collectAndComputeData(hdlc_frameHandle_t* pHdlcHandle,
                                hdlc_data_t         data);


/**************************************************************************//**
* @brief    Function that computes the L1 info.
*
* @param    pHdlcHandle: address of the handle to be initialized.\n
*                        Allowed values: all exepted NULL.
******************************************************************************/
void hdlc_fillL1Info(hdlc_frameHandle_t* pHdlcHandle);


/**************************************************************************//**
* @brief    Function that computes the L2 info.
*
* @param    pHdlcHandle      address of the handle to be initialized.\n
*                            Allowed values: all exepted NULL.
* @param    dlci             The wanted dlci.
* @param    commandResponse  Defines if it is a command or response.
* @param    frameType        Currently only supported HDLC_CTRL_FRAME_TYPE_UIH
*                            and HDLC_CTRL_FRAME_TYPE_UI frames.
* @param    dataLen          The length of the data.\n
*                            Allowed values: from 0x00 to the smallest value
*                            between pHdlcHandle.iFieldMaxLength and
*                            HDLC_I_FIELD_MAX_LENGTH.
******************************************************************************/
void hdlc_fillL2Info( hdlc_frameHandle_t*  pHdlcHandle,
                      hdlc_DLCI_t          dlci,
                      hdlc_addrCR_t        commandResponse,
                      hdlc_ctrlFrameType_t frameType,
                      sizeT                dataLen);

void hdlc_Transmit( hdlc_frameHandle_t*  pHdlcHandle,
                      hdlc_DLCI_t          dlci,
                      hdlc_addrCR_t        commandResponse,
                      hdlc_ctrlFrameType_t frameType,
                      sizeT                dataLen);


#ifdef __cplusplus
 }
#endif
#endif /* KDLC_H */

 /** @} */ //defgroup
/*****************************************************************************
                End of Module
*****************************************************************************/

