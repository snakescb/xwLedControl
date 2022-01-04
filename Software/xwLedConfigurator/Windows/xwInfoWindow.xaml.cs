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
    /// <summary>
    /// Interaktionslogik für xwInfoControl.xaml
    /// </summary>
    /// 


    public partial class xwInfoWindow : UserControl {

		Connection comPort = null;
		public List<string> lastAvailablePorts = new List<string>();

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

		}

		public void setConnection(ref Connection connection) {
			comPort = connection;

			//comport select automatic mode
			comPortMode.SelectedIndex = 0;

			//timer for gui update
			System.Windows.Forms.Timer guiUpdate = new System.Windows.Forms.Timer();
			guiUpdate.Tick += new EventHandler(updateGui);
			guiUpdate.Interval = 100;
			guiUpdate.Enabled = true;
		}

		private void updateGui(Object myObject, EventArgs myEventArgs) {

			//update current port combobox if the list of avilable ports has changed, or the currently selected port has changed
			if ((lastAvailablePorts.Count != comPort.availablePorts.Count) || (comPortCurrent.SelectedItem.ToString() != comPort.currentPort)) {

				lastAvailablePorts = new List<string>(comPort.availablePorts);

				comPortCurrent.Items.Clear();
				for (int i = 0; i < comPort.availablePorts.Count; i++) {
					comPortCurrent.Items.Add(comPort.availablePorts[i]);
					if (comPort.availablePorts[i] == comPort.currentPort) {
						comPortCurrent.SelectedIndex = i;
					}
				}
			}

			textConnection.Text = comPort.connectioState.ToString();

		}

        private void comPortMode_SelectionChanged(object sender, SelectionChangedEventArgs e) {
			if (comPortMode.SelectedIndex == 0) {
				comPort.setMode(Connection.connectionModes.Auto);
				comPortCurrent.IsEnabled = false;
			}
			else {
				comPort.setMode(Connection.connectionModes.Manual);
				comPortCurrent.IsEnabled = true;
			}
		}

        private void comPortCurrent_SelectionChanged(object sender, SelectionChangedEventArgs e) {
			if (comPort.connectioMode == Connection.connectionModes.Manual) {
                if (comPortCurrent.SelectedItem != null) comPort.setPort(comPortCurrent.SelectedItem.ToString());
            }
        }

        private void Button_Click(object sender, RoutedEventArgs e) {
			comPort.intializeBootloader();
        }
    }

	

}
