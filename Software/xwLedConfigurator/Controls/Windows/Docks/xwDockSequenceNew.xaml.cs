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

    public partial class xwDockSequenceNew : UserControl {

        public delegate void eSequenceCreated(sequence_t sequence);
        public event eSequenceCreated sequenceCreated;

        public xwDockSequenceNew() {
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
                sequence_t sequence = new sequence_t();
                sequence.name = sequenceName.Text;
                if (sequenceCreated != null) sequenceCreated(sequence);
            }
        }
    }	

}
