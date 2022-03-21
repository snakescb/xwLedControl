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
using System.Threading.Tasks;

namespace xwLedConfigurator {

	public class output_t {
		public int assignment;		
    }

	public class channel_t {
		public int channelNumber;
		public int eolOption;
		public bool isRGB;
		public byte channelDim;
		public Color color;
		public List<ledObject> ledObjects = new List<ledObject>();
		public List<output_t> outputs = new List<output_t>();
    }

	public class sequence_t {
		public string name;
		public byte dimInfo;
		public uint speedInfo;
		public List<channel_t> channels = new List<channel_t>();
    }

    public partial class xwLedWindow : UserControl {

		public delegate void eSimAuxRequest(bool show);
		public event eSimAuxRequest simAuxRequest;
		public delegate void eDownloadRequest(List<sequence_t> sequenceList);
		public event eDownloadRequest downloadRequest;
		public delegate void eUploadRequest(List<sequence_t> sequenceList);
		public event eUploadRequest uploadRequest;

		const int maxNumOutputs = 24;

		sequence_t currentSequence = null;
		int outputsUsed = 0;		
		List<sequence_t> sequenceList = new List<sequence_t>();
		Simulation simulation = new Simulation();
		ledObject copyPasteObject = null;
		xwDockChannel copyPasteProvider = null;
		bool copyPasteCutOperation = false;

		public xwLedWindow() {
            InitializeComponent();
			currentSequence = null;
			Connection.frameReceived += this.frameReceiver;
		}

		private void bSequences_Click(object sender, RoutedEventArgs e) {
			dockEditSequence.hide();
			dockEditChannel.hide();
			dockEditObject.hide();
			dockSequence.show(sequenceList);
        }

		private void dockSequence_sequenceManagement(xwDockSequence.sequenceManagement_t action, sequence_t sequence) {
			this.Dispatcher.BeginInvoke(new Action(() => {
				//add new sequence to list
				if (action == xwDockSequence.sequenceManagement_t.NEW_SEQUENCE) createSequence(sequence);

				//delete sequence from list
				if (action == xwDockSequence.sequenceManagement_t.DELETE_SEQUENCE) {
					//delete sequence			
					sequenceList.Remove(sequence);

					//if this was the active sequence, load another one.
					//if it was the last one, show empty screen
					if (sequence == currentSequence) {
						if (sequenceList.Count == 0) {
							currentSequence = null;
							sequenceDispayName.Text = "-";
							channelPanel.Children.Clear();
							outputsDisplay.Text = String.Format("Outputs: {0} / {1}", 0, maxNumOutputs);
							welcome.Visibility = Visibility.Visible;
							dockSequence.hide();
						}
						else {
							loadSequence(sequenceList[0]);
							dockSequence.show(sequenceList);
						}
					}
					else dockSequence.show(sequenceList);
				}

				//load sequence from list
				if (action == xwDockSequence.sequenceManagement_t.LOAD_SEQUENCE) {
					loadSequence(sequence);
					dockSequence.hide();
				}

				//save all sequences to file
				if (action == xwDockSequence.sequenceManagement_t.SAVE_TO_FILE) {
					SaveFileDialog saveFileDialog = new SaveFileDialog();
					saveFileDialog.FileName = "xwLedConfig"; // Default file name
					saveFileDialog.DefaultExt = ".xwled"; // Default file extension
					saveFileDialog.Filter = "xwled files | *.xwled"; // Filter files by extension
					if (saveFileDialog.ShowDialog() == true) {

						JsonSerializerSettings settings = new JsonSerializerSettings { TypeNameHandling = TypeNameHandling.Auto, Formatting = Formatting.Indented };
						string json = JsonConvert.SerializeObject(sequenceList, settings);
						File.WriteAllText(saveFileDialog.FileName, json);
						MessageBox.Show("File saved successfully", "Success", MessageBoxButton.OK, MessageBoxImage.Information);
						dockSequence.hide();
					}					
				}

				//load sequences from file
				if (action == xwDockSequence.sequenceManagement_t.LOAD_FROM_FILE) {
					OpenFileDialog openFileDialog = new OpenFileDialog();
					openFileDialog.DefaultExt = ".xwled"; // Default file extension
					openFileDialog.Filter = "xwled files | *.xwled"; // Filter files by extension
					if (openFileDialog.ShowDialog() == true) {

						string filecontent = File.ReadAllText(openFileDialog.FileName);
						try {
							JsonSerializerSettings settings = new JsonSerializerSettings { TypeNameHandling = TypeNameHandling.Auto, Formatting = Formatting.Indented };
							sequenceList = JsonConvert.DeserializeObject<List<sequence_t>>(filecontent, settings);
							loadSequence(sequenceList[0]);
							dockSequence.hide();
						}
						catch {
							MessageBox.Show("Error while reading file", "Invalid file", MessageBoxButton.OK, MessageBoxImage.Error);
						}				
					}
				}

				//download to device
				if (action == xwDockSequence.sequenceManagement_t.SAVE_TO_DEVICE) {
					//pause device and simulation first
					Connection.putFrame((byte)xwCom.SCOPE.LED, new byte[] { (byte)xwCom.LED.ENABLE_DISABLE, 0 });
					simulation.stop();
					if (simAuxRequest != null) simAuxRequest(false);
					//start download usercontrol
					if (downloadRequest != null) downloadRequest(sequenceList);
					dockSequence.hide();
				}

				//upload from device
				if (action == xwDockSequence.sequenceManagement_t.LOAD_FROM_DEVICE) {
					//pause device and simulation first
					Connection.putFrame((byte)xwCom.SCOPE.LED, new byte[] { (byte)xwCom.LED.ENABLE_DISABLE, 0 });
					simulation.stop();
					if (simAuxRequest != null) simAuxRequest(false);
					//start download usercontrol
					if (uploadRequest != null) uploadRequest(sequenceList);
					dockSequence.hide();
				}
			}));
		}

		private void channelEvent(xwDockChannel sender, xwDockChannel.channelEvent_t request) {
			if (request == xwDockChannel.channelEvent_t.DELETE_CHANNEL) {
				if (sender.channel.isRGB) outputsUsed -= 3;
				else outputsUsed--;				
				currentSequence.channels.Remove(sender.channel);
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

		private void objectCopyPasteRequest(xwDockChannel sender, xwDockChannel.channelCopyPasteEvent_t eventType, ledObject ledobject) {
			if (eventType == xwDockChannel.channelCopyPasteEvent_t.OBJECT_COPY) {
				copyPasteProvider = sender;
				copyPasteObject = ledobject;
				copyPasteCutOperation = false;
			}
			if (eventType == xwDockChannel.channelCopyPasteEvent_t.OBJECT_CUT) {
				copyPasteProvider = sender;
				copyPasteObject = ledobject;
				copyPasteCutOperation = true;
			}
			if (eventType == xwDockChannel.channelCopyPasteEvent_t.OBJECT_PASTE) {
				if (copyPasteObject != null) {
					sender.pasteObject(copyPasteObject);
					if (copyPasteCutOperation) {
						copyPasteProvider.cutObject(copyPasteObject);
						copyPasteObject = null;
						copyPasteProvider = null;
					}
				}
			}
		}

		private void channelUpdateOffset(xwDockChannel sender, double newOffset) {
			foreach (xwDockChannel channel in channelPanel.Children) channel.setOffset(newOffset);
		}

		private void createSequence(sequence_t newSequence) {
			//hide welcome message
			welcome.Visibility = Visibility.Collapsed;

			//update sequence settings
			newSequence.dimInfo = 0xFF;
			newSequence.speedInfo = 0x10000;
			sequenceList.Add(newSequence);
			dockSequence.hide();
			loadSequence(newSequence);
		}

		private void loadSequence(sequence_t sequence) {
			currentSequence = sequence;		
			sequenceDispayName.Text = sequence.name;
			reloadChannels();
		}

		private void reloadChannels() {

			//hide welcome message
			welcome.Visibility = Visibility.Collapsed;

			//if there are already channels loaded, reuse zoom and offset for the new channels
			int zoom = -1;
			double offset = -1;
			if (channelPanel.Children.Count > 0) {
				zoom = ((xwDockChannel)channelPanel.Children[0]).currentZoomLevel;
				offset = ((xwDockChannel)channelPanel.Children[0]).gridOffset;
			}

			channelPanel.Children.Clear();
			outputsUsed = 0;
			int channelNumber = 0;

			foreach (channel_t channel in currentSequence.channels) {
				//Renumber the channels
				channelNumber++;
				channel.channelNumber = channelNumber;

				//create new channel dock and attach event
				xwDockChannel dock;
				if (zoom == -1)  dock = new xwDockChannel(channel);
				else  dock = new xwDockChannel(channel, offset, zoom);
				dock.channelEvent += channelEvent;
				dock.objectEditRquest += objectEditRequest;
				dock.objectCopyPasteRequest += objectCopyPasteRequest;

				//bookkeeping
				if (channel.isRGB) outputsUsed += 3;
				else outputsUsed++;
				channelPanel.Children.Add(dock);
			}
			outputsDisplay.Text = String.Format("Outputs: {0} / {1}", outputsUsed, maxNumOutputs);			
		}

		public  void uploadFinished() {			
			this.Dispatcher.BeginInvoke(new Action(() => {
				if (sequenceList.Count > 0) {
					loadSequence(sequenceList[0]);
					reloadChannels();
				}
				else {
					currentSequence = null;
					sequenceDispayName.Text = "No Sequence";
					outputsDisplay.Text = String.Format("Outputs: {0} / {1}", 0, maxNumOutputs);
					channelPanel.Children.Clear();
				}				
			}));			
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
			if (currentSequence == null) return;
			int outputsAvailable = maxNumOutputs - outputsUsed;
			int outputsRequired = 1;
			if (isRGB) outputsRequired = 3;
			if (outputsAvailable >= outputsRequired) {
				channel_t channel = new channel_t();
				channel.isRGB = isRGB;
				channel.eolOption = 0;
				channel.color = Colors.White;
				channel.channelDim = 0xFF;

				for (int i = 0; i < outputsRequired; i++) {
					output_t output = new output_t();
					output.assignment = outputsUsed + i;
					channel.outputs.Add(output);
				}

				currentSequence.channels.Add(channel);
				outputsUsed += outputsRequired;
				reloadChannels();
			}
		}

		protected override void OnMouseLeftButtonDown(MouseButtonEventArgs e) {
			if (dragAreaSOB.IsMouseOver) {
				DataObject data = new DataObject();
				data.SetData("sob");
				DragDrop.DoDragDrop(this, data, DragDropEffects.Copy);
			}

			if (dragAreaLOB.IsMouseOver) {
				DataObject data = new DataObject();
				data.SetData("lob");
				DragDrop.DoDragDrop(this, data, DragDropEffects.Copy);
			}

			if (dragAreaBLK.IsMouseOver) {
				DataObject data = new DataObject();
				data.SetData("blk");
				DragDrop.DoDragDrop(this, data, DragDropEffects.Copy);
			}

			if (dragAreaDIM.IsMouseOver) {
				DataObject data = new DataObject();
				data.SetData("dim");
				DragDrop.DoDragDrop(this, data, DragDropEffects.Copy);
			}
		}

		protected override void OnMouseMove(MouseEventArgs e) {
			base.OnMouseMove(e);

			if (Mouse.LeftButton == MouseButtonState.Released) {
				if (dragAreaSOB.IsMouseOver || dragAreaLOB.IsMouseOver || dragAreaBLK.IsMouseOver || dragAreaDIM.IsMouseOver) Cursor = Cursors.Hand;
				else Cursor = Cursors.Arrow;
			}
			else Cursor = Cursors.Arrow;
		}

		private void settings_Click(object sender, RoutedEventArgs e) {
			if (currentSequence == null) return;
			dockSequence.hide();
			dockEditChannel.hide();
			dockEditObject.hide();
			dockEditSequence.show(currentSequence);
		}

		private void dockEditSequence_settingsSaved() {
			sequenceDispayName.Text = currentSequence.name;
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
			if(simAuxRequest != null) simAuxRequest(false);
		}

        private void startdevice_Click(object sender, RoutedEventArgs e) {
			Connection.putFrame((byte)xwCom.SCOPE.LED, new byte[] { (byte)xwCom.LED.ENABLE_DISABLE, 1 });
		}

        private void dockEditChannel_testoutput(int index, byte brightness) {
			byte[] data = new byte[maxNumOutputs + 1];
			data[0] = (byte)xwCom.LED.MANUAL_BRIGHTNESS;
			if (index >= 0) data[index + 1] = brightness;
			Connection.putFrame((byte)xwCom.SCOPE.LED, data);
		}

		public void HandleKeyPress(object sender, KeyEventArgs e) {
			foreach (xwDockChannel channel in channelPanel.Children) channel.keyPress(e);
		}

		//simulation control
        private void startSimulation_Click(object sender, RoutedEventArgs e) {
			if (currentSequence == null) return;
			simulation.start(currentSequence);
			if (simAuxRequest != null) simAuxRequest(true);
		}

		private void frameReceiver(ref cRxFrame rxFrame) {
			simulation.dataReceiver(ref rxFrame);			
		}

	}



}
