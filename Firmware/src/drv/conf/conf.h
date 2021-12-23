/***************************************************************************//**
 * @file        conf.h
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 440 $
 *              $LastChangedDate: 2010-05-20 09:59:35 +0200 (Do, 20 Mai 2010) $
 * @brief       device configuration driver
 ******************************************************************************/
#ifndef CONF_H
#define CONF_H

/******************************************************************************
 Imported Include References
******************************************************************************/
#include "common.h"

#define CONFIG_TYPE_SKYLED  0

#pragma pack(1)
typedef struct {

    u8  modeSelection;
    u8  batteryMinVoltage;
    u16 receiverCenter;
    u16 receiverMax;
    u16 receiverMin;

    u16  extPortSensorType;
    u16  extPortTriggerSource;
    u16  extPortTriggerInterval;
    u16  extPortParam1;
    u16  extPortParam2;
    u16  extPortIRCameraType;

} boardConfig_t;
#pragma pack()

/******************************************************************************
 make pointer to configuration array visible
******************************************************************************/
extern const u8* configBaseFlash;
extern const u8* configBaseBackup;
extern const u16 configMaxFrameSize;
extern u32 configConfigSize;
extern u16 configConfigSizeKB;
extern boardConfig_t boardConfig;

/**************************************************************************//**
 * @brief    Initialisierung
 *****************************************************************************/
void config_init(void);

/**************************************************************************//**
 * @brief    setzt den lese und schreib pointer auf die startadresse zurück
 *****************************************************************************/
void config_resetPointers(void);

/**************************************************************************//**
 * @brief    löschte die gesamte konfiguration
 *
 * @return  true if erase completed, false on error
 *****************************************************************************/
bool config_erase(void);

/**************************************************************************//**
 * @brief    liest boardconfig
 *****************************************************************************/
bool config_boardConfigRead(void);

/**************************************************************************//**
 * @brief    schreibt boardconfig
 *****************************************************************************/
void config_boardConfigWrite(void);

/**************************************************************************//**
 * @brief    initialisiert boardconfig
 *****************************************************************************/
void config_boardConfigFactoryDefault(void);

/**************************************************************************//**
 * @brief   speicher die übergebene anzahl bytes ab dem aktuellen pointer
 *          der pointer wird um die anzahl bytes inkrementiert
 *
 * @return  true wenn schreiben ok war, false falls ein fehler aufgetreten ist
 *****************************************************************************/
bool config_write(u8 numBytes, u8* pData);

/**************************************************************************//**
 * @brief   Gibt einen Pointer auf den aktuellen read pointer zurück. Der
 *          Pointer wird danach um die anzahl bytes inkrementiert
 *
 * @return  pointer auf die aktuell zu lesenden daten. Falls die anzahl bytes
 *          zu gross ist oder ein byte des angefordeten blocks ausserhalb des
 *          konfig-Bereichs liegt, wird null zurückgegeben
 *****************************************************************************/
u8* config_readBytes(u8 numBytes);

#endif //CONF_H

