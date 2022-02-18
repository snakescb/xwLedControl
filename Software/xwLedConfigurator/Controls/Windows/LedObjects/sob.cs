using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace xwLedConfigurator {

    public class sob : ledObject  {

        public Color color;

        public sob() {
            objecttype = "sob";
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

        public override byte[] getBuffer(bool reversed) {
            byte[] buffer = new byte[12];
            return buffer;
        }

    }
}
