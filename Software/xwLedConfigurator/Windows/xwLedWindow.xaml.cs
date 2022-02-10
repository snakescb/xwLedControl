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

	public class ledObject_t {
		public int size;
		public byte[] data;
    }

	public class output_t {
		public int index;
		public List<ledObject_t> ledObjects = new List<ledObject_t>();
    }

	public class channel_t {
		public int index;
		public int eolOption;
		public bool isRGB;
		public long length_ms;
		public List<output_t> outputs = new List<output_t>();
    }

	public class sequence_t {
		public string name;
		public int dimInfo;
		public int speedInfo;
		public List<channel_t> channels = new List<channel_t>();
    }

    public partial class xwLedWindow : UserControl {

		const int maxNumOutputs = 24;

		int currentSequence = 0;
		int outputsUsed = 0;
		List<sequence_t> sequenceList = new List<sequence_t>();

		public xwLedWindow() {
            InitializeComponent();

			//sample sequences
			sequence_t s = new sequence_t();
			s.name = "Welcome";
			sequenceList.Add(s);
			loadSequence(0);

			Connection.frameReceived += this.frameReceiver;
		}

		private void bSequences_Click(object sender, RoutedEventArgs e) {
			dockEditSequence.hide();
			dockEditChannel.hide();
			dockSequence.show(sequenceList);
        }

		private void dockSequence_sequenceManagement(xwDockSequence.sequenceManagement_t action, int sequenceIndex, string sequenceName) {
			//add new sequence to list
			if (action == xwDockSequence.sequenceManagement_t.NEW_SEQUENCE) createSequence(sequenceName);

			//delete sequence from list
			if (action == xwDockSequence.sequenceManagement_t.DELETE_SEQUENCE) {
				sequenceList.RemoveAt(sequenceIndex);
				dockSequence.show(sequenceList);
			}

			//load sequence from list
			if (action == xwDockSequence.sequenceManagement_t.LOAD_SEQUENCE) {
				loadSequence(sequenceIndex);
				dockSequence.hide();
			}
		}

		private void channelEvent(xwDockChannel sender, xwDockChannel.channelEvent_t request, int index) {
			if (request == xwDockChannel.channelEvent_t.DELETE_CHANNEL) {
				if (sequenceList[currentSequence].channels[index].isRGB) outputsUsed -= 3;
				else outputsUsed--;
				sequenceList[currentSequence].channels.RemoveAt(index);
				for (int i = 0; i < sequenceList[currentSequence].channels.Count; i++) sequenceList[currentSequence].channels[i].index = i;
				reloadChannels();
			}

			if (request == xwDockChannel.channelEvent_t.EDIT_CHANNEL) {
				dockEditChannel.show(sequenceList[currentSequence].channels[index]);			
			}
		}

		private void createSequence(string sequenceName) {
			sequence_t newSequence = new sequence_t();
			newSequence.name = sequenceName;
			sequenceList.Add(newSequence);
			dockSequence.hide();
			currentSequence = sequenceList.Count - 1;
			loadSequence(currentSequence);
		}

		private void loadSequence(int sequenceIndex) {
			currentSequence = sequenceIndex;
			sequence_t sequence = sequenceList[currentSequence];			
			sequenceDispayName.Text = sequence.name;
			reloadChannels();
		}

		private void reloadChannels() {
			channelPanel.Children.Clear();
			foreach (channel_t channel in sequenceList[currentSequence].channels) {
				xwDockChannel dock = new xwDockChannel(channel);
				dock.channelEvent += channelEvent;
				channelPanel.Children.Add(dock);
			}
			outputsDisplay.Text = String.Format("Outputs: {0} / {1}", outputsUsed, maxNumOutputs);
		}

		private void zoomIn_Click(object sender, RoutedEventArgs e) {
			foreach (xwDockChannel channel in channelPanel.Children) channel.zoom(true);
		}

		private void zoomOut_Click(object sender, RoutedEventArgs e) {
			foreach (xwDockChannel channel in channelPanel.Children) channel.zoom(false);
		}

		private void addChannel_Click(object sender, RoutedEventArgs e) {
			if (outputsUsed < maxNumOutputs) {
				channel_t channel = new channel_t();
				channel.index = sequenceList[currentSequence].channels.Count;
				channel.length_ms = 63000;
				channel.isRGB = false;
				channel.eolOption = 0;
				sequenceList[currentSequence].channels.Add(channel);
				outputsUsed++;
				reloadChannels();
			}			
        }

		protected override void OnMouseMove(MouseEventArgs e) {
			base.OnMouseMove(e);
			if (e.LeftButton == MouseButtonState.Pressed) {
				//check if drag comes from add object border element
				Point p = e.GetPosition(dragArea);
				if ((p.X >= 0) && (p.X <= dragArea.ActualWidth) && (p.Y >= 0) && (p.Y <= dragArea.ActualHeight)) {
					DataObject data = new DataObject();
					data.SetData("Object", this);
					DragDrop.DoDragDrop(this, data, DragDropEffects.Copy);
				}
			}
		}

		private void settings_Click(object sender, RoutedEventArgs e) {
			dockSequence.hide();
			dockEditChannel.hide();
			dockEditSequence.show(sequenceList[currentSequence]);
		}

		private void dockEditSequence_settingsSaved() {
			sequenceDispayName.Text = sequenceList[currentSequence].name;
		}

		private void frameReceiver(ref cRxFrame rxFrame) {

		}

        private void dockEditChannel_settingsSaved() {
			reloadChannels();
		}
    }

	

}
