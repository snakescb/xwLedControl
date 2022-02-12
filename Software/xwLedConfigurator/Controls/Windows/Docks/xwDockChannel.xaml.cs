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

        double majorGrid = 10;
        int subDivision = 5;
        int resolution = 17;
        int zoomLevel = 6;

        int[] zoomResolution = new int[] { 2, 4, 6, 8, 11, 14, 17, 22, 30, 38, 46, 62, 88, 110 };

        channel_t channel = null;

        public xwDockChannel(channel_t channel_) {
            InitializeComponent();
            channel = channel_;
            channelName.Content = String.Format("Channel {0}", channel.index + 1);
            if (channel.isRGB) {
                rgbband.Visibility = Visibility.Visible;
                bwband.Visibility = Visibility.Hidden;
            }
            refreshGrid();
        }

        public void refreshGrid() {
            grid.Children.Clear();
            //grid.refresh(0, majorGrid, subDivision, resolution);
            grid.refresh(10000, 5, 5, 10);

            //put led objects to canvas
            foreach (ledObject ledobj in channel.ledObjects) {

                var parent = VisualTreeHelper.GetParent(ledobj);

                double length = ledobj.stoptime - ledobj.starttime + 1;
                double width = length * resolution / 1000;
                double start = ledobj.starttime * resolution / 1000;

                ledobj.Width = width;
                //ledobj.Height = grid.ActualHeight - 28;
                ledobj.Height = 120 - 28;
                Canvas.SetLeft(ledobj, start);
                Canvas.SetTop(ledobj, 6);

                if (parent == null) grid.Children.Add(ledobj);
                ledobj.redraw();
            }
        }

        public void cleanup() {
            grid.Children.Clear();
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
            //MessageBox.Show(String.Format("Channel {0}: X-Position: {1} - Time: {2}", channel.index, release.X, releasetime_ms));
            int length = 5000;

            //create object with default time centered around release point. Will be place
            SOB sob = new SOB();
            sob.starttime = releasetime_ms-100;
            sob.stoptime = sob.starttime + length - 1;

            //add object to list, sort and realign 
            channel.ledObjects.Add(sob);
            sortObjects();
            alignObjects();
            refreshGrid();
        }

        private void sortObjects() {
            channel.ledObjects.Sort((x,y) => x.starttime.CompareTo(y.starttime));
        }

        private void alignObjects() {
            double lastStopTime = -1;

            foreach (ledObject ledobj in channel.ledObjects) {
                double length = ledobj.stoptime - ledobj.starttime + 1;

                if (ledobj.starttime < 0) {
                    ledobj.starttime = 0;
                    ledobj.stoptime = length - 1;
                }

                if (lastStopTime != -1) {
                    if (ledobj.starttime <= lastStopTime) {
                        ledobj.starttime = lastStopTime + 1;
                        ledobj.stoptime = ledobj.starttime + length - 1;
                    }
                }

                lastStopTime = ledobj.stoptime;
            }
        }
    }

}
