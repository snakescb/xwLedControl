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

    public partial class xwDockChannel : UserControl {

        public delegate void eChannelEvent(xwDockChannel sender, channelEvent_t request);
        public event eChannelEvent channelEvent;
        public delegate void eObjectEditRquest(xwDockChannel sender, ledObject ledobject);
        public event eObjectEditRquest objectEditRquest;

        public enum channelEvent_t {            
            DELETE_CHANNEL,
            EDIT_CHANNEL,
            GRID_OFFSET_CHANGED,    
        }

        public class cZoom {
            public double resolution;
            public double major;
            public int sub;
            public double snapsize_ms;
            public double minwidth_ms;

            public cZoom(double resolution, double major, int sub, double snapsize, double minwidth) {
                this.resolution = resolution;
                this.major = major;
                this.sub = sub;
                this.snapsize_ms = snapsize;
                this.minwidth_ms = minwidth;
            }
        }

        //Zoom table
        cZoom[] zoomLevels = new cZoom[] {
            new cZoom (   0.2, 300,  5, 15000, 15000),
            new cZoom (   0.5, 120,  4, 15000, 15000),
            new cZoom (   1  , 120,  4, 10000, 10000),
            new cZoom (   2  ,  60,  6,  5000,  5000),
            new cZoom (   4  ,  30,  6,  2500,  2500),
            new cZoom (   8  ,  20,  4,  2500,  2500),
            new cZoom (  16  ,  10,  5,  1000,  1000),
            new cZoom (  32  ,   5,  5,   500,   500),
            new cZoom (  64  ,   2,  4,   250,   250),
            new cZoom ( 128  ,   1,  5,   100,   100),
            new cZoom ( 256  ,   1, 10,    50,    50),
            new cZoom ( 512  ,   1, 10,    25,    25)
        };

        public double gridOffset = 0;
        int currentZoomLevel = 6;
        public channel_t channel = null;

        public xwDockChannel(channel_t channel) {
            InitializeComponent();
            this.channel = channel;

            //set name and color band of channel
            channelName.Content = String.Format("Channel {0}", channel.channelNumber);
            if (channel.isRGB) {
                rgbband.Visibility = Visibility.Visible;
                bwband.Visibility = Visibility.Hidden;
            }                        

            //refresh the grid, then refresh all visuals and objects
            refreshGrid();
            settingsChanged();
        }

        public void settingsChanged() {
            //apply new channel settings to visual
            channelColorBand.Color = channel.color;

            //remove all visuals
            grid.Children.Clear();

            //update the channel color in each led object
            //create a new visual for each object
            foreach (ledObject led in channel.ledObjects) {                
                led.updateChannelColor(channel.color, channel.isRGB);                
                grid.Children.Add(new ledVisual(led, 0, grid.Height - 40));
            }

            //refresh grid again, so new visuals get properly aligned and displayed
            refreshGrid();
        }

        public void refreshGrid() {

            grid.refresh(gridOffset, zoomLevels[currentZoomLevel].major, zoomLevels[currentZoomLevel].sub, zoomLevels[currentZoomLevel].resolution);

            //sort and realign objects, then update them to grid. Do not realign and sort during dragging operation
            if (!dragging) {
                sortObjects();
                alignObjects();
            }

            //realign the children in the grid
            foreach (ledVisual visual in grid.Children) {
                        
                double width = visual.ledObject.length * zoomLevels[currentZoomLevel].resolution / 1000;
                double start = visual.ledObject.starttime * zoomLevels[currentZoomLevel].resolution / 1000;

                visual.width = width;
                visual.refresh();
                Canvas.SetLeft(visual, start - gridOffset);
                Canvas.SetTop(visual, 14);

                if (visual.isHighlighted) Canvas.SetZIndex(visual, 2);
                else Canvas.SetZIndex(visual, 1);
            }
        }

        public void zoom(bool directionIn) {
            //make sure start time remains unchanged after zooming
            double starttime = gridOffset / zoomLevels[currentZoomLevel].resolution;

            if (directionIn) currentZoomLevel++;
            else currentZoomLevel--;
            if (currentZoomLevel >= zoomLevels.Length) currentZoomLevel = zoomLevels.Length - 1;
            if (currentZoomLevel < 0) currentZoomLevel = 0;

            gridOffset = starttime * zoomLevels[currentZoomLevel].resolution;

            refreshGrid();
        }

        public void setOffset(double newOffset) {
            gridOffset = newOffset;
            refreshGrid();
        }

        private void bDelete_Click(object sender, RoutedEventArgs e) {
            if (channelEvent != null) channelEvent(this, channelEvent_t.DELETE_CHANNEL);
        }

        private void bSettings_Click(object sender, RoutedEventArgs e) {
            if (channelEvent != null) channelEvent(this, channelEvent_t.EDIT_CHANNEL);
        }

        private void UserControl_Drop(object sender, DragEventArgs e) {

            Point release = e.GetPosition(grid);
            double releasetime_ms = Math.Round(((release.X + gridOffset) * 1000) / zoomLevels[currentZoomLevel].resolution);           
            string objectType = e.Data.GetData(DataFormats.StringFormat).ToString();

            //create object with 1 major time length at the next snap point next to the mousepointer
            double objectLength = zoomLevels[currentZoomLevel].major*1000;
            double objectStartTime = Math.Round(releasetime_ms / zoomLevels[currentZoomLevel].snapsize_ms) * zoomLevels[currentZoomLevel].snapsize_ms;

            //new SOB created
            if (objectType == "sob") {
                sob led = new sob();
                if (channel.isRGB) led.setColor(Colors.Purple);
                else led.setColor(channel.color);
                led.starttime = objectStartTime;
                led.length = objectLength;
                channel.ledObjects.Add(led);
                grid.Children.Add(new ledVisual(led, 0, grid.Height - 40));
            }
            //new LOB created
            else if (objectType == "lob") {
                lob led = new lob();
                if (channel.isRGB) led.setColor(Colors.Blue, Colors.Red);
                else {
                    hsvColor c = new hsvColor(channel.color);
                    c.value = 0;
                    led.setColor(c.toRGB(), channel.color);
                }
                led.starttime = objectStartTime;
                led.length = objectLength;
                channel.ledObjects.Add(led);
                grid.Children.Add(new ledVisual(led, 0, grid.Height - 40));
            }
            else MessageBox.Show(objectType + " - NOT IMPLEMENTED YET");    
            
            refreshGrid();
        }

        private void sortObjects() {
            channel.ledObjects.Sort((x,y) => x.starttime.CompareTo(y.starttime));
        }

        private void alignObjects() {            
            double lastStopTime = -1;
            foreach (ledObject ledobj in channel.ledObjects) {
                if (ledobj.starttime < 0) ledobj.starttime = 0;
                if ((lastStopTime != -1) && (ledobj.starttime <= lastStopTime)) ledobj.starttime = lastStopTime + 1;   
                lastStopTime = ledobj.starttime + ledobj.length - 1;
            }
        }

        public void keyPress(KeyEventArgs e) {
            if ((e.Key == Key.C) && (Keyboard.IsKeyDown(Key.LeftCtrl))) {
                if (dragObject != null) {
                    MessageBox.Show("Object Copy");
                }
            }

            if (e.Key == Key.Delete) {
                if (dragObject != null) {
                    grid.Children.Remove(dragObject);
                    channel.ledObjects.Remove(dragObject.ledObject);
                    dragObject = null;
                    refreshGrid();
                }
            }
        }

        //Moving and resizing control
        ledVisual dragObject = null;
        bool dragging = false;
        bool probablyClick = false;        
        double dragStartGridOffset;
        double dragObjectStartTime;
        double dragObjectLength;
        double dragStartPostion;
        enum dragAction_t {
            MOVE_GRID,
            MOVE_OBJECT,
            RESIZE_OBJECT_RIGHT,
            RESIZE_OBJECT_LEFT
        }
        dragAction_t dragAction = dragAction_t.MOVE_GRID;

        private void grid_MouseLeftButtonDown(object sender, MouseButtonEventArgs e) {
            dragStartGridOffset = gridOffset;
            if (dragObject != null) {
                dragObjectStartTime = dragObject.ledObject.starttime;
                dragObjectLength = dragObject.ledObject.length;
            }
            dragStartPostion = e.GetPosition(grid).X;
            probablyClick = true;
            dragging = true;
        }

        private void grid_MouseLeftButtonUp(object sender, MouseButtonEventArgs e) {
            //check if it was a click and not a drag operation
            if (probablyClick && (dragObject != null)) if (objectEditRquest != null) objectEditRquest(this, dragObject.ledObject);
            dragging = false;
            refreshGrid();
        }
        private void grid_MouseLeave(object sender, MouseEventArgs e) {
            dragging = false;
            Cursor = Cursors.Arrow;
            refreshGrid();
        }

        private void grid_MouseMove(object sender, MouseEventArgs e) {

            Point current = e.GetPosition(grid);
            if (Math.Abs(current.X - dragStartPostion) > 0) probablyClick = false;

            if (dragging) {
                double dragDistance = current.X - dragStartPostion;

                switch (dragAction) {
                   
                    //move the grid
                    case dragAction_t.MOVE_GRID:
                        double newOffset = dragStartGridOffset - dragDistance;
                        if (newOffset < 0) newOffset = 0;
                        setOffset(newOffset);
                        if (channelEvent != null) channelEvent(this, channelEvent_t.GRID_OFFSET_CHANGED);
                        break;
                    
                    //move an led object
                    case dragAction_t.MOVE_OBJECT:
                        double newStartTime = dragObjectStartTime + ((dragDistance * 1000) / zoomLevels[currentZoomLevel].resolution);
                        newStartTime = Math.Round(newStartTime / zoomLevels[currentZoomLevel].snapsize_ms) * zoomLevels[currentZoomLevel].snapsize_ms;
                        if (newStartTime < 0) newStartTime = 0;
                        dragObject.ledObject.starttime = newStartTime;
                        refreshGrid();
                        break;

                    //resize led object right
                    case dragAction_t.RESIZE_OBJECT_RIGHT:
                        double newLength = dragObjectLength + ((dragDistance * 1000) / zoomLevels[currentZoomLevel].resolution);
                        newLength = Math.Round(newLength / zoomLevels[currentZoomLevel].snapsize_ms) * zoomLevels[currentZoomLevel].snapsize_ms;
                        if (newLength < zoomLevels[currentZoomLevel].minwidth_ms) newLength = zoomLevels[currentZoomLevel].minwidth_ms;
                        dragObject.ledObject.length = newLength;
                        refreshGrid();
                        break;

                    //resize led object left
                    case dragAction_t.RESIZE_OBJECT_LEFT:
                        double newObjectStart = dragObjectStartTime + ((dragDistance * 1000) / zoomLevels[currentZoomLevel].resolution);
                        newObjectStart = Math.Round(newObjectStart / zoomLevels[currentZoomLevel].snapsize_ms) * zoomLevels[currentZoomLevel].snapsize_ms;
                        double originalEndTime = dragObjectStartTime + dragObjectLength - 1;
                        double newObjectLength = originalEndTime - newObjectStart + 1;
                        if (newObjectLength < zoomLevels[currentZoomLevel].minwidth_ms) {
                            newObjectLength = zoomLevels[currentZoomLevel].minwidth_ms;
                            newObjectStart = originalEndTime - newObjectLength + 1;
                         }
                        dragObject.ledObject.starttime = newObjectStart;
                        dragObject.ledObject.length = newObjectLength;
                        refreshGrid();
                        break;
                }
            }
            else {

                //check which action a mouse down will trigger
                //check if mouse is over led object if it was on grid before
                if (dragObject == null) {
                    bool refresh = false;
                    foreach (ledVisual visual in grid.Children) { 
                        if (visual.IsMouseOver) {
                            dragObject = visual;
                            dragObject.isHighlighted = true;
                            refresh = true;
                        }
                    }
                    if (refresh) refreshGrid();
                }
                //check if mouse is not on led object anymore
                else {
                    if (!dragObject.IsMouseOver) {                        
                        dragObject.isHighlighted = false;
                        dragObject = null;
                        refreshGrid();
                    }
                }

                //if the mouse is not on a led object, mouse down will move the grid
                if (dragObject == null) {
                    dragAction = dragAction_t.MOVE_GRID;
                    Cursor = Cursors.ScrollWE;
                }
                else {
                    Point relativePosition = e.GetPosition(dragObject);
                    //small led object can be moved only
                    if (dragObject.width <= 10 ) {
                        dragAction = dragAction_t.MOVE_OBJECT;
                        Cursor = Cursors.SizeAll;
                    }
                    //if mouse in on left edge, drag to left is enabled
                    else if (relativePosition.X < 5) {
                        dragAction = dragAction_t.RESIZE_OBJECT_LEFT;
                        Cursor = Cursors.SizeWE;
                    }
                    //if mouse in on the right edge, drag to right is enabled
                    else if (relativePosition.X >= (dragObject.width - 5)) {
                        dragAction = dragAction_t.RESIZE_OBJECT_RIGHT;
                        Cursor = Cursors.SizeWE;
                    }
                    //if everything else is not true, ledobject can be moved
                    else {
                        dragAction = dragAction_t.MOVE_OBJECT;
                        Cursor = Cursors.SizeAll;
                    }                    
                }

            }

        }
        
    }

}
