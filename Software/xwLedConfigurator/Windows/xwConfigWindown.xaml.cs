﻿using System;
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

namespace xwLedConfigurator
{

    public partial class xwConfigWindow : UserControl {



		public xwConfigWindow() {
            InitializeComponent();

			Connection.frameReceived += this.frameReceiver;
		}

		private void updateGui(Object myObject, EventArgs myEventArgs) {


		}

		private void frameReceiver(ref cRxFrame rxFrame) {

		}


    }

	

}