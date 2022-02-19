using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace xwLedConfigurator {

    public class lob : ledObject  {

        public Color colorStart, colorStop;

        public lob() {
            objecttype = "lob";
            colorStart = Colors.Black;
            colorStop = Colors.White;
        }

        public void setColor(Color colorStart, Color colorStop) {
            this.colorStart = colorStart;
            this.colorStop = colorStop;
        }

        public override void drawContent(DrawingContext dc, double width, double height) {    
            //fill whole object with color gradient from start to stop
            Pen borderTransparent = new Pen { Brush = Brushes.Transparent, Thickness = 0 };
            LinearGradientBrush contentBrush = new LinearGradientBrush();
            contentBrush.StartPoint = new Point(0,0);
            contentBrush.EndPoint = new Point(1,0);
            contentBrush.GradientStops.Add(new GradientStop(colorStart, 0.0));
            contentBrush.GradientStops.Add(new GradientStop(colorStop, 1.0));

            dc.DrawRectangle(contentBrush, borderTransparent, new Rect(0, 0, width, height));
        }

        public override void updateChannelColor(Color channelColor, bool rgbChannel) {
            //if object is on rgb channel, change nothing
            if (rgbChannel) return;

            //for non rgb objects, replace hue and saturation with channel color, and keep value unchanged
            hsvColor objectStart = new hsvColor(colorStart);
            hsvColor objectStop = new hsvColor(colorStop);
            hsvColor channel = new hsvColor(channelColor);

            objectStart.hue = channel.hue;
            objectStart.saturation = channel.saturation;
            objectStop.hue = channel.hue;
            objectStop.saturation = channel.saturation;

            colorStart = objectStart.toRGB();
            colorStop = objectStop.toRGB();
        }

        public override byte[] getBuffer(int colorChannel, bool reversed) {
            byte[] buffer = new byte[12];
            return buffer;
        }

    }
}
