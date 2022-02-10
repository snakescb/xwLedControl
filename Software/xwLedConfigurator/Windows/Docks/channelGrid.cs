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

        private int resolution = 10; //pixel per second
        private double length = 10;  //seconds
        private double major = 1;    //seconds
        private int sub = 4;         //number

        public void refresh(double length_, double major_, int sub_, int resolution_) {
            length = length_;
            major = major_;
            sub = sub_;
            resolution = resolution_;

            this.Width = length*resolution;
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

        }
    }

}
