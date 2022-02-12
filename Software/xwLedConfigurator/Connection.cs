using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.Threading;
using System.Runtime.InteropServices;
using System.Timers;

namespace xwLedConfigurator {

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
        public const int COM_TIMEOUT = 500;
        public static string portName = "";

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
            connectionTimeoutTimer.Interval = COM_TIMEOUT;
            connectionTimeoutTimer.Elapsed += connectionTimeout;
            connectionTimeoutTimer.Enabled = false;
        }

        static SerialPort comPort;
        static bool connectionEnabled = false;
        static cRxFrame rxFrame = new cRxFrame();
        static System.Timers.Timer connectionTimeoutTimer = new System.Timers.Timer();
        static Queue<messageQueue_t> messageQueue = new Queue<messageQueue_t>();
        struct messageQueue_t {
            public byte scope;
            public byte[] data;
        }

        static void connectionTimeout(Object source, ElapsedEventArgs e) {
            state = connectionStates.Searching;
            connectionTimeoutTimer.Stop();
        }
        
        public static void putFrame(byte dataScope, byte[] data) {
            messageQueue_t msg = new messageQueue_t();
            msg.scope = dataScope;
            msg.data = data;
            messageQueue.Enqueue(msg);
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
                                if (portList.Length > 0) {
                                    listIndex++;
                                    if (listIndex >= portList.Length) listIndex = 0;
                                    comPort.PortName = portList[listIndex];

                                    try
                                    {
                                        comPort.Open();
                                        sendFrame((byte)xwCom.SCOPE.COMMAND, new byte[] { (byte)xwCom.COMMAND.GET_STATIC_INFO });
                                        Thread.Sleep(50);
                                    }
                                    catch { Thread.Sleep(100); }

                                }
                                else Thread.Sleep(100);

                                break;
                            }
                                

                        case connectionStates.Connected: {

                                //check if there is a message in the queue
                                if (messageQueue.Count > 0) {
                                    messageQueue_t msg = messageQueue.Dequeue();
                                    sendFrame(msg.scope, msg.data);
                                }
                                else {
                                    sendFrame((byte)xwCom.SCOPE.COMMAND, new byte[] { (byte)xwCom.COMMAND.GET_DYNAMIC_INFO });
                                }                                
                                Thread.Sleep(20);

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
                                                                //go to connected state if not there already
                                                                if (state == connectionStates.Searching) {
                                                                    state = connectionStates.Connected;
                                                                    portName = comPort.PortName;
                                                                    messageQueue.Clear();
                                                                    //request inital informations required by windows
                                                                    putFrame((byte)xwCom.SCOPE.CONFIG, new byte[] { (byte)xwCom.CONFIG.GET_CONFIG });
                                                                }

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

        private static bool sendFrame(byte dataScope, byte[] data) {
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
            catch {
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
