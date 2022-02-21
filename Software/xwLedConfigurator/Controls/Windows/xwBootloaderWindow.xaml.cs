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
using System.IO;

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

        string currentMessage = "Connect device and search bootloader";
        string deviceInfoText = "";
        bool parseFirmwareRquest = false;
        bool firmwareParsed = false;
        string shippedFirmwareFile = System.IO.Path.Combine(Directory.GetParent(System.Reflection.Assembly.GetExecutingAssembly().Location).FullName, "Firmware\\firmware.hex");
        string slectedFirmwareFile = "";
        bool burning = false;
        FontAwesome.Sharp.IconChar currentIcon = FontAwesome.Sharp.IconChar.Search;

        private void updateGui(Object myObject, EventArgs myEventArgs) {
            loaderMessage.Text = currentMessage;
            loaderIcon.Icon = currentIcon;
            deviceInfo.Text = deviceInfoText;

            if (parseFirmwareRquest) {
                parseFirmwareRquest = false;
                parseFirmware();
            }

            if (burning) {                
                if (bootloader.numFlashCommandsSP > 0) {
                    int progressPercent = (int)Math.Round((double)bootloader.numFlashCommandsPV * 100 / bootloader.numFlashCommandsSP);
                    progress.Value = progressPercent;
                    progressText.Text = String.Format("Flashing {0}%", progressPercent);

                    loaderMessage.Visibility = Visibility.Hidden;
                    loaderIcon.Visibility = Visibility.Hidden;
                    progress.Visibility = Visibility.Visible;
                    progressText.Visibility = Visibility.Visible;
                }
            }
            else {
                progress.Visibility = Visibility.Hidden;
                progressText.Visibility = Visibility.Hidden;
                loaderMessage.Visibility = Visibility.Visible;
                loaderIcon.Visibility = Visibility.Visible;
            }
        }

        Bootloader bootloader;

        void searchBootLoaderThread() {

            currentMessage = "Resetting device";
            currentIcon = FontAwesome.Sharp.IconChar.Fire;
            deviceInfoText = "-\n-\n-";

            //close bootlaoder if still open
            if (bootloader != null) bootloader.close();

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
                    Thread.Sleep(200);

                    if (retval == 0) {
                        currentMessage = "Waiting for bootlaoder response";
                        currentIcon = FontAwesome.Sharp.IconChar.Fire;
                        Thread.Sleep(200);

                        //reset executed succesfully, try to reach bootloader on each available comports
                        string[] ports = SerialPort.GetPortNames();

                        for (int j = ports.Length - 1; j >= 0; j--) {

                            //try to connect to target on this port
                            bootloader = new Bootloader(ports[j]);
                            bootloader.connect();
                            while (bootloader.state == Bootloader.loaderstate_t.BUSY) Thread.Sleep(10);

                            //if bootloader connected, stop
                            if (bootloader.state == Bootloader.loaderstate_t.SUCCESS) {
                                currentMessage = "Bootloader connected. Select firmware and flash";
                                currentIcon = FontAwesome.Sharp.IconChar.ThumbsUp;
                                deviceInfoText = bootloader.serialnumber + "\n" + bootloader.flashsize + "kB\nV" + bootloader.blversion_major + "." + bootloader.blversion_minor;
                                parseFirmwareRquest = true;
                                return;
                            }
                        }

                        //if we enter here, bootloader did not connect on any port
                        currentMessage = "No answer received from device";
                        currentIcon = FontAwesome.Sharp.IconChar.ExclamationTriangle;
                        bootloader.close();
                        return;
                    }
                    else {
                        //error while sending reset sequence
                        currentMessage = "Error sending reset seuqence to device";
                        currentIcon = FontAwesome.Sharp.IconChar.ExclamationTriangle;
                        return;
                    }
                }
            }
            //no xwledcontrol device found
            currentMessage = "Cannot find any xwLedControl device...";
            currentIcon = FontAwesome.Sharp.IconChar.ExclamationTriangle;
        }

        void burnThread() {
            burning = true;
            //bootloader.massErase = true;
            bootloader.burn();
            while (bootloader.state == Bootloader.loaderstate_t.BUSY) Thread.Sleep(100);

            burning = false;

            if (bootloader.state == Bootloader.loaderstate_t.SUCCESS) {
                currentMessage = "Flashing completed successfully!";
                currentIcon = FontAwesome.Sharp.IconChar.ThumbsUp;
            }
            else {
                currentMessage = "Flashing failed... Please reconnect and try again";
                currentIcon = FontAwesome.Sharp.IconChar.ExclamationTriangle;
            }

            //close the bootloader
            bootloader.close();
        }

        private void UserControl_IsVisibleChanged(object sender, DependencyPropertyChangedEventArgs e) {
            if (this.Visibility == Visibility.Visible) {
                //Stop normal connection thread
                Connection.stop();

                //reset variables to default
                currentMessage = "Connect to device and search bootloader";
                currentIcon = FontAwesome.Sharp.IconChar.Search;
                deviceInfoText = "-\n-\n-";
                bSearch.IsEnabled = true;
                burning = false;
            }
            else {
                //close bootloader if still open
                if (bootloader != null) bootloader.close();
                //start regular connection
                Connection.start();
            }
        }

        private void bSearch_Click(object sender, RoutedEventArgs e) {
            Thread worker = new Thread(searchBootLoaderThread);
            worker.Start();
        }

        private void bFlash_Click(object sender, RoutedEventArgs e) {
            if (deviceInfoText.StartsWith("-")) {
                currentMessage = "Connect to device before burning";
                currentIcon = FontAwesome.Sharp.IconChar.ExclamationTriangle;
            }
            else if (!firmwareParsed) {
                currentMessage = "Select a valid firmware file before burning";
                currentIcon = FontAwesome.Sharp.IconChar.ExclamationTriangle;
            }
            else {
                if ((bool)massErase.IsChecked) bootloader.massErase = true;
                else bootloader.massErase = false;
                Thread worker = new Thread(burnThread);
                worker.Start();               
            }
        }

        private void parseFirmware() {

            if (bootloader == null) goto noFileParsed;

            string file = "";
            if (firmwareManual.IsChecked == true) {
                if (slectedFirmwareFile.Length == 0) goto noFileParsed;
                file = slectedFirmwareFile;
            }
            else file = shippedFirmwareFile;

            if (bootloader.parseFirmware(new StreamReader(file))) {
                string displayFileName = System.IO.Path.GetFileName(file);
                fileInfo.Text = displayFileName + "\n" + Math.Round(bootloader.codeSize, 2).ToString() + "kb\n" + bootloader.numPages.ToString();
                firmwareParsed = true;
                return;
            }
            
            MessageBox.Show("Cannot load firware file.");

            noFileParsed:
            fileInfo.Text = "-\n-\n-";
            firmwareParsed = false;
        }

        private void firmwareShipped_Click(object sender, RoutedEventArgs e) {
            parseFirmware();
        }

        private void firmwareManual_Click(object sender, RoutedEventArgs e) {
            if (slectedFirmwareFile.Length > 0) parseFirmware();
            else bSelectFile_Click(null, null);
        }

        private void bSelectFile_Click(object sender, RoutedEventArgs e) {
            Microsoft.Win32.OpenFileDialog openFileDialog = new Microsoft.Win32.OpenFileDialog();
            openFileDialog.DefaultExt = ".hex"; // Default file extension
            openFileDialog.Filter = "S19 Firmware File | *.hex"; // Filter files by extension
            if (openFileDialog.ShowDialog() == true) {
                firmwareManual.IsChecked = true;
                slectedFirmwareFile = openFileDialog.FileName;                
            }
            parseFirmware();
        }
    }

}
