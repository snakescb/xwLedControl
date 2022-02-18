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
using Microsoft.Win32;
using Newtonsoft;
using Newtonsoft.Json;
using System.IO;

namespace xwLedConfigurator {

	public class output_t {
		public int assignment;		
    }

	public class channel_t {
		public int channelNumber;
		public int eolOption;
		public bool isRGB;
		public Color color;
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
		Simulation simulation = new Simulation();

		public xwLedWindow() {
            InitializeComponent();

			//sample sequences
			sequence_t s = new sequence_t();
			s.name = "Welcome";
			sequenceList.Add(s);
			loadSequence(0);
			addRegularChannel_Click(null, null);
			addRgbChannel_Click(null, null);

			Connection.frameReceived += this.frameReceiver;
		}

		private void bSequences_Click(object sender, RoutedEventArgs e) {
			dockEditSequence.hide();
			dockEditChannel.hide();
			dockEditObject.hide();
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

			//save all sequences to file
			if (action == xwDockSequence.sequenceManagement_t.SAVE_SEQUENCES) {
				SaveFileDialog saveFileDialog = new SaveFileDialog();
				saveFileDialog.FileName = "xwLedConfig"; // Default file name
				saveFileDialog.DefaultExt = ".xwled"; // Default file extension
				saveFileDialog.Filter = "xw led files (.xwled)|*.xwled"; // Filter files by extension
				if (saveFileDialog.ShowDialog() == true) {
					string json = JsonConvert.SerializeObject(sequenceList, Formatting.Indented);
					File.WriteAllText(saveFileDialog.FileName, json);
					MessageBox.Show("File saved successfully", "Success", MessageBoxButton.OK, MessageBoxImage.Information);
					dockSequence.hide();
				}					
			}
		}

		private void channelEvent(xwDockChannel sender, xwDockChannel.channelEvent_t request) {
			if (request == xwDockChannel.channelEvent_t.DELETE_CHANNEL) {
				if (sender.channel.isRGB) outputsUsed -= 3;
				else outputsUsed--;
				sequenceList[currentSequence].channels.Remove(sender.channel);
				reloadChannels();
			}

			if (request == xwDockChannel.channelEvent_t.EDIT_CHANNEL) {
				dockSequence.hide();
				dockEditSequence.hide();
				dockEditObject.hide();
				dockEditChannel.show(sender.channel);
			}

			if (request == xwDockChannel.channelEvent_t.GRID_OFFSET_CHANGED) {
				foreach (xwDockChannel channel in channelPanel.Children) {
					if (channel != sender) channel.setOffset(sender.gridOffset);
				}
			}
		}

		private void objectEditRequest(xwDockChannel sender, ledObject ledobject) {
			dockSequence.hide();
			dockEditChannel.hide();
			dockEditSequence.hide();
			dockEditObject.show(ledobject, sender.channel);
        }

		private void channelUpdateOffset(xwDockChannel sender, double newOffset) {
			foreach (xwDockChannel channel in channelPanel.Children) channel.setOffset(newOffset);
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
			outputsUsed = 0;
			int channelNumber = 0;
			foreach (channel_t channel in sequenceList[currentSequence].channels) {
				//Renumber the channels
				channelNumber++;
				channel.channelNumber = channelNumber;

				//create new channel dock and attach event
				xwDockChannel dock = new xwDockChannel(channel);
				dock.channelEvent += channelEvent;
				dock.objectEditRquest += objectEditRequest;

				//bookkeeping
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

		private void addRegularChannel_Click(object sender, RoutedEventArgs e) {
			addChannel(false);			
        }

		private void addRgbChannel_Click(object sender, RoutedEventArgs e) {
			addChannel(true);
		}

		private void addChannel(bool isRGB) {
			int outputsAvailable = maxNumOutputs - outputsUsed;
			int outputsRequired = 1;
			if (isRGB) outputsRequired = 3;
			if (outputsAvailable >= outputsRequired) {
				channel_t channel = new channel_t();
				channel.isRGB = isRGB;
				channel.eolOption = 0;
				channel.color = Colors.White;

				for (int i = 0; i < outputsRequired; i++) {
					output_t output = new output_t();
					output.assignment = outputsUsed + i;
					channel.outputs.Add(output);
				}

				sequenceList[currentSequence].channels.Add(channel);
				outputsUsed += outputsRequired;
				reloadChannels();
			}
		}

		protected override void OnMouseMove(MouseEventArgs e) {
			base.OnMouseMove(e);
			if (e.LeftButton == MouseButtonState.Pressed) {
				//check if drag comes from add object border elements
				Point p = e.GetPosition(dragAreaSOB);
				if ((p.X >= 0) && (p.X <= dragAreaSOB.ActualWidth) && (p.Y >= 0) && (p.Y <= dragAreaSOB.ActualHeight)) {
					DataObject data = new DataObject();
					data.SetData("sob");
					DragDrop.DoDragDrop(this, data, DragDropEffects.Copy);
				}
				else {
					p = e.GetPosition(dragAreaLOB);
					if ((p.X >= 0) && (p.X <= dragAreaLOB.ActualWidth) && (p.Y >= 0) && (p.Y <= dragAreaLOB.ActualHeight)) {
						DataObject data = new DataObject();
						data.SetData("lob");
						DragDrop.DoDragDrop(this, data, DragDropEffects.Copy);
					}
					else {
						p = e.GetPosition(dragAreaBLK);
						if ((p.X >= 0) && (p.X <= dragAreaBLK.ActualWidth) && (p.Y >= 0) && (p.Y <= dragAreaBLK.ActualHeight)) {
							DataObject data = new DataObject();
							data.SetData("blk");
							DragDrop.DoDragDrop(this, data, DragDropEffects.Copy);
						}
						else {
							p = e.GetPosition(dragAreaDIM);
							if ((p.X >= 0) && (p.X <= dragAreaDIM.ActualWidth) && (p.Y >= 0) && (p.Y <= dragAreaDIM.ActualHeight)) {
								DataObject data = new DataObject();
								data.SetData("dim");
								DragDrop.DoDragDrop(this, data, DragDropEffects.Copy);
							}
						}
					}
				}
			}
		}

		private void settings_Click(object sender, RoutedEventArgs e) {
			dockSequence.hide();
			dockEditChannel.hide();
			dockEditObject.hide();
			dockEditSequence.show(sequenceList[currentSequence]);
		}

		private void dockEditSequence_settingsSaved() {
			sequenceDispayName.Text = sequenceList[currentSequence].name;
		}

		private void dockEditObject_settingsSaved(ledObject ledobject, channel_t channel) {
			foreach (xwDockChannel dock in channelPanel.Children) {
				if (dock.channel == channel) dock.refreshGrid();
			}
		}

		private void dockEditChannel_settingsSaved(channel_t channel) {
			foreach (xwDockChannel dock in channelPanel.Children) {
				if (dock.channel == channel) dock.settingsChanged();
			}
		}


        private void pause_Click(object sender, RoutedEventArgs e) {			
			Connection.putFrame((byte)xwCom.SCOPE.LED, new byte[] { (byte)xwCom.LED.ENABLE_DISABLE, 0 });
			simulation.stop();
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

		//handling Keypresses
		private void UserControl_Loaded(object sender, RoutedEventArgs e) {
			var window = Window.GetWindow(this);
			window.KeyDown += HandleKeyPress;
		}

		private void HandleKeyPress(object sender, KeyEventArgs e) {
			foreach (xwDockChannel channel in channelPanel.Children) channel.keyPress(e);
		}

		//simulation control
        private void startSimulation_Click(object sender, RoutedEventArgs e) {
			simulation.start(sequenceList[currentSequence]);
        }

		private void frameReceiver(ref cRxFrame rxFrame) {
			simulation.dataReceiver(ref rxFrame);
		}

	}



}
