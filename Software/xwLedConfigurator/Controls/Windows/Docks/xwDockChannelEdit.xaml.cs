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
using System.ComponentModel;

namespace xwLedConfigurator {

    public partial class xwDockChannelEdit : UserControl, INotifyPropertyChanged {

        public delegate void channelSettingsSaved(channel_t channel);
        public event channelSettingsSaved settingsSaved;

        public delegate void testOutputRequest(int i, byte brightness);
        public event testOutputRequest testoutput;

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName) {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }

        public Visibility rgbVisibility {
            get {
                if (channel != null) if (channel.isRGB) return Visibility.Visible;
                return Visibility.Collapsed;
            }
        }

        public Visibility bwVisibility {
            get {
                if (channel != null) if (channel.isRGB) return Visibility.Collapsed;
                return Visibility.Visible;
            }
        }

        public Color channelColor {
            get { return currentChannelColor; }
        }

        public xwDockChannelEdit() {
            InitializeComponent();
		}

        channel_t channel;
        Color currentChannelColor = Colors.White;

        public void show(channel_t channel) {
            this.channel = channel;
            title.Text = String.Format("Channel {0} Settings", channel.channelNumber);
            OnPropertyChanged("rgbVisibility");
            OnPropertyChanged("bwVisibility");                        
            this.Visibility = Visibility.Visible;

            eolOption0.IsChecked = false;
            eolOption1.IsChecked = false;
            eolOption2.IsChecked = false;
            eolOption3.IsChecked = false;

            currentChannelColor = channel.color;
            channelColorPicker.SelectedColor = currentChannelColor;
            OnPropertyChanged("channelColor");

            if (channel.eolOption == 0) eolOption0.IsChecked = true;
            if (channel.eolOption == 1) eolOption1.IsChecked = true;
            if (channel.eolOption == 2) eolOption2.IsChecked = true;
            if (channel.eolOption == 3) eolOption3.IsChecked = true;

            assignmentSelectorR.SelectedIndex = channel.outputs[0].assignment;

            if (channel.isRGB) {
                assignmentSelectorG.SelectedIndex = channel.outputs[1].assignment;
                assignmentSelectorB.SelectedIndex = channel.outputs[2].assignment;
            }

            dimming.value = channel.channelDim;
        }

        public void hide() {
            this.Visibility = Visibility.Collapsed;
        }

        private void bAbort_Click(object sender, RoutedEventArgs e) {
            hide();
        }

        private void bSave_Click(object sender, RoutedEventArgs e) {
            if (eolOption0.IsChecked == true) channel.eolOption = 0;
            if (eolOption1.IsChecked == true) channel.eolOption = 1;
            if (eolOption2.IsChecked == true) channel.eolOption = 2;
            if (eolOption3.IsChecked == true) channel.eolOption = 3;

            channel.outputs[0].assignment = assignmentSelectorR.SelectedIndex;

            if (channel.isRGB) {
                channel.outputs[1].assignment = assignmentSelectorG.SelectedIndex;
                channel.outputs[2].assignment = assignmentSelectorB.SelectedIndex;
            }
            else {
                //channel color: Color is changed to hsv, the brightness is increase to maximum, and then converted back to rgb
                hsvColor hsv = new hsvColor(channelColorPicker.SelectedColor);
                hsv.value = 1;
                channel.color = hsv.toRGB();
            }

            channel.channelDim = dimming.value;

            if (settingsSaved != null) settingsSaved(channel);
            hide();
        }

        private void bTest_Click(object sender, RoutedEventArgs e) {

            Button button = (Button)sender;
            string channelstring = button.Name.Substring(button.Name.Length - 1, 1);

            if (button.ClickMode == ClickMode.Press) {
                button.ClickMode = ClickMode.Release;

                if (testoutput != null) {
                    if (channelstring == "R") testoutput(assignmentSelectorR.SelectedIndex, dimming.value);
                    if (channelstring == "G") testoutput(assignmentSelectorG.SelectedIndex, dimming.value);
                    if (channelstring == "B") testoutput(assignmentSelectorB.SelectedIndex, dimming.value);
                }
            }
            else {
                button.ClickMode = ClickMode.Press;
                if (testoutput != null) testoutput(-1, 0);
            }
        }

        private void channelColorPicker_ColorChanged(object sender, RoutedEventArgs e) {
            if(channel != null) currentChannelColor = channelColorPicker.SelectedColor;
            OnPropertyChanged("channelColor");
        }
    }	

}
