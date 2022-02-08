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

    public partial class xwDockSequenceName : UserControl {

        public delegate void eNameUpdated(string s);
        public event eNameUpdated nameUpdated;

        public xwDockSequenceName() {
            InitializeComponent();
		}

        public void show() {
            sequenceName.Text = "";
            this.Visibility = Visibility.Visible;
        }

        public void hide() {
            this.Visibility = Visibility.Collapsed;
        }

        private void bAbort_Click(object sender, RoutedEventArgs e) {
            hide();
        }

        private void bCreateSequence_Click(object sender, RoutedEventArgs e) {
            if (sequenceName.Text.Length > 0) {
                if (nameUpdated != null) nameUpdated(sequenceName.Text);
            }
        }
    }	

}
