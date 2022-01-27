using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace xwLedConfigurator {
    public class CP210x {
        [DllImport("Scilabs/CP210x_InterfaceLibrary/ManufacturingDLL/CP210xManufacturing.dll")]
        private static extern Int32 CP210x_GetNumDevices(ref Int32 numOfDevices);
        public static Int32 GetNumDevices(ref Int32 numOfDevices) {
            return CP210x_GetNumDevices(ref numOfDevices);
        }

        [DllImport("CP210xManufacturing.dll")]
        private static extern Int32 CP210x_Open(Int32 deviceNum, ref IntPtr handle);
        public static Int32 Open(Int32 deviceNum, ref IntPtr handle) {
            return CP210x_Open(deviceNum, ref handle);
        }

        [DllImport("CP210xManufacturing.dll")]
        private static extern Int32 CP210x_Close(IntPtr handle);
        public static Int32 Close(IntPtr handle) {
            return CP210x_Close(handle);
        }

        [DllImport("CP210xManufacturing.dll")]
        private static extern Int32 CP210x_GetPartNumber(IntPtr handle, Byte[] lpbPartNum);
        public static Int32 GetPartNumber(IntPtr handle, Byte[] lpbPartNum) {
            return CP210x_GetPartNumber(handle, lpbPartNum);
        }

        [DllImport("CP210xManufacturing.dll")]
        private static extern Int32 CP210x_GetDeviceProductString(IntPtr deviceHandle, Byte[] devProduct, ref Int32 length, bool ConvertToASCII);
        public static Int32 getDeviceProductString(IntPtr deviceHandle, Byte[] devProduct, ref Int32 length, bool ConvertToASCII) {
            return CP210x_GetDeviceProductString(deviceHandle, devProduct, ref length, ConvertToASCII);
        }

        [DllImport("CP210xManufacturing.dll")]
        private static extern Int32 CP210x_GetDeviceManufacturerString(IntPtr deviceHandle, Byte[] manufacturer, ref Int32 length, bool ConvertToASCII);
        public static Int32 getManufacturerString(IntPtr deviceHandle, Byte[] manufacturer, ref Int32 length, bool ConvertToASCII) {
            return CP210x_GetDeviceManufacturerString(deviceHandle, manufacturer, ref length, ConvertToASCII);
        }

        [DllImport("CP210xManufacturing.dll")]
        private static extern Int32 CP210x_GetDeviceInterfaceString(IntPtr deviceHandle, Int32 deviceNum, Byte[] interfacestring, ref Int32 length, bool ConvertToASCII);
        public static Int32 getDeviceInterfaceString(IntPtr deviceHandle, Int32 deviceNum, Byte[] interfacestring, ref Int32 length, bool ConvertToASCII) {
            return CP210x_GetDeviceInterfaceString(deviceHandle, deviceNum, interfacestring, ref length, ConvertToASCII);
        }

        [DllImport("Scilabs/CP210x_InterfaceLibrary/RuntimeDLL/CP210xRuntime.dll")]
        private static extern Int32 CP210xRT_WriteLatch(IntPtr handle, UInt16 mask, UInt16 latch);
        public static Int32 WriteLatch(IntPtr handle, UInt16 mask, UInt16 latch) {
            return CP210xRT_WriteLatch(handle, mask, latch);
        }

        [DllImport("CP210xRuntime.dll")]
        private static extern Int32 CP210xRT_ReadLatch(IntPtr handle, UInt16[] lpLatch);
        public static Int32 ReadLatch(IntPtr handle, UInt16[] lpLatch) {
            return CP210xRT_ReadLatch(handle, lpLatch);
        }
    }
}
