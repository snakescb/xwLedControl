﻿using System;
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

namespace xwLedConfigurator {

    public partial class xwConfigWindow : UserControl {

		string deviceUid = "";
		string deviceType = "";
		string deviceVersion = "";
		string deviceFWVersion = "";
		int deviceConfigSize = 0;
		int deviceUsedConfigSize = 0;
		int sequence = 0;
		int numSequences = 0;
		int dim = 0;
		int speed = 0;
		int battery = 0;
		bool batteryWarning = false;
		string newTraceboxText = "";
		string runMode = "";
		bool tracePause = false;
		bool rxFailsafe = true;
		int rxScaled = 0;
		uint rxRaw = 0;
		uint aux = 0;
		byte configVoltage = 0;
		bool configUpdated = true;

		public xwConfigWindow() {
            InitializeComponent();

			string date = "";
			string branch = "";
			string tags = "";
			string committer = "";
			string message = "";

			var customMetadataList = Assembly.GetEntryAssembly().GetCustomAttributes<AssemblyMetadataAttribute>();
			foreach (var customMetadata in customMetadataList) {
				if (customMetadata.Key == "Branch") branch = customMetadata.Value;
				if (customMetadata.Key == "Date") date = customMetadata.Value;
				if (customMetadata.Key == "Tags") tags = customMetadata.Value;
				if (customMetadata.Key == "Committer") committer = customMetadata.Value;
				if (customMetadata.Key == "Message") message = customMetadata.Value;
			}

			//update software information
			versionInfoText.Text = Version.version + "\n" + date + "\n" + committer + "\n" + message;

			//timer for gui update
			System.Windows.Forms.Timer guiUpdate = new System.Windows.Forms.Timer();
			guiUpdate.Tick += new EventHandler(updateGui);
			guiUpdate.Interval = 50;
			guiUpdate.Enabled = true;

			Connection.frameReceived += this.frameReceiver;
		}

		private void updateGui(Object myObject, EventArgs myEventArgs) {

			if (Connection.state == Connection.connectionStates.Connected) {
				textConnection.Text = "Connected (" + Connection.portName + ")\n";
				textConnection.Text += deviceType + " " + deviceVersion + "\n";
				textConnection.Text += Math.Round(((float)deviceConfigSize / 1024), 2).ToString() + "kB";
				textConnection.Text += " (" + ((float)deviceUsedConfigSize / 1024).ToString("0.00") + "kB used)\n";
				textConnection.Text += "V" + deviceFWVersion + "\n";
				textConnection.Text += deviceUid;


				textLiveData.Text = runMode + "\n";
				if (sequence == 0xFF) textLiveData.Text += "OFF / ";
				else textLiveData.Text += (sequence+1).ToString() + " / ";
				if (numSequences == 0xFF) textLiveData.Text += "Empty Device\n";
				else textLiveData.Text +=  numSequences.ToString() + "\n";
				textLiveData.Text += dim.ToString() + "%\n";
				textLiveData.Text += (Math.Round(speed*100.0 / 0x10000)).ToString() + "%\n";
				textLiveData.Text += (Math.Round(aux/10.0)).ToString() + "%\n";
				if (rxFailsafe) textLiveData.Text += "No Receiver\n";
				else textLiveData.Text += rxScaled.ToString() + "% ( " + rxRaw.ToString() + "us )\n";
				if (battery < 50) textLiveData.Text += "No Battery\n";
				else {
					textLiveData.Text += ((float)battery / 10).ToString("0.0") + "V";
					if (batteryWarning) textLiveData.Text += " ( WARNING )\n";
					else textLiveData.Text += "\n";
				}
				//textLiveData.Text += ((float)deviceUsedConfigSize / 1024).ToString("0.00") + "kB";
			}
			else {
				textConnection.Text = "Disconnected\n-\n-\n-\n-";
				textLiveData.Text = "-\n-\n-\n-\n-\n-\n-";
			}

			if (newTraceboxText != "") {
				traceBox.AppendText(newTraceboxText + "\n");
				traceBox.ScrollToEnd();
				newTraceboxText = "";
			}

			if (configUpdated) {
				configUpdated = false;
				voltageSlider.value = configVoltage;
			}

		}

		private void frameReceiver(ref cRxFrame rxFrame) {

			if (rxFrame.scope == (byte)xwCom.SCOPE.COMMAND) {

				if (rxFrame.data[0] == (byte)xwCom.COMMAND_RESPONSE.RESPONSE_STATIC_INFO) {
					deviceUid = "0x";
					for (int i = 0; i < 12; i++) deviceUid += BitConverter.ToString(new byte[] { rxFrame.data[i + 1] });
					deviceConfigSize = BitConverter.ToInt32(new byte[] { rxFrame.data[16], rxFrame.data[15], rxFrame.data[14], rxFrame.data[13] }, 0);
					deviceVersion = xwCom.deviceVersions[rxFrame.data[17] & 0x0F];
					deviceType = xwCom.deviceTypes[rxFrame.data[17] >> 4];
					deviceFWVersion = rxFrame.data[18].ToString() + "." + rxFrame.data[19].ToString();
				}

				if (rxFrame.data[0] == (byte)xwCom.COMMAND_RESPONSE.RESPONSE_DYNAMIC_INFO) {
					if (rxFrame.data[1] == 0) rxFailsafe = false; 
					else rxFailsafe = true;
					if (!rxFailsafe) {
						rxScaled = BitConverter.ToInt16(new byte[] { rxFrame.data[3], rxFrame.data[2] }, 0);
						rxRaw = (uint)(rxFrame.data[4] << 8) + (uint)(rxFrame.data[5]);
					}
					aux = (uint)BitConverter.ToInt16(new byte[] { rxFrame.data[7], rxFrame.data[6] }, 0);
					runMode = xwCom.runModes[rxFrame.data[10]];
					sequence = rxFrame.data[12];
					numSequences = rxFrame.data[11];
					dim = (int)(rxFrame.data[14] / 2.55);
					battery = rxFrame.data[8];
					if (rxFrame.data[9] > 0) batteryWarning = true;
					else batteryWarning = false;
					deviceUsedConfigSize = BitConverter.ToInt32(new byte[] { rxFrame.data[22], rxFrame.data[21], rxFrame.data[20], rxFrame.data[19] }, 0);
					speed = BitConverter.ToInt32(new byte[] { rxFrame.data[18], rxFrame.data[17], rxFrame.data[16], rxFrame.data[15] }, 0);
				}

			}

			if (rxFrame.scope == (byte)xwCom.SCOPE.CONFIG) {
				if (rxFrame.data[0] == (byte)xwCom.CONFIG_RESPONSE.RESPONSE_CONFIG) {
					configVoltage = rxFrame.data[1];
					configUpdated = true;
				}
			}

			if (rxFrame.scope == (byte)xwCom.SCOPE.TRACE) {
				if (!tracePause) {
					string msg = System.Text.Encoding.Default.GetString(rxFrame.data).Substring(0, rxFrame.rxCount);
					msg = DateTime.Now.ToString("HH:mm:ss.fff") + " :  " + msg;
					if (newTraceboxText.Length == 0) newTraceboxText = msg;
					else newTraceboxText += "\n" + msg;
				}
			}
		}

		private void bTrash_Click(object sender, RoutedEventArgs e) {
			traceBox.Text = "";
        }

        private void bPlayPauseClick(object sender, RoutedEventArgs e) {
			if (tracePause) {
				tracePause = false;
				iconPlayPause.Icon = FontAwesome.Sharp.IconChar.Pause;
			}
			else {
				tracePause = true;
				iconPlayPause.Icon = FontAwesome.Sharp.IconChar.Play;
			}
        }

        private void bSaveConfig_Click(object sender, RoutedEventArgs e) {
			byte voltage = voltageSlider.value;

			//send new config and request config update
			byte[] command = new byte[] { (byte)xwCom.CONFIG.SET_CONFIG, voltage };			
			Connection.putFrame((byte)xwCom.SCOPE.CONFIG, command);
			Connection.putFrame((byte)xwCom.SCOPE.CONFIG, new byte[] { (byte)xwCom.CONFIG.GET_CONFIG });
        }

        private void bFactoryDefault_Click(object sender, RoutedEventArgs e) {
			Connection.putFrame((byte)xwCom.SCOPE.CONFIG, new byte[] { (byte)xwCom.CONFIG.SET_FACTORY_DEFAULTS });
			Connection.putFrame((byte)xwCom.SCOPE.CONFIG, new byte[] { (byte)xwCom.CONFIG.GET_CONFIG });
		}

        private void bReset_Click(object sender, RoutedEventArgs e) {
			Connection.putFrame((byte)xwCom.SCOPE.COMMAND, new byte[] { (byte)xwCom.COMMAND.RESET });
		}

    }

	

}
