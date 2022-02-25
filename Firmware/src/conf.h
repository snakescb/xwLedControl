/***************************************************************************//**
 * @file        conf.h
 * @author      Created: CLU
 * @brief       device configuration driver
 ******************************************************************************/
#ifndef CONF_H
#define CONF_H

/******************************************************************************
 Imported Include References
******************************************************************************/
#include "common.h"

#define CONFIG_TYPE_XWLEDCONTROL  1

#pragma pack(1)
typedef struct {
    uint8_t  modeSelection;
    uint8_t  batteryMinVoltage;
} boardConfig_t;
#pragma pack()

/******************************************************************************
 make pointer to configuration array visible
******************************************************************************/
extern const uint8_t* configBaseFlash;
extern const uint8_t* configBaseBackup;
extern const uint16_t configMaxFrameSize;
extern uint32_t configConfigSize;
extern uint16_t configConfigSizeKB;
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
 * @brief   speichert die übergebene anzahl bytes ab dem aktuellen pointer
 *          der pointer wird um die anzahl bytes inkrementiert
 *
 * @return  true wenn schreiben ok war, false falls ein fehler aufgetreten ist
 *****************************************************************************/
bool config_write(uint8_t numBytes, uint8_t* pData);

/**************************************************************************//**
 * @brief   Gibt einen Pointer auf den aktuellen read pointer zurück. Der
 *          Pointer wird danach um die anzahl bytes inkrementiert
 *
 * @return  pointer auf die aktuell zu lesenden daten. Falls die anzahl bytes
 *          zu gross ist oder ein byte des angefordeten blocks ausserhalb des
 *          konfig-Bereichs liegt, wird null zurückgegeben
 *****************************************************************************/
uint8_t* config_readBytes(uint8_t numBytes);

#endif //CONF_H

