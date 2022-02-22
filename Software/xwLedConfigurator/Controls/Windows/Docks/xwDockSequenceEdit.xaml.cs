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

    public partial class xwDockSequenceEdit : UserControl {

        public delegate void sequenceSettingsSaved();
        public event sequenceSettingsSaved settingsSaved;

        public xwDockSequenceEdit() {
            InitializeComponent();
		}

        sequence_t sequence;

        public void show(sequence_t sequence_) {
            sequence = sequence_;
            sequenceName.Text = sequence.name;

            if (sequence.dimInfo == 0) dimMode.SelectedIndex = 1;
            else dimMode.SelectedIndex = 0;

            if (sequence.speedInfo == 0) speedMode.SelectedIndex = 1;
            else speedMode.SelectedIndex = 0;

            dimmingInfo.value = (byte)sequence.dimInfo;
            speedInfo.value = sequence.speedInfo;
            this.Visibility = Visibility.Visible;
        }

        public void hide() {
            this.Visibility = Visibility.Collapsed;
        }

        private void bAbort_Click(object sender, RoutedEventArgs e) {
            hide();
        }

        private void bSave_Click(object sender, RoutedEventArgs e) {
            if (sequenceName.Text.Length > 0) {
                sequence.name = sequenceName.Text;

                if (dimMode.SelectedIndex == 1) sequence.dimInfo = 0;
                else sequence.dimInfo = dimmingInfo.value;

                if (speedMode.SelectedIndex == 1) sequence.speedInfo = 0;
                else sequence.speedInfo = speedInfo.value;

                if (settingsSaved != null) settingsSaved();
                hide();
            }
        }

        private void speedMode_SelectionChanged(object sender, SelectionChangedEventArgs e) {
            if (speedInfo != null) {
                if (speedMode.SelectedIndex == 1) speedInfo.Visibility = Visibility.Collapsed;
                else {
                    speedInfo.value = 0x10000;
                    speedInfo.Visibility = Visibility.Visible;
                }
            }
        }

        private void dimMode_SelectionChanged(object sender, SelectionChangedEventArgs e) {
            if (dimmingInfo != null) {
                if (dimMode.SelectedIndex == 1) dimmingInfo.Visibility = Visibility.Collapsed;
                else {
                    dimmingInfo.value = 0xFF;
                    dimmingInfo.Visibility = Visibility.Visible;
                }
            }
        }
    }	

}
