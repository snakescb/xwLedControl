using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using System.Windows.Controls.Primitives;
using System.Windows.Media;
using System.Windows.Controls;
using System.Globalization;
using Brush = System.Windows.Media.Brush;
using Pen = System.Windows.Media.Pen;
using System.Windows;
using Brushes = System.Windows.Media.Brushes;

namespace xwLedConfigurator {

    public class ledVisual : Canvas {

        public ledObject ledObject;
        public double width;
        public double height;
        public bool isHighlighted;
        public bool rgbChannel;

        public ledVisual(ledObject ledObject, double width, double height) {
            this.ledObject = ledObject;
            this.width = width;
            this.height = height;
            this.isHighlighted = false;

            base.Width = this.width;
            base.Height = this.height;            
        }

        public void refresh() {
            this.InvalidateVisual();
        }

        protected override void OnRender(DrawingContext dc) {
            if (ledObject != null) {                

                //draw background
                Brush background = (Brush)App.Current.Resources["AppMainColor"];
                Pen borderTransparent = new Pen { Brush = Brushes.Transparent, Thickness = 0 };               

                //draw background 
                dc.DrawRectangle(background, borderTransparent, new Rect(0, 0, width, height));

                //ask object to draw content
                ledObject.drawContent(dc, width, height);

                //draw border
                Pen borderAround;
                borderAround = new Pen { Brush = Brushes.White, Thickness = .5, StartLineCap = PenLineCap.Round, EndLineCap = PenLineCap.Round };
                if (isHighlighted) {
                    borderAround.Thickness = 3;
                }
                double penWidth = borderAround.Thickness;
                if (width > penWidth) dc.DrawRectangle(Brushes.Transparent, borderAround, new Rect(penWidth / 2, penWidth / 2, width - penWidth, height - penWidth));
                else dc.DrawRectangle(Brushes.Transparent, borderAround, new Rect(penWidth, penWidth, width, height));

            }
        }

    }

}
