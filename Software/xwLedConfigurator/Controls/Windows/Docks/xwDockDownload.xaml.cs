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
using System.Windows.Threading;

namespace xwLedConfigurator {

    public partial class xwDockDownload : UserControl {

        Download download;
        string messageText = "";
        int hideTimeout = -1;
        double progressValue;

        public xwDockDownload() {
            InitializeComponent();            
        }
      
        public void downloadRequestHandler(List<sequence_t> sequenceList) {
            message.Visibility = Visibility.Visible;
            progress.Visibility = Visibility.Collapsed;
            progressText.Visibility = Visibility.Collapsed;
            this.Visibility = Visibility.Visible;

            System.Windows.Forms.Timer guiUpdate = new System.Windows.Forms.Timer();
            guiUpdate.Tick += updateGui;
            guiUpdate.Interval = 50;
            guiUpdate.Enabled = true;

            download = new Download();
            download.downloadEvent += downloadEventHandler;
            Connection.frameReceived += download.frameReceiver;
            download.startDownload(sequenceList);

            progressValue = 0;
            messageText = "Initating download...";
            updateGui(null, null);
        }

        void downloadEventHandler(Download.eventType type, double eventData) {
            if (type == Download.eventType.ERROR_SEQUENCE_EMPTY) {
                messageText = "Error: Cannot download empty sequence";
                hideTimeout = 50;                ;
            }
            if (type == Download.eventType.ERROR_NO_RESPONSE) {
                messageText = "Error: No response from device";
                progressValue = 0;
                hideTimeout = 50;
            }
            if (type == Download.eventType.SUCCESS) {
                messageText = "Download completed";
                progressValue = 0;
                hideTimeout = 50;
            }            
            if (type != Download.eventType.PROGRESS_UPDATE) {
                progressValue = eventData;
            }
        }

        void updateGui(Object myObject, EventArgs myEventArgs) {         

            if (hideTimeout > 0) {
                hideTimeout--;
                if (hideTimeout == 0) this.Visibility = Visibility.Collapsed;
            }

            if (progressValue > 0) {
                message.Visibility = Visibility.Collapsed;
                progress.Visibility = Visibility.Visible;
                progressText.Visibility = Visibility.Visible;
                int percent = (int)(progressValue * 100);
                progress.Value = percent;
                progressText.Text = String.Format("Downloading {0}%", percent);
            }
            else {
                message.Visibility = Visibility.Visible;
                progress.Visibility = Visibility.Collapsed;
                progressText.Visibility = Visibility.Collapsed;
                message.Text = messageText;
            }
        }


    }	

}
