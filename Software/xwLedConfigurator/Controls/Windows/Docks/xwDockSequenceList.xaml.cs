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

    public partial class xwDockSequenceList : UserControl {

        public delegate void eSequenceRequest(sequenceRequest_t requestType, sequence_t sequence);
        public event eSequenceRequest sequenceRequest;

        public enum sequenceRequest_t {
            LOAD_SEQUENCE,
            DELETE_SEQUENCE
        }

        sequence_t sequence;

        public xwDockSequenceList(sequence_t sequence) {
            InitializeComponent();
            this.sequence = sequence;
            sequenceDisplayName.Text = sequence.name;
        }

        private void bEdit_Click(object sender, RoutedEventArgs e) {
            if (sequenceRequest != null) sequenceRequest(sequenceRequest_t.LOAD_SEQUENCE, sequence);
        }

        private void bDelete_Click(object sender, RoutedEventArgs e) {
            if (sequenceRequest != null) sequenceRequest(sequenceRequest_t.DELETE_SEQUENCE, sequence);
        }
    }	

}
