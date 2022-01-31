using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.Threading;
using System.Runtime.InteropServices;
using System.Timers;

namespace xwLedConfigurator {

    public static class xwCom {

        public enum HW_VERSIONS : byte {
            Unknown = 0x00,
            xwLedControl_V1 = 0x01
         }

        public enum SCOPE : byte {
            TRACE = 0x00,
            COMMAND = 0x01,
            CONFIG = 0x02,
            LED = 0x03
        }

        public enum COMMAND : byte {
            RESET = 0x01,
            GET_STATIC_INFO = 0x02,
            GET_DYNAMIC_INFO = 0x03,
            GET_VARIABLE_INFO = 0x04,
            STATUS_LED_MODE = 0x05
        }

        public enum COMMANDRESPONSE : byte {
            RESPONSE_STATIC_INFO = 0x01,
            RESPONSE_DYNAMIC_INFO = 0x02,
            RESPONSE_VARIABLE_INFO = 0x03
        }
    }

    public class cRxFrame {
        public enum rxState_t {
            IDLE,
            SCOPE,
            DATA
        }
        public rxState_t rxState = rxState_t.IDLE;
        public bool isPadded = false;
        public byte scope = 0;
        public byte[] data = new byte[Connection.COM_MAX_DATA_LEN + 1];
        public int rxCount = 0;
        public byte crc = (byte)Connection.comControl.CRC_INIT;
    }

    public static class Connection {

        public const int COM_MAX_DATA_LEN = 256;

        public delegate void frameReceiver(ref cRxFrame rxFrame);
        static public event frameReceiver frameReceived; // event        

        public enum comControl : byte {
            SOF = (byte) '{',
            PAD = (byte) '/',
            EOF = (byte) '}',
            CRC_INIT = 0x12
        }

        public enum connectionStates {
            Closed,            
            Searching,
            Connected
        }
        public static connectionStates state = connectionStates.Closed;

        static Connection() {
            state = connectionStates.Closed;
            Thread connectionWorker = new Thread(connectionThread);
            connectionWorker.Start();
            Thread receiver = new Thread(dataReceiver);
            receiver.Start();
            connectionTimeoutTimer.Interval = 500;
            connectionTimeoutTimer.Elapsed += connectionTimeout;
            connectionTimeoutTimer.Enabled = false;
        }

        static SerialPort comPort;
        static bool connectionEnabled = false;
        static cRxFrame rxFrame = new cRxFrame();
        static System.Timers.Timer connectionTimeoutTimer = new System.Timers.Timer();

        static void connectionTimeout(Object source, ElapsedEventArgs e) {
            state = connectionStates.Searching;
            connectionTimeoutTimer.Stop();
        }

        static void connectionThread() {

            string[] portList = new string[1];
            int listIndex = 0;

            while (true) {

                if (connectionEnabled) {
                    switch (state) {
                        case connectionStates.Closed: {
                                comPort = new SerialPort();
                                comPort.BaudRate = 115200;
                                comPort.DataBits = 8;
                                comPort.StopBits = StopBits.One;
                                comPort.Parity = Parity.None;
                                listIndex = 0;
                                
                                state = connectionStates.Searching;
                                break;
                            }   

                        case connectionStates.Searching: {
                                
                                if (comPort.IsOpen) {
                                    try { comPort.Close(); }
                                    catch { }
                                }

                                portList = SerialPort.GetPortNames();
                                listIndex++;
                                if (listIndex >= portList.Length) listIndex = 0;
                                comPort.PortName = portList[listIndex];            

                                try {
                                    comPort.Open();
                                    sendFrame((byte)xwCom.SCOPE.COMMAND, new byte[] { (byte)xwCom.COMMAND.GET_STATIC_INFO });
                                    Thread.Sleep(100);
                                }
                                catch {
                                    Thread.Sleep(100);
                                }

                                break;
                            }
                                

                        case connectionStates.Connected: {

                                sendFrame((byte)xwCom.SCOPE.COMMAND, new byte[] { (byte)xwCom.COMMAND.GET_DYNAMIC_INFO });
                                Thread.Sleep(400);

                                break;
                            }
                    }

                }
                else {
                    state = connectionStates.Closed;
                    if (comPort != null) {
                        if (comPort.IsOpen) comPort.Close();
                    }
                }

                Thread.Sleep(10);
            }            
        }

        static void dataReceiver() {
            while (true) {
                //try to read data from port
                if (comPort != null) {
                    try {
                        int bytesToRead = comPort.BytesToRead;
                        if (bytesToRead > 0) {
                            byte[] rxData = new byte[bytesToRead];
                            int rxBytes = comPort.Read(rxData, 0, bytesToRead);

                            for (int i = 0; i < rxBytes; i++) {

                                byte c = rxData[i];

                                //process received bytes
                                switch (rxFrame.rxState) {

                                    case cRxFrame.rxState_t.IDLE: {
                                            rxFrame.crc = (byte)comControl.CRC_INIT;
                                            rxFrame.isPadded = false;
                                            rxFrame.rxCount = 0;
                                            rxFrame.rxState = cRxFrame.rxState_t.SCOPE;
                                            break;
                                        }

                                    case cRxFrame.rxState_t.SCOPE: 
                                    case cRxFrame.rxState_t.DATA: {
                                            if (!((c == (byte)comControl.SOF) || (c == (byte)comControl.EOF) || (c == (byte)comControl.PAD)) || (rxFrame.isPadded)) {
                                                rxFrame.isPadded = false;
                                                rxFrame.crc = updateCrc(rxFrame.crc, c);

                                                if (rxFrame.rxState == cRxFrame.rxState_t.SCOPE) {
                                                    rxFrame.scope = c;
                                                    rxFrame.rxState = cRxFrame.rxState_t.DATA;
                                                }
                                                else {
                                                    rxFrame.data[rxFrame.rxCount++] = c;
                                                }
                                            }
                                            else {
                                                switch (c) {
                                                    case (byte)comControl.SOF: {
                                                            //this is an error, restart reception
                                                            rxFrame.rxState = cRxFrame.rxState_t.SCOPE;
                                                            rxFrame.rxCount = 0;
                                                            rxFrame.isPadded = false;
                                                            rxFrame.crc = (byte)comControl.CRC_INIT;
                                                            break;
                                                        }

                                                    case (byte)comControl.PAD: {
                                                            rxFrame.isPadded = true;
                                                            break;
                                                        }

                                                    case (byte)comControl.EOF: {
                                                            //reception complete, the last received databyte was the crc, check crc (must be 0 here), and process if crc is correct
                                                            rxFrame.rxCount--;
                                                            rxFrame.rxState = cRxFrame.rxState_t.IDLE;
                                                            if (rxFrame.crc == 0) {
                                                                //successful reception
                                                                //go to connected state
                                                                if (state == connectionStates.Searching) state = connectionStates.Connected;

                                                                //reset timeout timer
                                                                connectionTimeoutTimer.Stop();
                                                                connectionTimeoutTimer.Start();

                                                                //raise event for data processing
                                                                frameReceived(ref rxFrame);

                                                            }
                                                            break;
                                                        }
                                                }
                                            }

                                            break;
                                        }
                                }
                            }
                        }
                    }
                    catch {
                        //do nothing
                    }
                }
                Thread.Sleep(10);
            }
        }

        static bool sendFrame(byte dataScope, byte[] data) {
            byte[] buffer = new byte[data.Length * 2 + 5];
            int bytesToSend = 0;
            byte crc = (byte)comControl.CRC_INIT;

            buffer[bytesToSend++] = (byte)comControl.SOF;
            
            if ((dataScope == (byte)comControl.SOF) || (dataScope == (byte)comControl.EOF) || (dataScope == (byte)comControl.PAD)) buffer[bytesToSend++] = (byte)comControl.PAD;
            buffer[bytesToSend++] = dataScope;
            crc = updateCrc(crc, dataScope);

            for (int i = 0; i < data.Length; i++) {
                if ((data[i] == (byte)comControl.SOF) || (data[i] == (byte)comControl.EOF) || (data[i] == (byte)comControl.PAD)) buffer[bytesToSend++] = (byte)comControl.PAD;
                buffer[bytesToSend++] = data[i];
                crc = updateCrc(crc, data[i]);
            }

            buffer[bytesToSend++] = crc;
            buffer[bytesToSend++] = (byte)comControl.EOF;

            try {
                comPort.Write(buffer, 0, bytesToSend);
            }
            catch (Exception ex) {
                return false;
            }

            return true;
        }

        static byte updateCrc(byte crc, byte data) {
            return (byte)(crc ^ data);
        }

        public static void start() {
            connectionEnabled = true;
            state = connectionStates.Closed;
        }

        public static void stop() {
            connectionEnabled = false;
            state = connectionStates.Closed;
            connectionTimeoutTimer.Stop();
        }


    }
}
