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

    public partial class blkSettings : UserControl {

        public blkSettings() {
            InitializeComponent();
		}

        blk ledobject;
        channel_t channel;

        public void show(blk ledobject, channel_t channel) {

            this.ledobject = ledobject;
            this.channel = channel; 

            if (channel.isRGB) {
                rgbSelectors.Visibility = Visibility.Visible;
                bwSelectors.Visibility = Visibility.Collapsed;
                colorPicker1.SelectedColor = ledobject.colorOn;
                colorPicker2.SelectedColor = ledobject.colorOff;
            }
            else {
                rgbSelectors.Visibility = Visibility.Collapsed;
                bwSelectors.Visibility = Visibility.Visible;

                hsvColor hsvColor = new hsvColor(ledobject.colorOn);
                bwSelector1.baseColor = channel.color;
                bwSelector1.value = (byte) (hsvColor.value * 255);
                hsvColor = new hsvColor(ledobject.colorOff);
                bwSelector2.baseColor = channel.color;
                bwSelector2.value = (byte)(hsvColor.value * 255);
            }

            periodSetting.Text = ledobject.period.ToString();
            dutyCycleSelector.baseColor = Colors.Gray;
            dutyCycleSelector.baseColorStart = Colors.Gray;
            dutyCycleSelector.value = (byte)((ledobject.dutycycle * 255) / 100);

            this.Visibility = Visibility.Visible;
        }

        public bool save(blk ledobject, channel_t channel) {

            //validate
            double period;
            if (!double.TryParse(periodSetting.Text, out period)) return false;
            if (period > 0xFFFF) return false;

            ledobject.period = period;
            ledobject.dutycycle = (dutyCycleSelector.value * 100) / 255;

            if (channel.isRGB) {
                ledobject.colorOn = colorPicker1.SelectedColor;
                ledobject.colorOff = colorPicker2.SelectedColor;
            }
            else {
                //adjust brightness of channel color
                hsvColor ledObjectColorOn = new hsvColor(ledobject.colorOn);
                hsvColor ledObjectColorOff = new hsvColor(ledobject.colorOff);
                hsvColor channelColor = new hsvColor(channel.color);

                ledObjectColorOn.hue = channelColor.hue;
                ledObjectColorOn.saturation = channelColor.saturation;
                ledObjectColorOn.value = (double)bwSelector1.value / 255;
                ledObjectColorOff.hue = channelColor.hue;
                ledObjectColorOff.saturation = channelColor.saturation;
                ledObjectColorOff.value = (double)bwSelector2.value / 255;
                ledobject.colorOn = ledObjectColorOn.toRGB();
                ledobject.colorOff = ledObjectColorOff.toRGB();
            }

            return true;
        }

    }	

}
