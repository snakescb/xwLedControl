using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace xwLedConfigurator {

    public class sob : ledObject  {

        public Color color;

        public sob() {
            color = Colors.Black;
        }

        public void setColor(Color color) {
            this.color = color;
        }

        public override void drawContent(DrawingContext dc, double width, double height) {
            //fill whole object with the same color
            Pen borderTransparent = new Pen { Brush = Brushes.Transparent, Thickness = 0 };         
            Brush contentBrush = new SolidColorBrush(color);
            dc.DrawRectangle(contentBrush, borderTransparent, new Rect(0, 0, width, height));
        }

        public override void updateChannelColor(Color channelColor, bool rgbChannel) {
            //if object is on rgb channel, change nothing
            if (rgbChannel) return;

            //for non rgb objects, replace hue and saturation with channel color, and keep value unchanged
            hsvColor ledobject = new hsvColor(this.color);
            hsvColor channel = new hsvColor(channelColor);
            ledobject.hue = channel.hue;
            ledobject.saturation = channel.saturation;
            this.color = ledobject.toRGB();
        }

        public override byte[] getBuffer(int colorChannel, bool reversed) {
            byte[] buffer = new byte[12];
            buffer[0] = 0x01;

            if (colorChannel == 0) buffer[1] = (byte)((new hsvColor(color)).value * 255);
            if (colorChannel == 1) buffer[1] = color.R;
            if (colorChannel == 2) buffer[1] = color.G;
            if (colorChannel == 3) buffer[1] = color.B;

            buffer[2] = (byte)(length);
            buffer[3] = (byte)(length / 256);
            return buffer;
        }

        public void parseFromData(byte[] data, Color channelColor,  int colorChannel) {
            if (colorChannel == 0) {
                hsvColor hsv = new hsvColor(channelColor);
                hsv.value = data[1] / 255.0;
                color = hsv.toRGB();
            }
            if (colorChannel == 1) color.R = data[1];
            if (colorChannel == 2) color.G = data[1];
            if (colorChannel == 3) color.B = data[1];

            length = data[2];
            length += data[3] * 256;
        }

    }
}
