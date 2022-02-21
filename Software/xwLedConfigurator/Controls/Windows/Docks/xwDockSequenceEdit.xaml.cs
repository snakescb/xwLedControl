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
                sequence.dimInfo = dimmingInfo.value;
                sequence.speedInfo = speedInfo.value;
                if (settingsSaved != null) settingsSaved();
                hide();
            }
        }
    }	

}
