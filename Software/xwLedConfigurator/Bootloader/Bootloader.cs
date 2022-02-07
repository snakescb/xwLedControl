using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using System.IO;
using System.Threading;
using System.IO.Ports;

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
            IDLE = 0, BUSY, SUCCESS, ERROR
        }
        public loaderstate_t state;
        
        const byte RSP_ACK = 0x79;
        const byte CMD_DISCOVER = 0x7F;
        const byte CMD_GET = 0x00;
        const byte CMD_READ = 0x11;
        const byte CMD_WRITE = 0x31;
        const byte CMD_ERASE = 0x43;
        const byte CMD_GO = 0x21;
        const int  COMMAND_DEFAULT_TIMEOUT = 200;
        const uint MAX_DATA_PER_FRAME = 128;
        const uint STM32_FLASH_BASE_ADDRESS = 0x08000000;
        const uint STM32_PAGE_SIZE = 1024;
        const uint STM32_UID_ADDRESS = 0x1FFFF7E8;
        const uint STM32_FLASHSIZE_ADDRESS = 0x1FFFF7E0;

        SerialPort port;
        ArrayList flashDataList;
        ArrayList pageList;

        public Bootloader(string comport) {
            port = new SerialPort(comport);
            port.BaudRate = 115200;
            port.Parity = Parity.Even;
            port.StopBits = StopBits.One;
            massErase = false;
        }

        ~Bootloader() {
            close();
        }

        public void close() {
            port.Close();
        }

        public void connect() {
            state = loaderstate_t.BUSY;

            try {
                port.Open();               
                Thread worker = new Thread(connectThread);
                worker.Start();
            }
            catch {
                state = loaderstate_t.ERROR;
                return;
            }
        }

        public void burn() {
            state = loaderstate_t.BUSY;

            if ((!port.IsOpen) || (flashDataList.Count == 0)) {
                state = loaderstate_t.ERROR;
                return;
            }

            Thread worker = new Thread(burnThread);
            worker.Start();
        }

        void connectThread() {

            blCommand command = new blCommand(new byte[] { CMD_DISCOVER }, 1, COMMAND_DEFAULT_TIMEOUT, false, ref port);
            while (command.state == blCommand.blCommandState_t.BUSY) Thread.Sleep(1);

            if ((command.state == blCommand.blCommandState_t.TIMEOUT) || (command.rxData[0] != RSP_ACK)) state = loaderstate_t.ERROR;
            else {
                //ok, device connected, send get command, read version
                command = new blCommand(new byte[] { CMD_GET }, 3, COMMAND_DEFAULT_TIMEOUT, true, ref port);
                while (command.state == blCommand.blCommandState_t.BUSY) Thread.Sleep(1);

                if ((command.state == blCommand.blCommandState_t.TIMEOUT) || (command.rxData[0] != RSP_ACK)) state = loaderstate_t.ERROR;
                else {
                    //decode response and read bootloader version
                    blversion_major = (byte)(command.rxData[2] >> 4);
                    blversion_minor = (byte)(command.rxData[2] & 0x0F);

                    //read serial number
                    byte[] data = new byte[12];
                    bool readOK = readMemory(STM32_UID_ADDRESS, 12, ref data, ref port);

                    if (!readOK) state = loaderstate_t.ERROR;
                    else {
                        //decode data to serial number
                        serialnumber = BitConverter.ToString(data).Replace("-", string.Empty);

                        //read flash size
                        readOK = readMemory(STM32_FLASHSIZE_ADDRESS, 2, ref data, ref port);

                        if (!readOK) state = loaderstate_t.ERROR;
                        else {
                            //decode data to falsh size
                            flashsize = (byte)data[0] + (256 * ((byte)data[1]));
                            //Connection completed successfully
                            state = loaderstate_t.SUCCESS;
                        }                        
                    }                    
                }
            }
        }

        void burnThread() {

            numFlashCommandsSP = flashDataList.Count;
            numFlashCommandsPV = 0;

            //flash erase
            blCommand command = new blCommand(new byte[] { CMD_ERASE }, 1, COMMAND_DEFAULT_TIMEOUT, true, ref port);
            while (command.state == blCommand.blCommandState_t.BUSY) Thread.Sleep(1);

            if ((command.state == blCommand.blCommandState_t.TIMEOUT) || (command.rxData[0] != RSP_ACK)) state = loaderstate_t.ERROR;
            else {
                if (massErase) command = new blCommand(new byte[] { 0xFF }, 1, COMMAND_DEFAULT_TIMEOUT * 10, true, ref port);
                else {
                    byte[] cmd = new byte[1 + pageList.Count];
                    int cnt = 0;
                    cmd[cnt++] = (byte)(pageList.Count - 1);
                    for (int i = 0; i < pageList.Count; i++) cmd[cnt++] = (byte)(pageList[i]);                    
                    command = new blCommand(cmd, 1, COMMAND_DEFAULT_TIMEOUT * 20, true, ref port);
                }
                while (command.state == blCommand.blCommandState_t.BUSY) Thread.Sleep(1);

                if ((command.state == blCommand.blCommandState_t.TIMEOUT) || (command.rxData[0] != RSP_ACK)) state = loaderstate_t.ERROR;
                else {
                    //write data
                    bool writeOk;
                    foreach (blFlashData d in flashDataList) {
                        writeOk = writeMemory(d.address, d.length, d.data, ref port);
                        numFlashCommandsPV++;
                        if (!writeOk) {
                            state = loaderstate_t.ERROR;
                            return;
                        }
                    }

                    //send go
                    command = new blCommand(new byte[] { CMD_GO }, 1, COMMAND_DEFAULT_TIMEOUT, true, ref port);
                    while (command.state == blCommand.blCommandState_t.BUSY) Thread.Sleep(1);
                    command = new blCommand(new byte[] { 0x08, 0x00, 0x00, 0x00 }, 1, COMMAND_DEFAULT_TIMEOUT, true, ref port);
                    while (command.state == blCommand.blCommandState_t.BUSY) Thread.Sleep(1);                    

                    state = loaderstate_t.SUCCESS;
                }
            }
        }

        bool readMemory(uint address, uint numBytes, ref byte[] rsp, ref SerialPort serialPort) {

            //send command, wait for ack
            blCommand command = new blCommand(new byte[] { CMD_READ }, 1, COMMAND_DEFAULT_TIMEOUT, true, ref serialPort);
            while (command.state == blCommand.blCommandState_t.BUSY) Thread.Sleep(1);

            if ((command.state == blCommand.blCommandState_t.TIMEOUT) || (command.rxData[0] != RSP_ACK)) return false;
            else {
                //send address, wait for ack
                byte[] addr = new byte[4];
                addr[0] = (byte)((byte)(address >> 24) & 0xFF);
                addr[1] = (byte)((byte)(address >> 16) & 0xFF);
                addr[2] = (byte)((byte)(address >> 8) & 0xFF);
                addr[3] = (byte)(address & 0xFF);

                command = new blCommand(addr, 1, COMMAND_DEFAULT_TIMEOUT, true, ref serialPort);
                while (command.state == blCommand.blCommandState_t.BUSY) Thread.Sleep(1);

                if ((command.state == blCommand.blCommandState_t.TIMEOUT) || (command.rxData[0] != RSP_ACK)) return false;
                else {
                    //send number of bytes to read, wait for ack and data
                    command = new blCommand(new byte[] { (byte)(numBytes - 1) }, (int)numBytes + 1, COMMAND_DEFAULT_TIMEOUT * 2, true, ref serialPort);
                    while (command.state == blCommand.blCommandState_t.BUSY) Thread.Sleep(1);

                    if ((command.state == blCommand.blCommandState_t.TIMEOUT) || (command.rxData[0] != RSP_ACK)) return false;
                    else {
                        for (int i = 0; i < numBytes; i++) rsp[i] = command.rxData[i + 1];
                        return true;
                    }
                }
            }
        }

        bool writeMemory(uint address, uint numBytes, byte[] data, ref SerialPort serialPort) {

            //send command, wait for ack
            blCommand command = new blCommand(new byte[] { CMD_WRITE }, 1, COMMAND_DEFAULT_TIMEOUT, true, ref serialPort);
            while (command.state == blCommand.blCommandState_t.BUSY) Thread.Sleep(1);

            if ((command.state == blCommand.blCommandState_t.TIMEOUT) || (command.rxData[0] != RSP_ACK)) return false;
            else {
                //send address, wait for ack
                byte[] addr = new byte[4];
                addr[0] = (byte)((byte)(address >> 24) & 0xFF);
                addr[1] = (byte)((byte)(address >> 16) & 0xFF);
                addr[2] = (byte)((byte)(address >> 8) & 0xFF);
                addr[3] = (byte)(address & 0xFF);

                command = new blCommand(addr, 1, COMMAND_DEFAULT_TIMEOUT, true, ref serialPort);
                while (command.state == blCommand.blCommandState_t.BUSY) Thread.Sleep(1);

                if ((command.state == blCommand.blCommandState_t.TIMEOUT) || (command.rxData[0] != RSP_ACK)) return false;
                else {

                    //send number of bytes to write, and data
                    byte[] cmd = new byte[numBytes + 1];
                    cmd[0] = (byte)(numBytes - 1);
                    for (int i = 0; i < numBytes; i++) cmd[i + 1] = data[i];

                    command = new blCommand(cmd, 1, COMMAND_DEFAULT_TIMEOUT * 2, true, ref serialPort);
                    while (command.state == blCommand.blCommandState_t.BUSY) Thread.Sleep(1);

                    if ((command.state == blCommand.blCommandState_t.TIMEOUT) || (command.rxData[0] != RSP_ACK)) return false;
                    else return true;

                }
            }
        }

        void addPageToList(uint address) {
            uint pageNumber = (uint)((address - STM32_FLASH_BASE_ADDRESS) / STM32_PAGE_SIZE);
            //check if number is already in list
            bool found = false;
            foreach (byte n in pageList) {
                if (n == pageNumber) {
                    found = true;
                    break;
                }
            }
            if (found) return;
            pageList.Add((byte)pageNumber);
        }

        public bool parseFirmware(StreamReader inStream) {
            flashDataList = new ArrayList();
            pageList = new ArrayList();
            uint addressOffset = 0;
            blFlashData currentCommand = null;
            int lineCount = 0;
            int byteCount = 0;
            IntelHex hex = new IntelHex();

            //start reading
            while (true) {
                IntelHexStructure s = hex.Read(inStream);
                lineCount++;
                if (s == null) return false;
                else {

                    switch (s.type) {
                        case 1: {
                            //end of file command
                            //add last command to list
                            if (currentCommand != null) if (currentCommand.length > 0) flashDataList.Add(currentCommand);

                            //create list of used pages
                            foreach (blFlashData c in flashDataList) {
                                addPageToList(c.address);
                                addPageToList(c.address + c.length - 1);
                            }

                            codeSize = ((float)byteCount) / 1024;
                            numPages = pageList.Count;

                            return true;
                        }

                        case 2: {
                            //extended segment address record
                            addressOffset = (uint)(s.data[0] << 8);
                            addressOffset += (uint)(s.data[1]);
                            addressOffset = addressOffset << 4;
                            break;
                        }

                        case 3: {
                            //ignore start segment address
                            break;
                        }

                        case 4: {
                            //address linear address command
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
                            if (currentCommand == null) newCmd = true;
                            else {
                                if (currentCommand.length + s.dataLen > MAX_DATA_PER_FRAME) newCmd = true;
                                if (currentCommand.address + currentCommand.length != recordAddress) newCmd = true;
                            }

                            //create new command if needed
                            if (newCmd) {
                                if (currentCommand != null) flashDataList.Add(currentCommand); ;
                                currentCommand = new blFlashData();
                                currentCommand.address = recordAddress;
                            }

                            //copy data to command
                            for (uint i = 0; i < s.dataLen; i++) {
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

        class blFlashData {
            public uint address = 0;
            public uint length = 0;
            public byte[] data = new byte[256];
        }

        class blCommand {

            public enum blCommandState_t {
                IDLE = 0, BUSY, OK, TIMEOUT
            }
            public blCommandState_t state = blCommandState_t.IDLE;
            public byte[] rxData = new byte[256];            
            public int rxLen = 0;
            public int rxCount;

            SerialPort port;
            System.Timers.Timer timeoutTimer;
            Thread dataReceptption;

            public blCommand(byte[] commandData, int rxLength, int timeout, bool addChecksum, ref SerialPort serialPort) {
                port = serialPort;
                timeoutTimer = new System.Timers.Timer(timeout);
                timeoutTimer.Elapsed += timeout_callback;
                timeoutTimer.Enabled = true;

                state = blCommandState_t.BUSY;
                rxCount = 0;
                rxLen = rxLength;
                
                int txLen = commandData.Length;
                byte[] txData = new byte[txLen+5];
                byte checksum = 0x00;
                for (int i = 0; i < txLen; i++) {
                    txData[i] = commandData[i];
                    checksum = (byte)(checksum ^ commandData[i]);
                }
                
                if (addChecksum) {
                    if (txLen > 1) txData[txLen++] = (byte)checksum;
                    else txData[txLen++] = (byte)~checksum;
                }

                //send data
                port.DiscardInBuffer();
                port.DiscardOutBuffer();
                port.Write(txData, 0, txLen);

                dataReceptption = new Thread(blCommandReceiver);
                dataReceptption.Start();

            }

            private void blCommandReceiver() { 
                while(state == blCommandState_t.BUSY) {
                    if (port.BytesToRead > 0) {
                        int receivedBytes = port.Read(rxData, rxCount, port.BytesToRead);
                        rxCount += receivedBytes;

                        //enough bytes received?
                        if (rxCount >= rxLen) {
                            timeoutTimer.Enabled = false;
                            state = blCommandState_t.OK;
                            return;
                        }
                    }
                    Thread.Sleep(1);
                }
            }

            void timeout_callback(object sender, EventArgs e) {
                state = blCommandState_t.TIMEOUT;
                timeoutTimer.Enabled = false;
            }
        }
    }
}
