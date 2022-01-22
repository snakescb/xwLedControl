using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.Threading;
using System.Runtime.InteropServices;

namespace xwLedConfigurator {


    public class Connection {

        public enum connectionStates {
            Disconnected,
            Connected
        }

        public connectionStates connectioState = connectionStates.Disconnected;

        public Connection() {         
            Thread serialHandleThread = new Thread(checkSerialHandle);
            serialHandleThread.Start();
        }

        private IntPtr serialHandle = IntPtr.Zero;

        public bool intializeBootloader() {

            if (serialHandle != IntPtr.Zero) {

                //reset sequence
                int retval = 0;
                retval += CP210x.Open(0, ref serialHandle);
                retval += CP210x.WriteLatch(serialHandle, 1, 0);
                Thread.Sleep(50);
                retval += CP210x.WriteLatch(serialHandle, 2, 2);
                Thread.Sleep(50);
                retval += CP210x.WriteLatch(serialHandle, 1, 1);
                Thread.Sleep(50);
                retval += CP210x.WriteLatch(serialHandle, 2, 0);
                Thread.Sleep(50);
                retval += CP210x.Close(serialHandle);

                if (retval == 0) return true;
            }
            return false;

        }

        

        void checkSerialHandle() {

            while (true) {

                Int32 numDevs = 0;
                Int32 retVal = CP210x.GetNumDevices(ref numDevs);

                if (serialHandle != IntPtr.Zero) {
                    if ((numDevs == 0) || (retVal > 0)) {
                        serialHandle = IntPtr.Zero;
                        connectioState = connectionStates.Disconnected;
                    }
                }
                else {

                    for (Int32 i = 0; i < numDevs; i++) {

                        IntPtr handle = IntPtr.Zero;
                        Int32 tempStringlen = 0;
                        Byte[] tempString = new Byte[100];
                        string productString = "";
                        string manufacturerString = "";

                        retVal = CP210x.Open(i, ref handle);
                        retVal += CP210x.getDeviceProductString(handle, tempString, ref tempStringlen, true);
                        productString = Encoding.UTF8.GetString(tempString, 0, tempStringlen);
                        retVal += CP210x.getManufacturerString(handle, tempString, ref tempStringlen, true);
                        manufacturerString = Encoding.UTF8.GetString(tempString, 0, tempStringlen);
                        
                        retVal += CP210x.Close(handle);

                        if ((retVal == 0) && (productString.ToLower().StartsWith("xwledcontrol"))) {
                            serialHandle = handle;
                            connectioState = connectionStates.Connected;
                            break;
                        }
                    }              
                }
                
                Thread.Sleep(500);
            }
        }

    }
}
