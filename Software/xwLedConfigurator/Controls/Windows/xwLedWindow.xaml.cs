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

	public class output_t {
		public int assignment;		
    }

	public class channel_t {
		public int index;
		public int eolOption;
		public bool isRGB;
		public long length_ms;
		public List<ledObject> ledObjects = new List<ledObject>();
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
			addChannel_Click(null, null);
			addRgbChannel_Click(null, null);

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
			//removing oll ledobjects frin grids elements must be done before the are cleared, else the object cannot be added to a new control
			foreach (xwDockChannel ch in channelPanel.Children) ch.cleanup();

			channelPanel.Children.Clear();
			outputsUsed = 0;
			foreach (channel_t channel in sequenceList[currentSequence].channels) {
				xwDockChannel dock = new xwDockChannel(channel);
				dock.channelEvent += channelEvent;
				if (channel.isRGB) outputsUsed += 3;
				else outputsUsed++;
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
				channel.isRGB = false;
				channel.index = sequenceList[currentSequence].channels.Count;
				channel.length_ms = 63000;				
				channel.eolOption = 0;

				output_t output = new output_t();
				output.assignment = outputsUsed;
				channel.outputs.Add(output);

				sequenceList[currentSequence].channels.Add(channel);
				outputsUsed++;
				reloadChannels();
			}			
        }

		private void addRgbChannel_Click(object sender, RoutedEventArgs e) {
			if (outputsUsed < (maxNumOutputs-2)) {
				channel_t channel = new channel_t();
				channel.isRGB = true;
				channel.index = sequenceList[currentSequence].channels.Count;
				channel.length_ms = 63000;				
				channel.eolOption = 0;

				output_t output = new output_t();
				output.assignment = outputsUsed;
				channel.outputs.Add(output);

				output = new output_t();
				output.assignment = outputsUsed+1;
				channel.outputs.Add(output);

				output = new output_t();
				output.assignment = outputsUsed + 2;
				channel.outputs.Add(output);

				sequenceList[currentSequence].channels.Add(channel);
				outputsUsed += 3;
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

        private void pause_Click(object sender, RoutedEventArgs e) {
			Connection.putFrame((byte)xwCom.SCOPE.LED, new byte[] { (byte)xwCom.LED.ENABLE_DISABLE, 0 });
        }

        private void startdevice_Click(object sender, RoutedEventArgs e) {
			Connection.putFrame((byte)xwCom.SCOPE.LED, new byte[] { (byte)xwCom.LED.ENABLE_DISABLE, 1 });
		}

        private void dockEditChannel_testoutput(int index) {
			byte[] data = new byte[maxNumOutputs + 1];
			data[0] = (byte)xwCom.LED.MANUAL_BRIGHTNESS;
			if (index >= 0) data[index + 1] = 200;
			Connection.putFrame((byte)xwCom.SCOPE.LED, data);
		}
        
    }

	

}
