using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace xwLedConfigurator {

    public class dim : ledObject  {

        public Color colorMinimum, colorMaximum;

        public dim() {
            colorMinimum = Colors.Black;
            colorMaximum = Colors.White;
        }

        public void setColor(Color colorMinimum, Color colorMaximum) {
            this.colorMinimum = colorMinimum;
            this.colorMaximum = colorMaximum;
        }

        public override void drawContent(DrawingContext dc, double width, double height) {    
            //fill whole object with color gradient from start to stop
            Pen borderTransparent = new Pen { Brush = Brushes.Transparent, Thickness = 0 };
            LinearGradientBrush contentBrush = new LinearGradientBrush();
            contentBrush.StartPoint = new Point(0,0);
            contentBrush.EndPoint = new Point(0,1);
            contentBrush.GradientStops.Add(new GradientStop(colorMaximum, 0.0));
            contentBrush.GradientStops.Add(new GradientStop(colorMinimum, 1.0));

            dc.DrawRectangle(contentBrush, borderTransparent, new Rect(0, 0, width, height));
        }

        public override void updateChannelColor(Color channelColor, bool rgbChannel) {
            //if object is on rgb channel, change nothing
            if (rgbChannel) return;

            //for non rgb objects, replace hue and saturation with channel color, and keep value unchanged
            hsvColor objectStart = new hsvColor(colorMinimum);
            hsvColor objectStop = new hsvColor(colorMaximum);
            hsvColor channel = new hsvColor(channelColor);

            objectStart.hue = channel.hue;
            objectStart.saturation = channel.saturation;
            objectStop.hue = channel.hue;
            objectStop.saturation = channel.saturation;

            colorMinimum = objectStart.toRGB();
            colorMaximum = objectStop.toRGB();
        }

        public override byte[] getBuffer(int colorChannel, bool reversed) {

            byte[] buffer = new byte[12];
            buffer[0] = 0x04;

            if (colorChannel == 0) buffer[1] = (byte)((new hsvColor(colorMinimum).value * 255));
            if (colorChannel == 1) buffer[1] = colorMinimum.R;
            if (colorChannel == 2) buffer[1] = colorMinimum.G;
            if (colorChannel == 3) buffer[1] = colorMinimum.B;

            if (colorChannel == 0) buffer[2] = (byte)((new hsvColor(colorMaximum).value * 255));
            if (colorChannel == 1) buffer[2] = colorMaximum.R;
            if (colorChannel == 2) buffer[2] = colorMaximum.G;
            if (colorChannel == 3) buffer[2] = colorMaximum.B;

            buffer[4] = (byte)(length);
            buffer[5] = (byte)(length / 256);
            return buffer;

        }

    }
}
