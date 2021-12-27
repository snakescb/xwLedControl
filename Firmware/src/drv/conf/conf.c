/***************************************************************************//**
 * @file        conf.c
 * @author      Created: CLU
 *              $Author: CLU $
 *              $LastChangedRevision: 440 $
 *              $LastChangedDate: 2010-05-20 09:59:35 +0200 (Do, 20 Mai 2010) $
 * @brief       device configuration driver
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "conf.h"
#include "teco.h"
#include "crc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CONF_FLASH_BASE_ADDRESS   0x08000000
#define CONF_BOARD_CONFIG_PAGE            32
#define CONF_FIRST_PAGE                   33
#define CONF_FLASH_PAGESIZE             1024
#define CONF_FRAME_MAX_SIZE              128
#define CONF_CONFIG_START_ADDRESS  CONF_FLASH_BASE_ADDRESS + (CONF_FIRST_PAGE*CONF_FLASH_PAGESIZE)

const u8 backupConfig[] = {
    0x20, 0x08, 0x00, 0x00, 0x04, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x28, 0x00, 0x00, 0x00, 0xD4, 0x00, 0x00, 0x00,
    0x80, 0x01, 0x00, 0x00, 0xEC, 0x01, 0x00, 0x00,
    0x46, 0x41, 0x49, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x06, 0xFF, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x98, 0x02, 0x00, 0x00,
    0xB0, 0x02, 0x00, 0x00, 0xD4, 0x02, 0x00, 0x00,
    0xF8, 0x02, 0x00, 0x00, 0x34, 0x03, 0x00, 0x00,
    0x7C, 0x03, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00,
    0x00, 0x01, 0x21, 0x00, 0x00, 0x02, 0x22, 0x00,
    0x00, 0x03, 0x23, 0x00, 0x00, 0x04, 0x24, 0x00,
    0x00, 0x05, 0x25, 0x00, 0x42, 0x31, 0x20, 0x4C,
    0x45, 0x44, 0x20, 0x31, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x31, 0x20, 0x4C, 0x45, 0x44, 0x20, 0x32,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x31, 0x20, 0x4C,
    0x45, 0x44, 0x20, 0x33, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x31, 0x20, 0x4C, 0x45, 0x44, 0x20, 0x34,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x31, 0x20, 0x4C,
    0x45, 0x44, 0x20, 0x35, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x31, 0x20, 0x4C, 0x45, 0x44, 0x20, 0x36,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x32,
    0x00, 0x00, 0x05, 0x32, 0x00, 0x00, 0x05, 0x32,
    0x00, 0x00, 0x05, 0x32, 0x00, 0x00, 0x05, 0x32,
    0x00, 0x00, 0x05, 0x32, 0x4B, 0x6E, 0x69, 0x67,
    0x68, 0x74, 0x52, 0x69, 0x64, 0x65, 0x72, 0x00,
    0x06, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x94, 0x03, 0x00, 0x00, 0xDC, 0x03, 0x00, 0x00,
    0x3C, 0x04, 0x00, 0x00, 0xB4, 0x04, 0x00, 0x00,
    0x2C, 0x05, 0x00, 0x00, 0x98, 0x05, 0x00, 0x00,
    0x00, 0x00, 0x20, 0x00, 0x00, 0x01, 0x21, 0x00,
    0x00, 0x02, 0x22, 0x00, 0x00, 0x03, 0x23, 0x00,
    0x00, 0x04, 0x24, 0x00, 0x00, 0x05, 0x25, 0x00,
    0x42, 0x31, 0x20, 0x4C, 0x45, 0x44, 0x20, 0x31,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x31, 0x20, 0x4C,
    0x45, 0x44, 0x20, 0x32, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x31, 0x20, 0x4C, 0x45, 0x44, 0x20, 0x33,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x31, 0x20, 0x4C,
    0x45, 0x44, 0x20, 0x34, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x31, 0x20, 0x4C, 0x45, 0x44, 0x20, 0x35,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x31, 0x20, 0x4C,
    0x45, 0x44, 0x20, 0x36, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x05, 0x32, 0x00, 0x00, 0x05, 0x32,
    0x00, 0x00, 0x05, 0x32, 0x00, 0x00, 0x05, 0x32,
    0x00, 0x00, 0x05, 0x32, 0x00, 0x00, 0x05, 0x32,
    0x52, 0x47, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x06, 0xFF, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xE0, 0x05, 0x00, 0x00,
    0x28, 0x06, 0x00, 0x00, 0x70, 0x06, 0x00, 0x00,
    0xB8, 0x06, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
    0x48, 0x07, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00,
    0x00, 0x01, 0x40, 0x00, 0x00, 0x02, 0x80, 0x00,
    0x00, 0x03, 0x21, 0x00, 0x00, 0x04, 0x41, 0x00,
    0x00, 0x05, 0x81, 0x00, 0x42, 0x31, 0x20, 0x52,
    0x47, 0x42, 0x20, 0x31, 0x2D, 0x33, 0x00, 0x00,
    0x42, 0x31, 0x20, 0x52, 0x47, 0x42, 0x20, 0x34,
    0x2D, 0x36, 0x00, 0x00, 0x00, 0x00, 0x05, 0x32,
    0x00, 0x00, 0x05, 0x32, 0x44, 0x69, 0x6D, 0x6D,
    0x65, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x06, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x90, 0x07, 0x00, 0x00, 0xA8, 0x07, 0x00, 0x00,
    0xC0, 0x07, 0x00, 0x00, 0xD8, 0x07, 0x00, 0x00,
    0xF0, 0x07, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00,
    0x00, 0x00, 0x20, 0x00, 0x00, 0x01, 0x21, 0x00,
    0x00, 0x02, 0x22, 0x00, 0x00, 0x03, 0x23, 0x00,
    0x00, 0x04, 0x24, 0x00, 0x00, 0x05, 0x25, 0x00,
    0x42, 0x31, 0x20, 0x4C, 0x45, 0x44, 0x20, 0x31,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x31, 0x20, 0x4C,
    0x45, 0x44, 0x20, 0x32, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x31, 0x20, 0x4C, 0x45, 0x44, 0x20, 0x33,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x31, 0x20, 0x4C,
    0x45, 0x44, 0x20, 0x34, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x31, 0x20, 0x4C, 0x45, 0x44, 0x20, 0x35,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x31, 0x20, 0x4C,
    0x45, 0x44, 0x20, 0x36, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x05, 0x32, 0x00, 0x00, 0x05, 0x32,
    0x00, 0x00, 0x05, 0x32, 0x00, 0x00, 0x05, 0x32,
    0x00, 0x00, 0x05, 0x32, 0x00, 0x00, 0x05, 0x32,
    0x02, 0xFF, 0xFF, 0x00, 0xD0, 0x07, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0xF0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
    0x03, 0xFF, 0x00, 0x32, 0x2C, 0x01, 0x96, 0x00,
    0xFF, 0xFF, 0x00, 0x02, 0x01, 0x00, 0xB0, 0x04,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x05, 0x03, 0xFF, 0x00, 0x32,
    0xF4, 0x01, 0x96, 0x00, 0xFF, 0xFF, 0x00, 0x02,
    0x01, 0x00, 0x84, 0x03, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
    0x02, 0x00, 0xFF, 0x00, 0xC8, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x02, 0xFF, 0xFF, 0x00,
    0x2C, 0x01, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0x02, 0xFF, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x01, 0x00, 0x4C, 0x04,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x05, 0x01, 0x00, 0x84, 0x03,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x00, 0xFF, 0x00, 0xC8, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x02, 0xFF, 0xFF, 0x00,
    0x2C, 0x01, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0x02, 0xFF, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x01, 0x00, 0xC8, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x05, 0x04, 0x00, 0xFF, 0x00,
    0xD0, 0x07, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x03,
    0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x05, 0x02, 0xFF, 0xFF, 0x00,
    0x64, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0x02, 0xFF, 0x00, 0x00, 0x90, 0x01, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x01, 0x00, 0x20, 0x03,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x00, 0xF7, 0x00, 0x64, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x02, 0xFF, 0xFF, 0x00,
    0x90, 0x01, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x05, 0x02, 0x00, 0xF7, 0x00,
    0x64, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0x02, 0xFF, 0xFF, 0x00, 0x64, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x02, 0xFF, 0x00, 0x00,
    0x90, 0x01, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0x01, 0x00, 0x58, 0x02, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0xF7, 0x00,
    0x64, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0x02, 0xFF, 0xFF, 0x00, 0x64, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x02, 0xFF, 0x00, 0x00,
    0x90, 0x01, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x05, 0x01, 0x00, 0x64, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x00, 0xF7, 0x00, 0x64, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x02, 0xFF, 0xFF, 0x00,
    0x64, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0x02, 0xFF, 0x00, 0x00, 0x90, 0x01, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x01, 0x00, 0x90, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x00, 0xF7, 0x00, 0x64, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x02, 0xFF, 0xFF, 0x00,
    0x64, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0x02, 0xFF, 0x00, 0x00, 0x90, 0x01, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x01, 0x00, 0x64, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x05, 0x01, 0x00, 0xC8, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x00, 0xF7, 0x00, 0x64, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x02, 0xFF, 0xFF, 0x00,
    0x64, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0x02, 0xFF, 0x00, 0x00, 0x90, 0x01, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x01, 0x00, 0xC8, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x00, 0xF7, 0x00, 0x64, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x02, 0xFF, 0xFF, 0x00,
    0x64, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0x02, 0xFF, 0x00, 0x00, 0x90, 0x01, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x01, 0x00, 0xC8, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x05, 0x01, 0x00, 0x2C, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x00, 0xF7, 0x00, 0x64, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x02, 0xFF, 0xFF, 0x00,
    0x64, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0x02, 0xFF, 0x00, 0x00, 0x90, 0x01, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x02, 0x00, 0xF7, 0x00,
    0x64, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0x02, 0xFF, 0xFF, 0x00, 0x64, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x02, 0xFF, 0x00, 0x00,
    0x90, 0x01, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0x01, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
    0x01, 0x00, 0x90, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0xF7, 0x00,
    0x64, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0x02, 0xFF, 0xFF, 0x00, 0xF4, 0x01, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x02, 0xFF, 0x00, 0x00,
    0x90, 0x01, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0x01, 0x00, 0x90, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
    0x02, 0x00, 0xFF, 0x00, 0xE8, 0x03, 0x00, 0x00,
    0x00, 0xFF, 0x00, 0x01, 0x02, 0xFF, 0xFF, 0x00,
    0xE8, 0x03, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0x02, 0xFF, 0x00, 0x00, 0xE8, 0x03, 0x00, 0x00,
    0xFF, 0x00, 0x00, 0x01, 0x02, 0x00, 0xFF, 0x00,
    0xE8, 0x03, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x01,
    0x02, 0xFF, 0x00, 0x00, 0xE8, 0x03, 0x00, 0x00,
    0xFF, 0x00, 0x00, 0x01, 0xF0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
    0x02, 0x00, 0x00, 0x00, 0xE8, 0x03, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0xFF, 0x00,
    0xE8, 0x03, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x01,
    0x02, 0xFF, 0xFF, 0x00, 0xE8, 0x03, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0x02, 0xFF, 0xFF, 0x00,
    0xE8, 0x03, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x01,
    0x02, 0xFF, 0x00, 0x00, 0xE8, 0x03, 0x00, 0x00,
    0xFF, 0x00, 0x00, 0x01, 0xF0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
    0x02, 0xFF, 0x00, 0x00, 0xE8, 0x03, 0x00, 0x00,
    0xFF, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00,
    0xE8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x02, 0x00, 0x00, 0x00, 0xE8, 0x03, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0xFF, 0x00,
    0xE8, 0x03, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x01,
    0x02, 0xFF, 0xFF, 0x00, 0xE8, 0x03, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x01, 0xF0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
    0x03, 0x00, 0xFF, 0x32, 0xE8, 0x03, 0x2C, 0x01,
    0x00, 0xFF, 0x00, 0x02, 0x03, 0x00, 0xFF, 0x32,
    0xE8, 0x03, 0xDE, 0x00, 0xFF, 0xFF, 0x00, 0x02,
    0x03, 0x7F, 0x00, 0x32, 0xE8, 0x03, 0xB4, 0x00,
    0xFF, 0x00, 0x00, 0x02, 0x03, 0x00, 0xFF, 0x32,
    0xE8, 0x03, 0xDE, 0x00, 0xFF, 0xFF, 0x00, 0x02,
    0x03, 0xFF, 0x00, 0x32, 0xE8, 0x03, 0x2C, 0x01,
    0xFF, 0x00, 0x00, 0x02, 0xF0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
    0x03, 0x00, 0x00, 0x32, 0xE8, 0x03, 0x2C, 0x01,
    0x00, 0x00, 0x00, 0x02, 0x03, 0x00, 0xFF, 0x32,
    0xE8, 0x03, 0xDE, 0x00, 0x00, 0xFF, 0x00, 0x02,
    0x03, 0x7F, 0xFF, 0x32, 0xE8, 0x03, 0xB4, 0x00,
    0xFF, 0xFF, 0x00, 0x02, 0x03, 0x00, 0xFF, 0x32,
    0xE8, 0x03, 0xDE, 0x00, 0xFF, 0xFF, 0x00, 0x02,
    0x03, 0xFF, 0x00, 0x32, 0xE8, 0x03, 0x2C, 0x01,
    0xFF, 0x00, 0x00, 0x02, 0xF0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
    0x03, 0xFF, 0x00, 0x32, 0xE8, 0x03, 0x2C, 0x01,
    0xFF, 0x00, 0x00, 0x02, 0x03, 0x00, 0x00, 0x32,
    0xE8, 0x03, 0xDE, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x03, 0x00, 0x00, 0x32, 0xE8, 0x03, 0xB4, 0x00,
    0x00, 0x00, 0x00, 0x02, 0x03, 0x00, 0xFF, 0x32,
    0xE8, 0x03, 0xDE, 0x00, 0xFF, 0xFF, 0x00, 0x02,
    0x03, 0xFF, 0xFF, 0x32, 0xE8, 0x03, 0x2C, 0x01,
    0xFF, 0xFF, 0x00, 0x02, 0xF0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
    0x04, 0x00, 0xFF, 0x00, 0xE8, 0x03, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x03, 0xF0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
    0x04, 0xFF, 0x00, 0x00, 0xE8, 0x03, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x03, 0xF0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
    0x04, 0x00, 0xFF, 0x00, 0xE8, 0x03, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x03, 0xF0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
    0x04, 0xFF, 0x00, 0x00, 0xE8, 0x03, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x03, 0xF0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
    0x04, 0x00, 0xFF, 0x00, 0xE8, 0x03, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x03, 0xF0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
    0x04, 0xFF, 0x00, 0x00, 0xE8, 0x03, 0x00, 0x00,
    0xFF, 0xFF, 0x00, 0x03, 0xF0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
    0x70, 0xFF
};


/* Private variables ---------------------------------------------------------*/
const u8* configBaseFlash = (u8*)((u32)CONF_CONFIG_START_ADDRESS);
const u8* configBaseBackup = (u8*)backupConfig;
const u16 configMaxFrameSize = CONF_FRAME_MAX_SIZE;

u32 configFlashSize;
u32 configConfigSize;
u16 configConfigSizeKB;
u16 configConfigNumPages;
u32 configTopAddress;

u32 confCurrentWriteAddress  = (u32)CONF_CONFIG_START_ADDRESS;
u32 confCurrentReadAddress   = (u32)CONF_CONFIG_START_ADDRESS;

bool configOk = false;

boardConfig_t boardConfig;

/* Private functions ---------------------------------------------------------*/
void conf_unlock() {
    __asm(" cpsid i\n");
    __asm(" cpsid f\n");
    FLASH_Unlock();
}

void conf_lock() {
    FLASH_Lock();
    __asm(" cpsie f\n");
    __asm(" cpsie i\n");
}

/*******************************************************************************
 * config_boardConfigRead
 ******************************************************************************/
bool config_boardConfigRead(void) {

    u32 addr = CONF_FLASH_BASE_ADDRESS + (CONF_BOARD_CONFIG_PAGE*CONF_FLASH_PAGESIZE);
    u16 crc  = *((u16*)addr);
    u16 byteLen  = sizeof(boardConfig_t);
    u16 wordLen  = (byteLen+1) >> 1;

    u16 crcCalc  = crc_calc((u8*)(addr+2), byteLen);
    if (crcCalc != crc) return false;

    u16* pC = (u16*)&boardConfig;
    u16* pF = (u16*) addr;

    for (u16 i=0; i<wordLen; i++) pC[i] = pF[i+1];

    return true;
}

/*******************************************************************************
 * config_boardConfigWrite
 ******************************************************************************/
void config_boardConfigWrite(void) {

    u32 addr = CONF_FLASH_BASE_ADDRESS + (CONF_BOARD_CONFIG_PAGE*CONF_FLASH_PAGESIZE);
    u16 byteLen  = sizeof(boardConfig_t);
    u16 wordLen  = (byteLen+1) >> 1;
    u16 crc = crc_calc((u8*)&boardConfig, byteLen);

    conf_unlock();
    for (u32 i=0; i<0x2FFFFF; i++);
    conf_unlock();

    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    FLASH_ErasePage(addr);
    FLASH_ProgramHalfWord(addr, crc);

    u16* p = (u16*)&boardConfig;

    for (u16 i=0; i<wordLen; i++) {
        addr += 2;
        FLASH_ProgramHalfWord(addr, p[i]);
    }

    conf_lock();
}

/*******************************************************************************
 * config_boardConfigFactoryDefault
 ******************************************************************************/
void config_boardConfigFactoryDefault(void) {

    boardConfig.modeSelection = 0;
    boardConfig.batteryMinVoltage = 0;
    boardConfig.receiverMin = 593;
    boardConfig.receiverCenter = 803;
    boardConfig.receiverMax = 1013;

    boardConfig.extPortSensorType = 0;
    boardConfig.extPortTriggerSource = 0;
    boardConfig.extPortTriggerInterval = 0;
    boardConfig.extPortParam1 = 0;
    boardConfig.extPortParam2 = 0;
    boardConfig.extPortIRCameraType = 0;

    config_boardConfigWrite();
}

/*******************************************************************************
 * config_erase
 ******************************************************************************/
bool config_erase(void) {

    conf_unlock();
    for (u32 i=0; i<0x2FFFFF; i++);
    conf_unlock();

    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    u16 startPage = CONF_FIRST_PAGE;
    u16 stopPage  = CONF_FIRST_PAGE + configConfigNumPages;

    for (u8 i=startPage; i<stopPage; i++) {
        u32 pageAddress = CONF_FLASH_BASE_ADDRESS + ((u32)i*CONF_FLASH_PAGESIZE);
        if (FLASH_ErasePage(pageAddress) != FLASH_COMPLETE) {
            conf_lock();
            return false;
        }
    }

    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    conf_lock();
    return true;
}

/*******************************************************************************
 * config_resetPointers
 ******************************************************************************/
void config_resetPointers(void) {
    confCurrentWriteAddress  = (u32)CONF_CONFIG_START_ADDRESS;
    confCurrentReadAddress   = (u32)CONF_CONFIG_START_ADDRESS;
}

/*******************************************************************************
 * config_write
 ******************************************************************************/
bool config_write(u8 numBytes, u8* pData) {

    //pr�fe die anzahl bytes
    if (numBytes > CONF_FRAME_MAX_SIZE) return false;
    if (numBytes % 2 != 0) return false;

    //pr�fe ob ganzer block innerhalb konfiguration liegt
    u32 lastWriteAddress = confCurrentWriteAddress + numBytes - 1;
    if (lastWriteAddress > configTopAddress) return false;

    conf_unlock();

    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    u8 numWords = numBytes >> 1;
    u16* pConfigData = (u16*)(pData);

    for (u8 i=0; i<numWords; i++) {
        //schreibe word ins flash
        if (FLASH_ProgramHalfWord(confCurrentWriteAddress, pConfigData[i]) != FLASH_COMPLETE) {
            conf_lock();
            return false;
        }

        //pr�fe ob word richtig geschrieben wurde
        u16* pTest = (u16*)confCurrentWriteAddress;
        if (*pTest != pConfigData[i]) {
            conf_lock();
            return false;
        }

        confCurrentWriteAddress += 2;
    }

    conf_lock();
    return true;
}

/*******************************************************************************
 * config_readBytes
 ******************************************************************************/
u8* config_readBytes(u8 numBytes) {

    if (numBytes > CONF_FRAME_MAX_SIZE) return null;
    if (numBytes % 2 != 0) return null;
    if (confCurrentReadAddress > configTopAddress) return null;
    if (confCurrentReadAddress + numBytes > configTopAddress) return null;

    u8* ret = (u8*)confCurrentReadAddress;
    confCurrentReadAddress += numBytes;
    return ret;
}


/*******************************************************************************
 * config_init
 ******************************************************************************/
void config_init(void) {

    u32 configFlashSizeKB;

    configFlashSizeKB    = (u32)(*((u16*)0x1ffff7e0));
    configFlashSize      = configFlashSizeKB << 10;
    configConfigSize     = configFlashSize - (CONF_FIRST_PAGE*CONF_FLASH_PAGESIZE);
    configConfigSizeKB   = configConfigSize >> 10;
    configConfigNumPages = configConfigSize / CONF_FLASH_PAGESIZE;
    configTopAddress     = CONF_CONFIG_START_ADDRESS + configConfigSize - 1;

}
