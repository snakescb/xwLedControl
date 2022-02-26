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

        public delegate void eSequenceManagement(sequenceManagement_t action, sequence_t sequence);
        public event eSequenceManagement sequenceManagement;

        public enum sequenceManagement_t {
            NEW_SEQUENCE,
            DELETE_SEQUENCE,
            LOAD_SEQUENCE,
            SAVE_TO_FILE,
            LOAD_FROM_FILE,
            SAVE_TO_DEVICE,
            LOAD_FROM_DEVICE
        }

        public xwDockSequence() {
            InitializeComponent();
		}

        public void show(List<sequence_t> sequenceList) {
            this.Visibility = Visibility.Visible;
            sequenceItemList.Children.Clear();

            //add elements for each sequence to list
            foreach (sequence_t sequence in sequenceList) {
                xwDockSequenceList item = new xwDockSequenceList(sequence);
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
            sequenceCreate.show();
        }

        private void UserControl_IsVisibleChanged(object sender, DependencyPropertyChangedEventArgs e) {
            sequenceCreate.hide();
        }

        private void sequenceCreate_created(sequence_t sequence) {
            sequenceCreate.hide();
            if (sequenceManagement != null) sequenceManagement(sequenceManagement_t.NEW_SEQUENCE, sequence);
        }

        private void sequenceRequest(xwDockSequenceList.sequenceRequest_t request, sequence_t sequence) {
            
            if (request == xwDockSequenceList.sequenceRequest_t.DELETE_SEQUENCE) {
                if (sequenceManagement != null) sequenceManagement(sequenceManagement_t.DELETE_SEQUENCE, sequence);
            }

            if (request == xwDockSequenceList.sequenceRequest_t.LOAD_SEQUENCE) {
                if (sequenceManagement != null) sequenceManagement(sequenceManagement_t.LOAD_SEQUENCE, sequence);
            }
            
        }

        private void bSaveToFile_Click(object sender, RoutedEventArgs e) {
            if (sequenceManagement != null) sequenceManagement(sequenceManagement_t.SAVE_TO_FILE, null);
        }

        private void bLoadFromFile_Click(object sender, RoutedEventArgs e) {
            if (sequenceManagement != null) sequenceManagement(sequenceManagement_t.LOAD_FROM_FILE, null);
        }

        private void bSaveToDevice_Click(object sender, RoutedEventArgs e) {
            if (sequenceManagement != null) sequenceManagement(sequenceManagement_t.SAVE_TO_DEVICE, null);
        }

        private void bLoadFromDevice_Click(object sender, RoutedEventArgs e) {
            if (sequenceManagement != null) sequenceManagement(sequenceManagement_t.LOAD_FROM_DEVICE, null);
        }
    }	

}
