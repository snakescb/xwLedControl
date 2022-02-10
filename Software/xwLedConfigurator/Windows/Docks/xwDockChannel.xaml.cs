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

    public partial class xwDockChannel : UserControl {

        public delegate void eChannelEvent(xwDockChannel sender, channelEvent_t request, int index);
        public event eChannelEvent channelEvent;

        public enum channelEvent_t {            
            DELETE_CHANNEL,
            EDIT_CHANNEL
        }

        public double channelLength_s { get { return channelTime; } set { channelTime = value; refreshGrid(); } }   // total time dispayed in grid (seconds)
        public double majorGrid_s { get { return majorGrid; } set { majorGrid = value; refreshGrid(); } }           // time per major grid tick (seconds)
        public int gridDivision { get { return subDivision; } set { subDivision = value; refreshGrid(); } }         // subdivisions per major grid (number)
        public int resolution_pps { get { return resolution; } set { resolution = value; refreshGrid(); } }         // resolution in pixels per second


        double channelTime = 60;
        double majorGrid = 10;
        int subDivision = 5;
        int resolution = 17;
        int zoomLevel = 6;


        int[] zoomResolution = new int[] { 2, 4, 6, 8, 11, 14, 17, 22, 30, 38, 46, 62, 88, 110 };

        channel_t channel = null;

        public xwDockChannel(channel_t channel_) {
            InitializeComponent();
            channel = channel_;
            channelTime = channel.length_ms / 1000;
            channelName.Content = "Channel " + (channel.index + 1).ToString();
            refreshGrid();
        }

        public void refreshGrid() {
            grid.refresh(channelTime, majorGrid, subDivision, resolution);
        }

        public void zoom(bool directionIn) {
            if (directionIn) zoomLevel++;
            else zoomLevel--;

            if (zoomLevel >= zoomResolution.Length) zoomLevel = zoomResolution.Length - 1;
            if (zoomLevel < 0) zoomLevel = 0;

            resolution = zoomResolution[zoomLevel];
            refreshGrid();
        }

        private void bDelete_Click(object sender, RoutedEventArgs e) {
            if (channelEvent != null) channelEvent(this, channelEvent_t.DELETE_CHANNEL, channel.index);
        }

        private void bSettings_Click(object sender, RoutedEventArgs e) {
            if (channelEvent != null) channelEvent(this, channelEvent_t.EDIT_CHANNEL, channel.index);
        }

        private void UserControl_Drop(object sender, DragEventArgs e) {
            Point release = e.GetPosition(grid);
            double releasetime_ms = Math.Round((release.X * 1000) / resolution);
            MessageBox.Show(String.Format("Channel {0}: X-Position: {1} - Time: {2}", channel.index, release.X, releasetime_ms));
        }
    }

}
