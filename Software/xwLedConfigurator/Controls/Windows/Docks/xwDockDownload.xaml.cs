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
using System.Windows.Threading;
using System.Threading.Tasks;

namespace xwLedConfigurator {

    public partial class xwDockDownload : UserControl {

        Download download;

        public xwDockDownload() {
            InitializeComponent();
            download = new Download();
            download.downloadEvent += downloadEventHandler;
            Connection.frameReceived += download.frameReceiver;
        }
      
        public void downloadRequestHandler(List<sequence_t> sequenceList) {
            this.Dispatcher.BeginInvoke(new Action(() => {

                this.Visibility = Visibility.Visible;
                showText("Initating download...");
                download.startDownload(sequenceList);

            }));
        }

        void downloadEventHandler(Download.eventType type, double eventData) {
            this.Dispatcher.BeginInvoke(new Action(() => {

                switch (type) {
                    case Download.eventType.ERROR_NO_RESPONSE: {
                        showText("Error: No response from device");
                        hideWindowIn(3000);
                        break;
                    }

                    case Download.eventType.PROGRESS_UPDATE: {
                        showProgress(eventData);
                        break;
                    }

                    case Download.eventType.FINISHED: {
                        hideWindowIn(3000);
                        break;
                    }
                }

            }));            
        }

        void hideWindowIn(int milliseconds) {
            Task.Run(() => {
                Thread.Sleep(milliseconds);
                this.Dispatcher.BeginInvoke(new Action(() => {
                    this.Visibility = Visibility.Collapsed;
                }));
            });
        }

        void showText(string text) {
            message.Visibility = Visibility.Visible;
            progressBar.Visibility = Visibility.Collapsed;
            progressText.Visibility = Visibility.Collapsed;

            message.Text = text;
        }

        void showProgress(double progress) {
            message.Visibility = Visibility.Collapsed;
            progressBar.Visibility = Visibility.Visible;
            progressText.Visibility = Visibility.Visible;

            int percent = (int)(progress * 100);
            progressBar.Value = percent;
            progressText.Text = String.Format("Downloading {0}%", percent);
        }

    }	

}
