using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using System.IO;
using System.Threading;

namespace xwLedConfigurator {
    class Bootloader {

        public string serialnumber;
        public int flashsize;
        public byte blversion_major;
        public byte blversion_minor;
        public float codeSize;
        public int numPages;
        public bool massErase;
        public int numFlashCommandsSP;
        public int numFlashCommandsPV;

        public enum loaderstate_t {
            IDLE = 0,
            BUSY,
            SUCCESS,
            ERROR
        }

        bool extendedErase;

        public loaderstate_t state;

        public Bootloader(ComPort port) {
            com = port;
            state = loaderstate_t.IDLE;
        }
        public void connect() {
            state = loaderstate_t.BUSY;
            Thread worker = new Thread(tConnect);
            worker.Start();
        }
        public void flash() {
            state = loaderstate_t.BUSY;
            Thread worker = new Thread(tBurn);
            worker.Start();
        }

        const byte RSP_ACK = 0x79;
        const byte RSP_NACK = 0x1F;
        const byte CMD_DISCOVER = 0x7F;
        const byte CMD_GET = 0x00;
        const byte CMD_READ = 0x11;
        const byte CMD_WRITE = 0x31;
        const byte CMD_ERASE = 0x43;
        const byte CMD_EXTENDED_ERASE = 0x44;
        const byte CMD_GO = 0x21;
        const int  DISCOVER_TIMEOUT_DEFAULT = 500;
        const uint MAX_DATA_PER_FRAME = 128;
        const uint STM32_FLASH_BASE_ADDRESS = 0x08000000;
        const uint STM32_PAGE_SIZE = 1024;

        ArrayList flashDataList;
        ArrayList pageList;
        uint addressOffset;
        flashData currentCommand;
        static ComPort com;

        class loaderCommand {

            public enum commandState_t {
                IDLE = 0,
                BUSY,
                OK,
                TIMEOUT
            }
            public int txLen = 0;
            public int rxLen = 0;
            public int timeout = 0;
            public byte[] txData = new byte[256];
            public byte[] rxData = new byte[256];
            public commandState_t state = commandState_t.IDLE;
            public bool busy = false;
            public uint rxCounter;

            System.Timers.Timer timeoutTimer = new System.Timers.Timer();

            public loaderCommand() {
                timeoutTimer.AutoReset = true;
                timeoutTimer.Elapsed += timeout_callback;
            }
            public loaderCommand(byte[] commandData, int rxLength, int maxTime, bool addChecksum) {
                timeoutTimer.AutoReset = true;
                timeoutTimer.Elapsed += timeout_callback;

                txLen = commandData.Length;
                byte checksum = 0x00;
                for(int i = 0; i < txLen; i++) {
                    txData[i] = commandData[i];
                    checksum = (byte)(checksum ^ commandData[i]);
                }
                rxLen = rxLength;
                timeout = maxTime;

                if(addChecksum) {
                    if(txLen > 1) txData[txLen++] = (byte)checksum;
                    else txData[txLen++] = (byte)~checksum;
                }

                start();
            }
            public void start() {
                state = commandState_t.BUSY;
                busy = true;               

                timeoutTimer.Interval = timeout;
                timeoutTimer.Enabled = true;
                rxCounter = 0;

                //send data
                com.purge();
                com.rxData += dataRx;
                com.send(txData, txLen);
            }
            void dataRx(byte[] data) {

                for(int i = 0; i < data.Length; i++) rxData[rxCounter++] = data[i];

                //enough bytes received?
                if(rxCounter >= rxLen) {
                    timeoutTimer.Enabled = false;
                    state = commandState_t.OK;
                    com.purge();
                    com.rxData -= dataRx;
                    busy = false;
                }
            }
            void timeout_callback(object sender, EventArgs e) {
                state = commandState_t.TIMEOUT;
                com.purge();
                com.rxData -= dataRx;
                busy = false;
            }
        }
        class flashData {
            public uint address;
            public uint length;
            public byte[] data;

            public flashData() {
                address = 0;
                data = new byte[256];
                length = 0;
            }
        }

        void tConnect() {
            extendedErase = false;
            byte[] d = new byte[1];
            d[0] = CMD_DISCOVER;
            loaderCommand c = new loaderCommand(d, 1, DISCOVER_TIMEOUT_DEFAULT, false);
            while(c.busy) Thread.Sleep(1);
            if(c.state == loaderCommand.commandState_t.TIMEOUT) {
                state = loaderstate_t.ERROR;
                return;
            }
            else {
                //check response
                if(c.rxData[0] != RSP_ACK) {                    
                    state = loaderstate_t.ERROR;
                    return;
                }
                else {
                    //ok
                    //get command, read version
                    c = new loaderCommand(new byte[] { CMD_GET }, 3, 1000, true);
                    while(c.busy) Thread.Sleep(1);
                    if(c.state == loaderCommand.commandState_t.OK) {
                        blversion_major = (byte)((byte)(c.rxData[2] >> 4) & 0x0F);
                        blversion_minor = (byte)(c.rxData[2] & 0x0F);

                        if(c.rxData[9] == 0x44) extendedErase = true;

                        //read serial number
                        byte[] data = new byte[12];
                        bool readOK = readMemory(0x1FFFF7E8, 12, ref data);

                        //state = loaderstate_t.SUCCESS;
                        //return;


                        if (readOK) {
                            serialnumber = BitConverter.ToString(data).Replace("-", string.Empty);
                            //read memory size
                            data = new byte[2];
                            readOK = readMemory(0x1FFFF7E0, 2, ref data);
                            if(readOK) {
                                flashsize = (byte)data[0] + (256 * ((byte)data[1]));
                                state = loaderstate_t.SUCCESS;
                                return;
                            }
                            else {
                                //Thread.Sleep(DISCOVER_TIMEOUT_DEFAULT);
                                state = loaderstate_t.ERROR;
                                return;
                            }
                        }
                        else {
                            //Thread.Sleep(DISCOVER_TIMEOUT_DEFAULT);
                            state = loaderstate_t.ERROR;
                            return;
                        }
                    }
                    else {
                        //Thread.Sleep(DISCOVER_TIMEOUT_DEFAULT);
                        state = loaderstate_t.ERROR;
                        return;
                    }
                }
            }
        }
        void tBurn() {
            numFlashCommandsSP = flashDataList.Count;
            numFlashCommandsPV = 0;

            //flash erase
            if(extendedErase) {
                loaderCommand c = new loaderCommand(new byte[] { CMD_EXTENDED_ERASE }, 1, 500, true);
                while(c.busy) Thread.Sleep(1);
                if((c.state == loaderCommand.commandState_t.TIMEOUT) || (c.rxData[0] != RSP_ACK)) {
                    state = loaderstate_t.ERROR;
                    return;
                }
                if(massErase) {
                    c = new loaderCommand(new byte[] { 0xFF, 0xFF }, 2, 2000, true);
                }
                else {
                    byte[] cmd = new byte[2 + pageList.Count*2];
                    int cnt = 0;
                    cmd[cnt++] = 0;
                    cmd[cnt++] = (byte)(pageList.Count - 1);
                    for(int i = 0; i < pageList.Count; i++) {
                        cmd[cnt++] = 0;
                        cmd[cnt++] = (byte)(pageList[i]);
                    }
                    c = new loaderCommand(cmd, 1, 2000, true);
                }
                while(c.busy) Thread.Sleep(1);
                if((c.state == loaderCommand.commandState_t.TIMEOUT) || (c.rxData[0] != RSP_ACK)) {
                    state = loaderstate_t.ERROR;
                    return;
                }
            }
            else {
                loaderCommand c = new loaderCommand(new byte[] { CMD_ERASE }, 1, 500, true);
                while(c.busy) Thread.Sleep(1);
                if((c.state == loaderCommand.commandState_t.TIMEOUT) || (c.rxData[0] != RSP_ACK)) {
                    state = loaderstate_t.ERROR;
                    return;
                }
                if(massErase) {
                    c = new loaderCommand(new byte[] { 0xFF }, 1, 2000, true);
                }
                else {
                    byte[] cmd = new byte[1 + pageList.Count];
                    cmd[0] = (byte)(pageList.Count - 1);
                    for(int i = 0; i < pageList.Count; i++) cmd[i + 1] = (byte)(pageList[i]);
                    c = new loaderCommand(cmd, 1, 2000, true);
                }
                while(c.busy) Thread.Sleep(1);
                if((c.state == loaderCommand.commandState_t.TIMEOUT) || (c.rxData[0] != RSP_ACK)) {
                    state = loaderstate_t.ERROR;
                    return;
                }
            }

            //write data
            bool writeOk;
            foreach(flashData d in flashDataList) {
                writeOk = writeMemory(d.address, d.data, (int)d.length);
                numFlashCommandsPV++;
                if(!writeOk) {
                    state = loaderstate_t.ERROR;
                    return;
                }
            }

            //send go
            loaderCommand command = new loaderCommand(new byte[] { CMD_GO }, 1, 100, true);
            while(command.busy) Thread.Sleep(1);
            command = new loaderCommand(new byte[] { 0x08, 0x00, 0x00, 0x00 }, 1, 100, true);
            while(command.busy) Thread.Sleep(1);

            state = loaderstate_t.SUCCESS;

        }

        bool readMemory(uint address, uint numBytes, ref byte[] rsp) {
            //send command, wait for ack
            loaderCommand c = new loaderCommand(new byte[] { CMD_READ }, 1, 500, true);
            while(c.busy) Thread.Sleep(1);
            if((c.state == loaderCommand.commandState_t.TIMEOUT) || (c.rxData[0] != RSP_ACK)) return false;

            //send address, wait for ack
            byte[] addr = new byte[4];
            addr[0] = (byte)((byte)(address >> 24) & 0xFF);
            addr[1] = (byte)((byte)(address >> 16) & 0xFF);
            addr[2] = (byte)((byte)(address >> 8) & 0xFF);
            addr[3] = (byte)(address & 0xFF);
            c = new loaderCommand(addr, 1, 500, true);
            while(c.busy) Thread.Sleep(1);
            if((c.state == loaderCommand.commandState_t.TIMEOUT) || (c.rxData[0] != RSP_ACK)) return false;

            //send number of bytes to read, wait for ack and data
            c = new loaderCommand(new byte[] { (byte)(numBytes - 1) }, (byte)(numBytes + 1), 500, true);
            while(c.busy) Thread.Sleep(1);
            if((c.state == loaderCommand.commandState_t.TIMEOUT) || (c.rxData[0] != RSP_ACK)) return false;

            //copy data
            for(int i = 0; i < numBytes; i++) rsp[i] = c.rxData[i + 1];

            //done
            return true;
        }
        bool writeMemory(uint address, byte[] data, int length) {
            //send command, wait for ack
            loaderCommand c = new loaderCommand(new byte[] { CMD_WRITE }, 1, 500, true);
            while(c.busy) Thread.Sleep(1);
            if((c.state == loaderCommand.commandState_t.TIMEOUT) || (c.rxData[0] != RSP_ACK)) return false;

            //send address, wait for ack
            byte[] addr = new byte[4];
            addr[0] = (byte)((byte)(address >> 24) & 0xFF);
            addr[1] = (byte)((byte)(address >> 16) & 0xFF);
            addr[2] = (byte)((byte)(address >> 8) & 0xFF);
            addr[3] = (byte)(address & 0xFF);
            c = new loaderCommand(addr, 1, 500, true);
            while(c.busy) Thread.Sleep(1);
            if((c.state == loaderCommand.commandState_t.TIMEOUT) || (c.rxData[0] != RSP_ACK)) return false;


            //send number of bytes to write, and data
            byte[] cmd = new byte[length + 1];
            cmd[0] = (byte)(length - 1);
            for(int i = 0; i < length; i++) cmd[i + 1] = data[i];
            c = new loaderCommand(cmd, 1, 500, true);
            while(c.busy) Thread.Sleep(1);
            if((c.state == loaderCommand.commandState_t.TIMEOUT) || (c.rxData[0] != RSP_ACK)) return false;

            //done
            return true;
        }
        void addPageToList(uint address) {
            uint pageNumber = (uint)((address - STM32_FLASH_BASE_ADDRESS) / STM32_PAGE_SIZE);
            //check if number is already in list
            bool found = false;
            foreach(byte n in pageList) {
                if(n == pageNumber) {
                    found = true;
                    break;
                }
            }
            if(!found) pageList.Add((byte)pageNumber);
        }

        public bool parse(StreamReader inStream) {
            flashDataList = new ArrayList();
            pageList = new ArrayList();
            addressOffset = 0;
            currentCommand = null;
            int lineCount = 0;
            int byteCount = 0;
            IntelHex hex = new IntelHex();

            //start reading
            while(true) {
                IntelHexStructure s = hex.Read(inStream);
                lineCount++;
                if(s == null) return false;
                else {

                    switch(s.type) {
                        case 1: {
                            //end of file command
                            //add last command to list
                            if(currentCommand != null) if(currentCommand.length > 0) flashDataList.Add(currentCommand);

                            //create list of used pages
                            foreach(flashData c in flashDataList) {
                                addPageToList(c.address);
                                addPageToList(c.address + c.length - 1);
                            }

                            codeSize = ((float)byteCount) / 1024;
                            numPages = pageList.Count;

                            return true;
                        }

                        case 4: {
                            //address offset command
                            addressOffset = 0;
                            addressOffset += (uint)(s.data[0] << 24);
                            addressOffset += (uint)(s.data[1] << 16);
                            break;
                        }

                        case 5: {
                            //ignore linear start address
                            break;
                        }

                        case 0: {
                            //data command
                            uint recordAddress = addressOffset + s.address;

                            //check if a new command structure is needed
                            bool newCmd = false;
                            if(currentCommand == null) newCmd = true;
                            else {
                                if(currentCommand.length + s.dataLen > MAX_DATA_PER_FRAME) newCmd = true;
                                if(currentCommand.address + currentCommand.length != recordAddress) newCmd = true;
                            }

                            //create new command if needed
                            if(newCmd) {
                                if(currentCommand != null) flashDataList.Add(currentCommand); ;
                                currentCommand = new flashData();
                                currentCommand.address = recordAddress;
                            }

                            //copy data to command
                            for(uint i = 0; i < s.dataLen; i++) {
                                currentCommand.data[currentCommand.length++] = s.data[i];
                                byteCount++;
                            }
                            break;
                        }

                        default: {
                            return false;
                        }
                    }
                }
            }
        }
    }
}
