using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Controls;
using System.Windows.Media;

namespace xwLedConfigurator {

    public class hsvColor {

        public double hue = 0;
        public double saturation = 0;
        public double value = 0;

        public hsvColor() { }
        public hsvColor(Color c) { 
            fromRGB(c);
        }

        public void fromRGB(Color color) {

            double h, s, v;

            // Convert RGB to a 0.0 to 1.0 range.
            double r = color.R / 255.0;
            double g = color.G / 255.0;
            double b = color.B / 255.0;

            // Get the maximum and minimum RGB components.
            double max = r;
            if (max < g) max = g;
            if (max < b) max = b;

            double min = r;
            if (min > g) min = g;
            if (min > b) min = b;

            double diff = max - min;
            v = max;
            h = 0;

            //max = min
            if (Math.Abs(diff) < 0.00001) {
                s = 0;
                h = 0;  // H is actually undefined.               
            }
            else {
                if (max == r) h = 60 * (0 + ((g - b) / diff));
                if (max == g) h = 60 * (2 + ((b - r) / diff));
                if (max == b) h = 60 * (4 + ((r - g) / diff));

                if (max < 0.00001) s = 0;
                else s = diff / max;

                if (h < 0) h += 360;
                if (h > 360) h -= 360;
            }

            this.hue = h;
            this.saturation = s;
            this.value = v;
        }

        public Color toRGB() {

            double hi = Math.Floor(hue / 60);
            double f = (hue / 60) - hi;
            double p = value * (1 - saturation);
            double q = (value * (1 - saturation * f));
            double t = (value * (1 - saturation * (1 - f)));

            if (hi == 1) return Color.FromArgb(255, (byte)(q * 255), (byte)(value * 255), (byte)(p * 255));
            if (hi == 2) return Color.FromArgb(255, (byte)(p * 255), (byte)(value * 255), (byte)(t * 255));
            if (hi == 3) return Color.FromArgb(255, (byte)(p * 255), (byte)(q * 255), (byte)(value * 255));
            if (hi == 4) return Color.FromArgb(255, (byte)(t * 255), (byte)(p * 255), (byte)(value * 255));
            if (hi == 5) return Color.FromArgb(255, (byte)(value * 255), (byte)(p * 255), (byte)(q * 255));

            return Color.FromArgb(255, (byte)(value * 255), (byte)(t * 255), (byte)(p * 255));

        }
    }
}
