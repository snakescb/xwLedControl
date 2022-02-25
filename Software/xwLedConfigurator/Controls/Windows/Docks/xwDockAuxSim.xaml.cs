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

    public partial class xwDockAuxSim : UserControl {

        bool sendAux = false;
        byte lastAux = 0;

        public xwDockAuxSim() {
            InitializeComponent();
            Thread auxSenderThread = new Thread(auxSender);
            auxSenderThread.Start();
        }

        public void simAuxRequestHandler(bool show) {
            if (show) {
                auxslider.baseColor = Colors.Gray;
                auxslider.baseColorStart = Colors.Gray;
                auxslider.value = 127;
                sendAux = true;
                this.Visibility = Visibility.Visible;
            }
            else {
                sendAux = false;
                this.Visibility = Visibility.Collapsed;
            }
        }

        void auxSender() {
            while (true) {
                if (sendAux) {
                    if (auxslider.value != lastAux) Connection.putFrame((byte)xwCom.SCOPE.LED, new byte[] { (byte)xwCom.LED.AUX_SIM, (byte)(auxslider.value / 2.55) });
                    lastAux = auxslider.value;
                }                
                Thread.Sleep(50);
            }
        }


    }	

}
