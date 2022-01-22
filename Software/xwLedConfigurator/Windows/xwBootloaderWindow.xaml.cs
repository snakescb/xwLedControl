using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Globalization;
using System.Reflection;
using System.Threading;
using System.Timers;
using System.Linq;
using System.IO.Ports;

namespace xwLedConfigurator
{
    /// <summary>
    /// Interaktionslogik für xwInfoControl.xaml
    /// </summary>
    /// 


    public partial class xwBootloaderWindow : UserControl {

		public xwBootloaderWindow() {
            InitializeComponent();

			//timer for gui update
			System.Windows.Forms.Timer guiUpdate = new System.Windows.Forms.Timer();
			guiUpdate.Tick += new EventHandler(updateGui);
			guiUpdate.Interval = 100;
			guiUpdate.Enabled = true;

		}

		private void updateGui(Object myObject, EventArgs myEventArgs) {

		}

        Bootloader loader;

        private void searchLoader(object sender, RoutedEventArgs e) {
       

            //search device
            Int32 numDevs = 0;
            Int32 retVal = CP210x.GetNumDevices(ref numDevs);            

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
                    
                    //device fount, send reset sequence
                    int retval = 0;
                    retval += CP210x.Open(0, ref handle);
                    retval += CP210x.WriteLatch(handle, 1, 0);
                    Thread.Sleep(5);
                    retval += CP210x.WriteLatch(handle, 2, 2);
                    Thread.Sleep(5);
                    retval += CP210x.WriteLatch(handle, 1, 1);
                    Thread.Sleep(5);
                    retval += CP210x.WriteLatch(handle, 2, 0);
                    Thread.Sleep(5);
                    retval += CP210x.Close(handle);

                    if (retval == 0) {

                        //reset executed succesfully, try to reach bootloader on all available comports
                        string[] ports = SerialPort.GetPortNames();

                        foreach (string portname in ports) {

                            ComPort port = new ComPort();
                            port.portOpen(portname, 115200, Parity.Even, 8, StopBits.One);
                            Bootloader loader = new Bootloader(port);
                            loader.connect();
                            while (loader.state == Bootloader.loaderstate_t.BUSY) Thread.Sleep(10);

                            if (loader.state == Bootloader.loaderstate_t.SUCCESS) {
                                loaderMessage.Text = "Bootloader connected!";
                                loaderIcon.Icon = FontAwesome.Sharp.IconChar.ThumbsUp;
                                port.portClose();
                                return;
                            }
                            else port.portClose();
                        }
                    }
                }                
            }

            loaderMessage.Text = "Cannot connect to device...";
            loaderIcon.Icon = FontAwesome.Sharp.IconChar.ThumbsDown;
        }
    }

}
