using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using System.Windows.Controls.Primitives;
using System.Windows.Media;
using System.Windows.Controls;
using Pen = System.Windows.Media.Pen;
using Point = System.Windows.Point;
using Brush = System.Windows.Media.Brush;
using Color = System.Drawing.Color;
using Brushes = System.Windows.Media.Brushes;

namespace xwLedConfigurator {
    public class ruler : Canvas {

        public int topOffset = 30;
        public int mainTickHeight = 25;
        public int mainTickOffset = 20;
        public int subTickHeight =  -5;

        protected override void OnRender(DrawingContext dc) {
            
            Size size = new Size((int)base.ActualWidth, (int)base.ActualHeight);

            Pen foreground = new Pen {
                Brush = Brushes.White,
                Thickness = 1.4,
                StartLineCap = PenLineCap.Round,
                EndLineCap = PenLineCap.Round
            };

            //draw line
            dc.DrawLine(foreground, new Point(0, topOffset), new Point(size.Width, topOffset));

            int numMainTicks = 10;
            int subTicksDivision = 5;

            //draw ticks
            double mainTickDistance = size.Width / numMainTicks;
            double subTickDistance = mainTickDistance / subTicksDivision;
            for (int i = 0; i <= numMainTicks; i++) {
                double mainTickStart = i * mainTickDistance;
                dc.DrawLine(foreground, new Point(mainTickStart, topOffset - mainTickOffset), new Point(mainTickStart, topOffset + mainTickHeight - mainTickOffset));

                if (i < numMainTicks) {
                    for (int z = 1; z < subTicksDivision; z++) {
                        double subTickStart = mainTickStart + z * subTickDistance;
                        dc.DrawLine(foreground, new Point(subTickStart, topOffset), new Point(subTickStart, topOffset + subTickHeight));
                    }
                }
            }



            //sub ticks
            //for (int i = 0; i < 100; i++) {
            //    dc.DrawLine(foreground, new Point(i*10, topOffset), new Point(i * 10, topOffset+subTickHeight));
            //}

            //dc.DrawLine()

            /*
            Size size = new Size(base.ActualWidth, base.ActualHeight);

            int tickCount = (int)((this.Maximum - this.Minimum) / this.TickFrequency) + 1;

            if ((this.Maximum - this.Minimum) % this.TickFrequency == 0)

                tickCount -= 1;

            Double tickFrequencySize;

            // Calculate tick's setting

            tickFrequencySize = (size.Width * this.TickFrequency / (this.Maximum - this.Minimum));

            string text = "";

            FormattedText formattedText = null;

            double num = this.Maximum - this.Minimum;

            int i = 0;

            // Draw each tick text

            for (i = 0; i <= tickCount; i++) {
                text = Convert.ToString(Convert.ToInt32(this.Minimum + this.TickFrequency * i), 10);

                //g.DrawString(text, font, brush, drawRect.Left + tickFrequencySize * i, drawRect.Top + drawRect.Height/2, stringFormat);

                //formattedText = new FormattedText(text, CultureInfo.GetCultureInfo("en-us"), FlowDirection.LeftToRight, new Typeface("Verdana"), 8, Brushes.Black);

                //dc.DrawText(formattedText, new Point((tickFrequencySize * i), 30));
            }
            */

        }
    }

}
