#ifndef XWCOM_H_INCLUDED
#define XWCOM_H_INCLUDED

//*****************************************************************************
// XWING GLOBAL DEFINITIONS
//*****************************************************************************
#define HW_VERSION_V1                             1
#define HW_VERSION_V2                             2
#define HW_VERSION_V3                             3
#define HW_VERSION_V4                             4

#define HW_TYPE_XWLEDCONTROL                      0
#define HW_TYPE_XWLEDCONTROL_PRO                  1

//*****************************************************************************
// XWING GLOBAL TYPEDEFS
//*****************************************************************************
	typedef enum {
        SCOPE_TRACE                           = 0x00 ,
        SCOPE_COMMAND                         = 0x01 ,
        SCOPE_CONFIG                          = 0x02 ,
        SCOPE_LED                             = 0x03 ,
	} dlci_e;

    typedef enum  {
        COMMAND_RESET                        = 0x01 ,
        COMMAND_GET_STATIC_INFO              = 0x02 ,
        COMMAND_GET_DYNAMIC_INFO             = 0x03 
    } query_command_e;

	typedef enum {
        COMMAND_RESPONSE_STATIC_INFO         = 0x01 ,
        COMMAND_RESPONSE_DYNAMIC_INFO        = 0x02 ,
        COMMAND_RESPONSE_VARIABLE_INFO       = 0x03 ,
	} response_command_e;

    typedef enum  {
        CONFIG_GET_FRAME_MAX_SIZE            = 0x01 ,
        CONFIG_ERASE_CONFIG                  = 0x02 ,
        CONFIG_RESET_POINTER                 = 0x03 ,
        CONFIG_SET_FRAME                     = 0x04 ,
        CONFIG_GET_FRAME                     = 0x05 ,
        CONFIG_SET_CONFIG                    = 0x06 ,
        CONFIG_GET_CONFIG                    = 0x07 ,
        CONFIG_REINITIALIZE                  = 0x08 ,        
        CONFIG_SET_FACTORY_DEFAULTS          = 0x09 ,
    } query_config_e;

	typedef enum {
        CONFIG_RESPONSE_FRAME_MAX_SIZE       = 0x01 ,
        CONFIG_RESPONSE_FRAME                = 0x02 ,
        CONFIG_RESPONSE_ACKNOWLEDGE          = 0x03 ,
        CONFIG_RESPONSE_CONFIG               = 0x04 ,
	} response_config_e;

    typedef enum  {
       LED_ENABLE_DISABLE                    = 0x01 ,
       LED_MANUAL_BRIGHTNESS                 = 0x02 ,
       LED_SIM_SET_OBJECTS                   = 0x03 ,
       LED_SIM_GET_BUFFER_INFO               = 0x04 ,
       LED_SIM_GET_BUFFER_STATE              = 0x05 ,
       LED_START_SIM                         = 0x06 ,
       LED_AUX_SIM                           = 0x07 ,
       LED_START_SIM_WITH_OFFSET             = 0x08 ,
       LED_SIM_SET_CHANNEL_OPTIONS           = 0x09 ,
    } query_led_e;

    typedef enum {
        LED_RESPONSE_ACKNOWLEDGE             = 0x01 ,
        LED_RESPONSE_BUFFER_INFO             = 0x02 ,
        LED_RESPONSE_BUFFER_STATE            = 0x03 ,
	} response_led_e;

#endif // XWCOM_H_INCLUDED
