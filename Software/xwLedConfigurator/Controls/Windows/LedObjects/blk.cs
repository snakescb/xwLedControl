using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace xwLedConfigurator {

    public class blk : ledObject  {

        public Color colorOn, colorOff;
        public double period;
        public double dutycycle;

        public blk() {
            colorOn = Colors.Black;
            colorOff = Colors.White;
        }

        public void setColor(Color colorOn, Color colorOff) {
            this.colorOn = colorOn;
            this.colorOff = colorOff;
        }

        public override void drawContent(DrawingContext dc, double width, double height) {

            int fullPeriods = (int)(length / period);
            double pxPeriod = width * period / length;
            double pxOn =  (pxPeriod * dutycycle) / 100;
            double pxOff = (pxPeriod * (100 - dutycycle)) / 100;

            //fill whole object with the same color
            Pen borderTransparent = new Pen { Brush = Brushes.Transparent, Thickness = 0 };
            Brush contentBrushOn = new SolidColorBrush(colorOn);
            Brush contentBrushOff = new SolidColorBrush(colorOff);
            //dc.DrawRectangle(contentBrush, borderTransparent, new Rect(0, 0, width, height));

            double currentX = 0;
            for (int i = 0; i < fullPeriods; i++) {
                dc.DrawRectangle(contentBrushOn, borderTransparent, new Rect(currentX, 0, pxOn, height));
                currentX += pxOn;
                dc.DrawRectangle(contentBrushOff, borderTransparent, new Rect(currentX, 0, pxOff, height));
                currentX += pxOff;
            }

            if (currentX < width) {
                if (currentX + pxOn < width) {
                    dc.DrawRectangle(contentBrushOn, borderTransparent, new Rect(currentX, 0, pxOn, height));
                    currentX += pxOn;
                }
                else {
                    dc.DrawRectangle(contentBrushOn, borderTransparent, new Rect(currentX, 0, width-currentX, height));
                    currentX = width;
                } 
            }

            if (currentX < width) {
                dc.DrawRectangle(contentBrushOff, borderTransparent, new Rect(currentX, 0, width - currentX, height));
                currentX = width;
            }

        }

        public override void updateChannelColor(Color channelColor, bool rgbChannel) {
            //if object is on rgb channel, change nothing
            if (rgbChannel) return;

            //for non rgb objects, replace hue and saturation with channel color, and keep value unchanged
            hsvColor objectOn = new hsvColor(colorOn);
            hsvColor objectOff = new hsvColor(colorOff);
            hsvColor channel = new hsvColor(channelColor);

            objectOn.hue = channel.hue;
            objectOn.saturation = channel.saturation;
            objectOff.hue = channel.hue;
            objectOff.saturation = channel.saturation;

            colorOn = objectOn.toRGB();
            colorOff = objectOff.toRGB();
        }

        public override byte[] getBuffer(int colorChannel, bool reversed) {

            byte[] buffer = new byte[12];
            buffer[0] = 0x03;

            Color color1 = colorOn;
            Color color2 = colorOff;
            byte duty = (byte)dutycycle;
            if (reversed) {
                color1 = colorOff;
                color2 = colorOn;
                duty = (byte)(100 - duty);
            }

            if (colorChannel == 0) buffer[1] = (byte)((new hsvColor(color1).value * 255));
            if (colorChannel == 1) buffer[1] = color1.R;
            if (colorChannel == 2) buffer[1] = color1.G;
            if (colorChannel == 3) buffer[1] = color1.B;

            if (colorChannel == 0) buffer[2] = (byte)((new hsvColor(color2).value * 255));
            if (colorChannel == 1) buffer[2] = color2.R;
            if (colorChannel == 2) buffer[2] = color2.G;
            if (colorChannel == 3) buffer[2] = color2.B;

            buffer[3] = duty;

            buffer[4] = (byte)(length);
            buffer[5] = (byte)(length / 256);

            buffer[6] = (byte)(period);
            buffer[7] = (byte)(period / 256);

            return buffer;

        }

        public override bool applyBuffer(byte[] buffer) {
            if (buffer[0] != 0x03) return false;

            hsvColor hsv = new hsvColor(Colors.White);
            hsv.value = (double)buffer[1] / 255;
            colorOn = hsv.toRGB();
            hsv.value = (double)buffer[2] / 255;
            colorOff = hsv.toRGB();

            dutycycle = buffer[3];

            length = buffer[4];
            length += buffer[5] * 256;

            period = buffer[6];
            period += buffer[7] * 256;
            return true;

        }

        public override void reconstructColors(ledObject red, ledObject green, ledObject blue) {
            colorOn.R = (byte)((new hsvColor(((blk)red).colorOn)).value * 255);
            colorOn.G = (byte)((new hsvColor(((blk)green).colorOn)).value * 255);
            colorOn.B = (byte)((new hsvColor(((blk)blue).colorOn)).value * 255);

            colorOff.R = (byte)((new hsvColor(((blk)red).colorOff)).value * 255);
            colorOff.G = (byte)((new hsvColor(((blk)green).colorOff)).value * 255);
            colorOff.B = (byte)((new hsvColor(((blk)blue).colorOff)).value * 255);
        }

    }
}
