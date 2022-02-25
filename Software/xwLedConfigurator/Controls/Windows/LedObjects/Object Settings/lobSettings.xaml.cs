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

    public partial class lobSettings : UserControl {

        public lobSettings() {
            InitializeComponent();
		}

        lob ledobject;
        channel_t channel;

        public void show(lob ledobject, channel_t channel) {

            this.ledobject = ledobject;
            this.channel = channel; 

            if (channel.isRGB) {
                rgbSelectors.Visibility = Visibility.Visible;
                bwSelectors.Visibility = Visibility.Collapsed;
                colorPicker1.SelectedColor = ledobject.colorStart;
                colorPicker2.SelectedColor = ledobject.colorStop;
            }
            else {
                rgbSelectors.Visibility = Visibility.Collapsed;
                bwSelectors.Visibility = Visibility.Visible;

                hsvColor hsvColor = new hsvColor(ledobject.colorStart);
                bwSelector1.baseColor = channel.color;
                bwSelector1.value = (byte) (hsvColor.value * 255);
                hsvColor = new hsvColor(ledobject.colorStop);
                bwSelector2.baseColor = channel.color;
                bwSelector2.value = (byte)(hsvColor.value * 255);
            }            


            this.Visibility = Visibility.Visible;
        }

        public bool save(lob ledobject, channel_t channel) {


            if (channel.isRGB) {
                ledobject.colorStart = colorPicker1.SelectedColor;
                ledobject.colorStop = colorPicker2.SelectedColor;
            }
            else {
                //adjust brightness of channel color
                hsvColor ledObjectColorStart = new hsvColor(ledobject.colorStart);
                hsvColor ledObjectColorStop = new hsvColor(ledobject.colorStop);
                hsvColor channelColor = new hsvColor(channel.color);

                ledObjectColorStart.hue = channelColor.hue;
                ledObjectColorStart.saturation = channelColor.saturation;
                ledObjectColorStart.value = (double)bwSelector1.value / 255;
                ledObjectColorStop.hue = channelColor.hue;
                ledObjectColorStop.saturation = channelColor.saturation;
                ledObjectColorStop.value = (double)bwSelector2.value / 255;
                ledobject.colorStart = ledObjectColorStart.toRGB();
                ledobject.colorStop = ledObjectColorStop.toRGB();
            }

            return true;
        }

    }	

}
