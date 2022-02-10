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

    public partial class xwDockChannelEdit : UserControl {

        public delegate void channelSettingsSaved();
        public event channelSettingsSaved settingsSaved;

        public xwDockChannelEdit() {
            InitializeComponent();
		}

        channel_t channel;

        public void show(channel_t channel_) {
            channel = channel_;
            title.Text = String.Format("Channel {0} Settings", channel.index + 1);
            this.Visibility = Visibility.Visible;

            eolOption0.IsChecked = false;
            eolOption1.IsChecked = false;
            eolOption2.IsChecked = false;
            eolOption3.IsChecked = false;

            if (channel.eolOption == 0) eolOption0.IsChecked = true;
            if (channel.eolOption == 1) eolOption1.IsChecked = true;
            if (channel.eolOption == 2) eolOption2.IsChecked = true;
            if (channel.eolOption == 3) eolOption3.IsChecked = true;
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

            if (settingsSaved != null) settingsSaved();
            hide();
        }
    }	

}
