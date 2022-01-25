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

        enum state_t {
            IDLE = 0,
            CONNECTED
        }

        state_t state = state_t.IDLE;

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

        Bootloader bootloader;

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
                    retval += CP210x.WriteLatch(handle, 3, 2);
                    Thread.Sleep(5);
                    retval += CP210x.WriteLatch(handle, 1, 1);
                    Thread.Sleep(5);
                    retval += CP210x.WriteLatch(handle, 2, 0);
                    Thread.Sleep(10);
                    retval += CP210x.Close(handle);

                    if (retval == 0) {
                        //reset executed succesfully, try to reach bootloader on each available comports
                        string[] ports = SerialPort.GetPortNames();

                        for (int j=ports.Length-1; j>=0; j--) {    
                            
                            //try to connect to target on this port
                            bootloader = new Bootloader(ports[j]);
                            bootloader.connect();
                            while (bootloader.state == Bootloader.loaderstate_t.BUSY) Thread.Sleep(10);

                            //if bootloader connected, stop
                            if (bootloader.state == Bootloader.loaderstate_t.SUCCESS) {
                                loaderMessage.Text = "Bootloader connected!";
                                loaderIcon.Icon = FontAwesome.Sharp.IconChar.ThumbsUp;
                                deviceInfo.Text = bootloader.serialnumber + "\n" + bootloader.flashsize + "kB\nV" + bootloader.blversion_major + "." + bootloader.blversion_minor;

                                bootloader.close();
                                return;
                            }
                        }

                        //if we enter here, bootloader did not connect on any port
                        loaderMessage.Text = "No answer received from device";
                        loaderIcon.Icon = FontAwesome.Sharp.IconChar.ThumbsDown;
                        bootloader.close();
                        return;
                    }
                    else {
                        //error while sending reset sequence
                        loaderMessage.Text = "Error sending reset seuqence to device";
                        loaderIcon.Icon = FontAwesome.Sharp.IconChar.ThumbsDown;
                        return;
                    }
                }                
            }
            //no xwledcontrol device found
            loaderMessage.Text = "Cannot find any xwLedControl device...";
            loaderIcon.Icon = FontAwesome.Sharp.IconChar.ThumbsDown;
        }

        private void UserControl_IsVisibleChanged(object sender, DependencyPropertyChangedEventArgs e) {
            //reset everything to defailt
            loaderMessage.Text = "Connect device and search bootloader";            
            loaderIcon.Icon = FontAwesome.Sharp.IconChar.Search;
            deviceInfo.Text = "-\n-\n-";
            state = state_t.IDLE;
            bSearch.IsEnabled = true;
        }
    }

}
