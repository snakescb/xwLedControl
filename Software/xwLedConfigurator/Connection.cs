using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.Threading;

namespace xwLedConfigurator {
    public class Connection {

        public enum connectionStates {
            Disconnected,
            Connected
        }

        public enum connectionModes {
            Auto,
            Manual,
            Bootloader
        }

        public connectionStates connectioState = connectionStates.Disconnected;
        public connectionModes connectioMode = connectionModes.Auto;
        public List<string> availablePorts = new List<string>();
        public string currentPort = "";

        SerialPort serialPort = new SerialPort();


        public Connection() {         
            Thread connectorThread = new Thread(connector);
            connectorThread.Start();
        }

        public void setMode(connectionModes newMode) { connectioMode = newMode; }
        public void setPort(string newPort) { switchPort(newPort); }
        public void intializeBootloader() {

            //close openport if any
            try {
                serialPort.Close();
            }
            catch { }

            //setup new port
            serialPort.PortName = currentPort;
            serialPort.BaudRate = 115200;
            serialPort.Parity = Parity.Even;
            serialPort.DataBits = 8;
            serialPort.StopBits = StopBits.One;

            //open new port
            try {
                serialPort.Open();

                serialPort.RtsEnable = false;
                Thread.Sleep(10);
                serialPort.DtrEnable = false;
                Thread.Sleep(10);
                serialPort.DtrEnable = true;
                serialPort.RtsEnable = true;

            }
            catch { }

        }

        void switchPort(string name) {
            currentPort = name;
            try {
                serialPort.Close();
            }
            catch { }
        }

        void connector() {

            while (true) {

                availablePorts.Clear();
                availablePorts.AddRange(SerialPort.GetPortNames());
                availablePorts.Sort();

                if (connectioState == connectionStates.Disconnected) {
                    if (connectioMode == connectionModes.Auto) {
                        foreach (string port in availablePorts) {
                            if (connectioState == connectionStates.Disconnected) {
                                switchPort(port);
                                Thread.Sleep(500);
                            }
                        }
                    }
                    if (connectioMode == connectionModes.Manual) switchPort(currentPort);
                }
               
                Thread.Sleep(500);
            }
        }

    }
}
