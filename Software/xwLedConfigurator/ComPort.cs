using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.Threading;

namespace xwLedConfigurator {
    class ComPort {

        public bool portIsOpen;
        public delegate void rxDelegate(byte[] rx);
        public event rxDelegate rxData;
        
        SerialPort com;
        Thread tReader;

        public ComPort() {
            portIsOpen = false;
        }
        public void portClose() {
            portIsOpen = false;
            try {
                com.Close();
                tReader.Abort();
            }
            catch { }
        }
        public string[] getPorts() {
            return SerialPort.GetPortNames();
        }
        public bool portOpen(string portName, int baudRate, Parity parity, int dataBits, StopBits stopBits) {
            try {
                com = new SerialPort(portName, baudRate, parity, dataBits, stopBits);
                purge();
                com.Open();
                tReader = new Thread(dataReader);
                tReader.Start();
                portIsOpen = true;
                return true;
            }
            catch {
                return false;
            }
        }
        public void purge() {
            try {
                com.DiscardInBuffer();
                com.DiscardOutBuffer();
            }
            catch { };
        }
        public void send(byte[] d, int length) {
            try {
                com.Write(d, 0, length);
            }
            catch { }
        }         
        
        void dataReader() {
            while(true) {
                try {
                    int numbytes = com.BytesToRead;
                    if(numbytes > 0) {
                        if (numbytes > 100) numbytes = 100;
                        byte[] buf = new byte[100];
                        int count = com.Read(buf, 0, 100);
                        rxData(buf);
                    }
                }
                catch {
                    com.Close();
                    portIsOpen = false;
                    break;
                }
            }
        }
        
    }
}
