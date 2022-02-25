using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace xwLedConfigurator {

    public class lob : ledObject  {

        public Color colorStart, colorStop;

        public lob() {
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
            buffer[0] = 0x02;

            Color color1 = colorStart;
            Color color2 = colorStop;
            if (reversed) {
                color1 = colorStop;
                color2 = colorStart;
            }

            if (colorChannel == 0) buffer[1] = (byte)((new hsvColor(color1).value * 255));
            if (colorChannel == 1) buffer[1] = color1.R;
            if (colorChannel == 2) buffer[1] = color1.G;
            if (colorChannel == 3) buffer[1] = color1.B;

            if (colorChannel == 0) buffer[2] = (byte)((new hsvColor(color2).value * 255));
            if (colorChannel == 1) buffer[2] = color2.R;
            if (colorChannel == 2) buffer[2] = color2.G;
            if (colorChannel == 3) buffer[2] = color2.B;

            buffer[4] = (byte)(length);
            buffer[5] = (byte)(length / 256);
            return buffer;

        }

        public override bool applyBuffer(byte[] buffer) {
            if (buffer[0] != 0x02) return false;

            hsvColor hsv = new hsvColor(Colors.White);
            hsv.value = (double)buffer[1] / 255;
            colorStart = hsv.toRGB();
            hsv.value = (double)buffer[2] / 255;
            colorStop = hsv.toRGB();
            length = buffer[4];
            length += buffer[5] * 256;
            return true;

        }

        public override void reconstructColors(ledObject red, ledObject green, ledObject blue) {
            colorStart.R = (byte)((new hsvColor(((lob)red).colorStart)).value * 255);
            colorStart.G = (byte)((new hsvColor(((lob)green).colorStart)).value * 255);
            colorStart.B = (byte)((new hsvColor(((lob)blue).colorStart)).value * 255);

            colorStop.R = (byte)((new hsvColor(((lob)red).colorStop)).value * 255);
            colorStop.G = (byte)((new hsvColor(((lob)green).colorStop)).value * 255);
            colorStop.B = (byte)((new hsvColor(((lob)blue).colorStop)).value * 255);
        }

    }
}
