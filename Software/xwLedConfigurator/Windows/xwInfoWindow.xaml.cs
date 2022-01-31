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

namespace xwLedConfigurator
{

    public partial class xwInfoWindow : UserControl {

		string deviceUid = "";
		string deviceType = "";
		string deviceFWVersion = "";
		int deviceConfigSize = 0;
		string newTraceboxText = "";

		public xwInfoWindow() {
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
			versionInfoText.Text = Assembly.GetEntryAssembly().GetName().Version.ToString() + "\n" + date + "\n" + branch + "\n" + tags + "\n" + committer + "\n" + message;

			//timer for gui update
			System.Windows.Forms.Timer guiUpdate = new System.Windows.Forms.Timer();
			guiUpdate.Tick += new EventHandler(updateGui);
			guiUpdate.Interval = 100;
			guiUpdate.Enabled = true;

			Connection.frameReceived += this.frameReceiver;
		}

		private void updateGui(Object myObject, EventArgs myEventArgs) {

			if (Connection.state == Connection.connectionStates.Connected) textConnection.Text = "Connected\n" + deviceType + "\n" + Math.Round(((float)deviceConfigSize/1024),2).ToString() + "kB\n" + deviceFWVersion + "\n" + deviceUid;
			else textConnection.Text = "Disconnected\n-\n-\n-\n-";

			if (newTraceboxText != "") {
				traceBox.Text = newTraceboxText + "\n" + traceBox.Text;
				newTraceboxText = "";
			}

		}

		private void frameReceiver(ref cRxFrame rxFrame) {
			if (rxFrame.scope == (byte)xwCom.SCOPE.COMMAND) {
				if (rxFrame.data[0] == (byte)xwCom.COMMANDRESPONSE.RESPONSE_STATIC_INFO) {

					deviceUid = "0x";
					for (int i = 0; i < 12; i++) deviceUid += BitConverter.ToString(new byte[] { rxFrame.data[i+1] });
					deviceConfigSize = (rxFrame.data[13] << 24) + (rxFrame.data[14] << 16) + (rxFrame.data[15] << 8) + (rxFrame.data[16]);
					deviceType = ((xwCom.HW_VERSIONS)rxFrame.data[17]).ToString();
					deviceFWVersion = rxFrame.data[18].ToString() + "." + rxFrame.data[19].ToString();

				}
			}

			if (rxFrame.scope == (byte)xwCom.SCOPE.TRACE) {
				string msg = System.Text.Encoding.Default.GetString(rxFrame.data).Substring(0,rxFrame.rxCount);
				msg = DateTime.Now.ToString("HH:mm:ss.fff") + " " + msg;
				newTraceboxText = msg;
			}
		}


		private void bTrash_Click(object sender, RoutedEventArgs e) {
			traceBox.Text = "";
        }

    }

	

}
