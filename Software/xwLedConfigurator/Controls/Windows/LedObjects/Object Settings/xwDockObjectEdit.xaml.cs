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

    public partial class xwDockObjectEdit : UserControl {

        public delegate void objectSettingsSaved(ledObject ledobject, channel_t channel);
        public event objectSettingsSaved settingsSaved;


        public xwDockObjectEdit() {
            InitializeComponent();
		}

        ledObject ledobject;
        channel_t channel;

        public void show(ledObject ledobject, channel_t channel) {

            this.ledobject = ledobject;
            this.channel = channel;
            sobSettingsPanel.Visibility = Visibility.Collapsed; 
            lobSettingsPanel.Visibility = Visibility.Collapsed;
            dimSettingsPanel.Visibility = Visibility.Collapsed;
            blkSettingsPanel.Visibility = Visibility.Collapsed;

            starttime.Text = ledobject.starttime.ToString();
            runtime.Text = ledobject.length.ToString();

            if (channel.isRGB) {
                bwBand.Visibility = Visibility.Collapsed;
                rgbBand.Visibility = Visibility.Visible;
            }
            else {
                bwColor.Color = channel.color;
                bwBand.Visibility = Visibility.Visible;
                rgbBand.Visibility = Visibility.Collapsed;
            }

            if (ledobject is sob) sobSettingsPanel.show((sob)ledobject, channel);
            if (ledobject is lob) lobSettingsPanel.show((lob)ledobject, channel);
            if (ledobject is dim) dimSettingsPanel.show((dim)ledobject, channel);
            if (ledobject is blk) blkSettingsPanel.show((blk)ledobject, channel);


            this.Visibility = Visibility.Visible;
        }

        public void hide() {
            this.Visibility = Visibility.Collapsed;
        }

        private void bAbort_Click(object sender, RoutedEventArgs e) {
            hide();
        }

        private void bSave_Click(object sender, RoutedEventArgs e) {

            //validate
            double start, length;
            if (!double.TryParse(starttime.Text, out start)) return;
            if (!double.TryParse(runtime.Text, out length)) return;
            if (length > 0xFFFF) return;

            ledobject.starttime = start;
            ledobject.length = length;

            if (ledobject is sob) if (!sobSettingsPanel.save((sob)ledobject, channel)) return;
            if (ledobject is lob) if (!lobSettingsPanel.save((lob)ledobject, channel)) return;
            if (ledobject is dim) if (!dimSettingsPanel.save((dim)ledobject, channel)) return;
            if (ledobject is blk) if (!blkSettingsPanel.save((blk)ledobject, channel)) return;


            if (settingsSaved != null) settingsSaved(ledobject, channel);
            hide();
        }
    }	

}
