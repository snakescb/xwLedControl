using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using System.Windows.Controls.Primitives;
using System.Windows.Media;
using System.Windows.Controls;
using System.Globalization;
using Pen = System.Windows.Media.Pen;
using Point = System.Windows.Point;
using Brush = System.Windows.Media.Brush;
using Color = System.Windows.Media.Color;
using Brushes = System.Windows.Media.Brushes;

namespace xwLedConfigurator {

    public class channelGrid : Canvas {

        private int resolution = 10; //pixels per second
        private double offset = 0;   //offset in pixel
        private double major = 1;    //seconds per major tick
        private int sub = 4;         //number        

        public void refresh(double setoffset, double setmajor, int setsub, int setresolution) {

            major = setmajor;
            sub = setsub;
            resolution = setresolution;
            offset = setoffset;

            this.InvalidateVisual();
        }

        protected override void OnRender(DrawingContext dc) {
            
            Size size = new Size((int)base.ActualWidth, (int)base.ActualHeight);
       
            Brush background = new SolidColorBrush( Color.FromArgb(80, 0, 0, 0) );
            Pen penBorder= new Pen { Brush = Brushes.White, Thickness = 1.5,StartLineCap = PenLineCap.Round, EndLineCap = PenLineCap.Round };
            Pen penMajor = new Pen { Brush = Brushes.White, Thickness = 0.6, StartLineCap = PenLineCap.Round, EndLineCap = PenLineCap.Round };
            Pen penMinor = new Pen { Brush = Brushes.White, Thickness = 0.1, StartLineCap = PenLineCap.Round, EndLineCap = PenLineCap.Round };

            //draw background with border
            dc.DrawRectangle(background, penBorder, new System.Windows.Rect(0,0, size.Width, size.Height));

            //get the offset in seconds
            double offset_s = offset / resolution;
            double major_px = major * resolution;
            double firstMajor_px = (major - offset_s % major) * resolution;
            double firstMajor_s = Math.Ceiling(offset_s / major) * major;
            double minor_px = major_px / sub;

            int tickCount = 0;
            //draw minors before first major
            while (true) {
                tickCount++;
                double nextMinor_px = firstMajor_px - tickCount * minor_px;
                if (nextMinor_px <=0 ) break;
                else dc.DrawLine(penMinor, new Point(nextMinor_px, 0), new Point(nextMinor_px, size.Height));
            }

            //draw majors, minors and text until the end of the canvas is reached
            tickCount = 0;
            while (true) {    
                double nextMajor_px = firstMajor_px + tickCount * major_px;
                if (nextMajor_px >= size.Width) break;
                else {
                    if (nextMajor_px > 0) dc.DrawLine(penMajor, new Point(nextMajor_px, 0), new Point(nextMajor_px, size.Height));
                    for (int i = 0; i < sub; i++) {
                        double nextMinor_px = nextMajor_px + i * minor_px;
                        if (nextMinor_px <= size.Width) dc.DrawLine(penMinor, new Point(nextMinor_px, 0), new Point(nextMinor_px, size.Height));
                    }

                    int seconds = (int)(firstMajor_s + tickCount * major);
                    int minutes = (int)Math.Floor((double) seconds / 60);
                    seconds -= minutes * 60;

                    FormattedText text = new FormattedText(String.Format("{0}:{1:D2}", minutes, seconds), CultureInfo.GetCultureInfo("en-us"), System.Windows.FlowDirection.LeftToRight, new Typeface("Verdana"), 11, Brushes.White, VisualTreeHelper.GetDpi(this).PixelsPerDip);
                    if (text.Width < major_px - 10) {
                        if (nextMajor_px - text.Width - 5 > 0) {
                            dc.DrawText(text, new Point(nextMajor_px - text.Width - 3, size.Height - text.Height - 3));
                        }
                    }
                }
                tickCount++;
            }

            /*

            //draw grid
            int numMajors = (int) Math.Ceiling(length / major) - 1;
            double pixelsPerMajor = major * resolution;
            double pixelsPerMinor = pixelsPerMajor / sub;
            for (int i = 0; i <= numMajors; i++) {
                double majorStart = pixelsPerMajor * i;
                if (i > 0) dc.DrawLine(penMajor, new Point(majorStart, 0), new Point(majorStart, size.Height));
                for (int j = 1; j < sub; j++) {
                    double minorStart = majorStart + j * pixelsPerMinor;
                    if (minorStart < size.Width) dc.DrawLine(penMinor, new Point(minorStart, 0), new Point(minorStart, size.Height));
                }
            }

            //draw text
            for (int i = 1; i <= numMajors+1; i++) {
                double majorStart = pixelsPerMajor * i;

                int seconds = (int) ( i * major );
                int minutes = (int) Math.Floor((double)seconds / 60);
                seconds -= minutes * 60;

                FormattedText text = new FormattedText(String.Format("{0}:{1:D2}", minutes, seconds), CultureInfo.GetCultureInfo("en-us"), System.Windows.FlowDirection.LeftToRight, new Typeface("Verdana"), 9, Brushes.White, VisualTreeHelper.GetDpi(this).PixelsPerDip);
                if (text.Width < pixelsPerMajor - 10) {
                    if(majorStart < size.Width) dc.DrawText(text, new Point(majorStart - text.Width - 3, size.Height - text.Height - 3));
                }
            }
            */

        }
    }

}
