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

        public delegate void eSequenceRequest(xwDockSequenceList sender, sequenceRequest_t request);
        public event eSequenceRequest sequenceRequest;

        public enum sequenceRequest_t {
            LOAD_SEQUENCE,
            DELETE_SEQUENCE
        }

        public string sequenceName;
        public int index;

        public xwDockSequenceList(int i,string s) {
            InitializeComponent();
            index = i; sequenceName = s; 
            sequenceDisplayName.Text = sequenceName;
        }

        private void bEdit_Click(object sender, RoutedEventArgs e) {
            if (sequenceRequest != null) sequenceRequest(this, sequenceRequest_t.LOAD_SEQUENCE);
        }

        private void bDelete_Click(object sender, RoutedEventArgs e) {
            if (sequenceRequest != null) sequenceRequest(this, sequenceRequest_t.DELETE_SEQUENCE);
        }
    }	

}
