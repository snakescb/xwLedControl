using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Globalization;
using System.Reflection;
using System.Threading;
using System.Timers;
using System.Linq;

namespace xwLedConfigurator {

    public class sobCanvas : Canvas {

        public int dimvalue = 0; //dim value in %
        public Brush color = Brushes.Gray;

        protected override void OnRender(DrawingContext dc) {
            //if (render != null) render(ref dc);

            Size size = new Size((int)base.ActualWidth, (int)base.ActualHeight);

            Brush background = (Brush)this.FindResource("AppMainColor");
            Pen penBorder = new Pen { Brush = Brushes.Transparent, Thickness = 0.5, StartLineCap = PenLineCap.Round, EndLineCap = PenLineCap.Round };

            //draw background with border
            dc.DrawRectangle(background, penBorder, new System.Windows.Rect(0, 0, size.Width, size.Height));

            //draw constant light value
            double heigth = size.Height * dimvalue / 100;
            dc.DrawRectangle(color, penBorder, new System.Windows.Rect(0, size.Height - heigth, size.Width, heigth));
        }

    }

    public partial class SOB : ledObject {

        public byte R = 127;
        public byte G = 127;
        public byte B = 127;

        public SOB() {
            InitializeComponent();
        }

        public override void redraw() {
            graphics.color = new SolidColorBrush(Color.FromArgb(255, R, G, B));
            graphics.dimvalue = 50;
        }

        public override void getBuffer(ref byte[] buf, bool reverse) {

        }

    }	
}
