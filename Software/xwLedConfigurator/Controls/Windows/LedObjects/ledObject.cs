using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Controls;
using System.Windows.Media;
using System.Globalization;
using Pen = System.Windows.Media.Pen;
using Point = System.Windows.Point;
using Brush = System.Windows.Media.Brush;
using Color = System.Windows.Media.Color;
using Brushes = System.Windows.Media.Brushes;
using System.Drawing;

namespace xwLedConfigurator {


    public abstract class ledObject : UserControl {
        
        public int bufferLength = 12;
        public double starttime;
        public double stoptime;
        byte[] buffer = new byte[12];

        public ledObject() {
            
        }

        public abstract void redraw();
        public abstract void getBuffer(ref byte[] buf, bool reverse);

    }
}
