using System;
using System.Collections.Generic;
using System.Text;

namespace xwLedConfigurator {
    public static class xwCom {

        public static string[] deviceTypes = {
            "Unknown Device", //0
            "xwLedControl V1.0" //1
        };

        public static string[] runModes = {
            "Disabled", //0
            "Master", //1
            "Slave", //2
            "Simulation" //3
        };

        public enum SCOPE : byte {
            TRACE = 0x00,
            COMMAND = 0x01,
            CONFIG = 0x02,
            LED = 0x03
        }

        public enum COMMAND : byte {
            RESET = 0x01,
            GET_STATIC_INFO = 0x02,
            GET_DYNAMIC_INFO = 0x03
        }

        public enum COMMAND_RESPONSE : byte {
            RESPONSE_STATIC_INFO = 0x01,
            RESPONSE_DYNAMIC_INFO = 0x02,
            RESPONSE_VARIABLE_INFO = 0x03
        }

        public enum CONFIG : byte {
            GET_FRAME_MAX_SIZE = 0x01,
            ERASE_CONFIG = 0x02,
            RESET_POINTER = 0x03,
            SET_FRAME = 0x04,
            GET_FRAME = 0x05,
            SET_CONFIG = 0x06,
            GET_CONFIG = 0x07,
            REINITIALIZE = 0x08,
            SET_FACTORY_DEFAULTS = 0x09
        }

        public enum CONFIG_RESPONSE : byte {
            RESPONSE_FRAME_MAX_SIZE = 0x01,
            RESPONSE_FRAME = 0x02,
            RESPONSE_ACKNOWLEDGE = 0x03,
            RESPONSE_CONFIG = 0x04
        }
    }

}
