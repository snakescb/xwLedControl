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
        public abstract byte[] getBuffer(int colorChannel, bool reversed); //channel 0 will access brightness, channel 1 is Red, channel 2 is Green, channel 3 is blue

    }
}
