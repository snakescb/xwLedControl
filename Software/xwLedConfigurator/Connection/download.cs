using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.Threading;
using System.Runtime.InteropServices;
using System.Timers;

namespace xwLedConfigurator {

    public class Download {

        class outputController {

            public int output;            
            int colorChannel;
            channel_t channel;
            public byte[] data;

            public outputController(channel_t channel, int output, int colorChannel) {
                this.channel = channel;
                this.output = output;
                this.colorChannel = colorChannel;                         
            }
        }

        List<sequence_t> sequenceList;

        public Download() {

        }

    }
}
