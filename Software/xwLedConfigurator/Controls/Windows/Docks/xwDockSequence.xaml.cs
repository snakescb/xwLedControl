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

    public partial class xwDockSequence : UserControl {

        public delegate void eSequenceManagement(sequenceManagement_t action, int sequenceIndex, string sequenceName);
        public event eSequenceManagement sequenceManagement;

        public enum sequenceManagement_t {
            NEW_SEQUENCE,
            DELETE_SEQUENCE,
            LOAD_SEQUENCE,
            SAVE_TO_FILE,
            LOAD_FROM_FILE
        }

        public xwDockSequence() {
            InitializeComponent();
		}

        public void show(List<sequence_t> sequenceList) {
            this.Visibility = Visibility.Visible;
            sequenceItemList.Children.Clear();

            //add elements for each sequence to list
            for (int i = 0; i < sequenceList.Count; i++) {
                sequence_t sequence = sequenceList[i];
                xwDockSequenceList item = new xwDockSequenceList(i, sequence.name);
                item.sequenceRequest += sequenceRequest;
                sequenceItemList.Children.Add(item);
            }
        }

        public void hide() {
            this.Visibility = Visibility.Collapsed;
        }

        private void bClose_Click(object sender, RoutedEventArgs e) {
            hide();
        }

        private void bCreateSequence_Click(object sender, RoutedEventArgs e) {
            sequenceNameNew.show();
        }

        private void UserControl_IsVisibleChanged(object sender, DependencyPropertyChangedEventArgs e) {
            sequenceNameNew.hide();
        }

        private void sequenceName_nameUpdated(string newSequenceName) {
            sequenceNameNew.hide();
            if (sequenceManagement != null) sequenceManagement(sequenceManagement_t.NEW_SEQUENCE, -1, newSequenceName);
        }

        private void sequenceRequest(xwDockSequenceList sender, xwDockSequenceList.sequenceRequest_t request) {
            if (request == xwDockSequenceList.sequenceRequest_t.DELETE_SEQUENCE) {
                if (sequenceManagement != null) sequenceManagement(sequenceManagement_t.DELETE_SEQUENCE, sender.index, "");
            }

            if (request == xwDockSequenceList.sequenceRequest_t.LOAD_SEQUENCE) {
                if (sequenceManagement != null) sequenceManagement(sequenceManagement_t.LOAD_SEQUENCE, sender.index, "");
            }
        }

        private void bSaveToFile_Click(object sender, RoutedEventArgs e) {
            if (sequenceManagement != null) sequenceManagement(sequenceManagement_t.SAVE_TO_FILE, 0, "");
        }

        private void bLoadFromFile_Click(object sender, RoutedEventArgs e) {
            if (sequenceManagement != null) sequenceManagement(sequenceManagement_t.LOAD_FROM_FILE, 0, "");
        }
    }	

}
