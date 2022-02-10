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

namespace xwLedConfigurator {

	public class cSequence {
		public string sequenceName = "";
    }

    public partial class xwLedWindow : UserControl {

		const int maxSequences = 8;

		List<cSequence> sequenceList = new List<cSequence>();

		public xwLedWindow() {
            InitializeComponent();

			//sample sequences
			cSequence s = new cSequence();
			s.sequenceName = "FAI";
			sequenceList.Add(s);
			s = new cSequence();
			s.sequenceName = "Night Rider";
			sequenceList.Add(s);

			//timer for gui update
			System.Windows.Forms.Timer guiUpdate = new System.Windows.Forms.Timer();
			guiUpdate.Tick += new EventHandler(updateGui);
			guiUpdate.Interval = 50;
			guiUpdate.Enabled = true;

			Connection.frameReceived += this.frameReceiver;
		}

		private void updateGui(Object myObject, EventArgs myEventArgs) {


		}

		private void frameReceiver(ref cRxFrame rxFrame) {

		}

        private void bSequences_Click(object sender, RoutedEventArgs e) {
			dockSequence.show(sequenceList);
        }

        private void dockSequence_sequenceManagement(xwDockSequence.sequenceManagement_t action, string sequenceName) {
			//add new sequence to list
			if (action == xwDockSequence.sequenceManagement_t.NEW_SEQUENCE) {
				foreach (cSequence s in sequenceList) {
					if (s.sequenceName == sequenceName) {
						MessageBox.Show("Sequence " + sequenceName + " already exists");
						return;
					}
				}

				cSequence newSequence = new cSequence();
				newSequence.sequenceName= sequenceName;
				sequenceList.Add(newSequence);
				dockSequence.show(sequenceList);
			}

			//delete sequence from list
			if (action == xwDockSequence.sequenceManagement_t.DELETE_SEQUENCE) {
				for (int i = 0; i < sequenceList.Count; i++) {
					cSequence s = sequenceList[i];
					if (s.sequenceName == sequenceName) {
						sequenceList.RemoveAt(i);
						break;
					}
				}
				dockSequence.show(sequenceList);
			}
        }
    }

	

}
