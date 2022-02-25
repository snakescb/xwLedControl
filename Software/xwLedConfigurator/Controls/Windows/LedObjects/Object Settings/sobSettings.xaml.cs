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

    public partial class sobSettings : UserControl {

        public sobSettings() {
            InitializeComponent();
		}

        sob ledobject;
        channel_t channel;

        public void show(sob ledobject, channel_t channel) {

            this.ledobject = ledobject;
            this.channel = channel; 

            if (channel.isRGB) {
                rgbTitle.Visibility = Visibility.Visible;
                rgbSelector.Visibility = Visibility.Visible;
                bwTitle.Visibility = Visibility.Collapsed;
                bwSelector.Visibility = Visibility.Collapsed;

                colorPicker.SelectedColor = ledobject.color;                
            }
            else {
                bwTitle.Visibility = Visibility.Visible;
                bwSelector.Visibility = Visibility.Visible;
                rgbTitle.Visibility = Visibility.Collapsed;
                rgbSelector.Visibility = Visibility.Collapsed;
                
                hsvColor hsvColor = new hsvColor(ledobject.color);
                bwSelector.baseColor = channel.color;
                bwSelector.value = (byte) (hsvColor.value * 255);
            }            

            this.Visibility = Visibility.Visible;
        }

        public bool save(sob ledobject, channel_t channel) {

            if (channel.isRGB) ledobject.color = colorPicker.SelectedColor;
            else {
                //adjust brightness of channel color
                hsvColor ledObjectColor = new hsvColor(ledobject.color);
                hsvColor channelColor = new hsvColor(channel.color);
                
                ledObjectColor.hue = channelColor.hue;
                ledObjectColor.saturation = channelColor.saturation;
                ledObjectColor.value = (double) bwSelector.value / 255;
                ledobject.color = ledObjectColor.toRGB();
            }

            return true;
        }

    }	

}
