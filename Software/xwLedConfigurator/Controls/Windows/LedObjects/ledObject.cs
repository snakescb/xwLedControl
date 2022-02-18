using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Controls;
using System.Windows.Media;

namespace xwLedConfigurator {


    public abstract class ledObject {

        public string objecttype;
        public double starttime;
        public double length;
     
        public ledObject() {
        }

        public abstract void drawContent(DrawingContext dc, double width, double height);
        public abstract void updateChannelColor(Color channelColor, bool rgbChannel);
        public abstract byte[] getBuffer(bool reversed);

    }
}
