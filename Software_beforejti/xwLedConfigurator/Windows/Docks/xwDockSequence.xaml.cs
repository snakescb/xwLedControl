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

        public delegate void eSequenceManagement(sequenceManagement_t action, string sequenceName);
        public event eSequenceManagement sequenceManagement;

        public enum sequenceManagement_t {
            NEW_SEQUENCE,
            DELETE_SEQUENCE
        }

        public xwDockSequence() {
            InitializeComponent();
		}

        public void show(List<cSequence> sequenceList) {
            this.Visibility = Visibility.Visible;
            sequenceItemList.Children.Clear();

            //add elements for each sequence to list
            foreach (cSequence sequence in sequenceList) {
                xwDockSequenceListItem i = new xwDockSequenceListItem(sequence.sequenceName);
                i.sequenceRequest += sequenceRequest;
                sequenceItemList.Children.Add(i);
            }
        }

        public void hide() {
            this.Visibility = Visibility.Collapsed;
        }

        private void bClose_Click(object sender, RoutedEventArgs e) {
            hide();
        }

        private void bCreateSequence_Click(object sender, RoutedEventArgs e) {
            sequenceName.show();
        }

        private void UserControl_IsVisibleChanged(object sender, DependencyPropertyChangedEventArgs e) {
            sequenceName.hide();
        }

        private void sequenceName_nameUpdated(string newSequenceName) {
            sequenceName.hide();
            if (sequenceManagement != null) sequenceManagement(sequenceManagement_t.NEW_SEQUENCE, newSequenceName);
        }

        private void sequenceRequest(xwDockSequenceListItem sender, xwDockSequenceListItem.sequenceRequest_t request) {
            if (request == xwDockSequenceListItem.sequenceRequest_t.DELETE_SEQUENCE) {
                if (sequenceManagement != null) sequenceManagement(sequenceManagement_t.DELETE_SEQUENCE, sender.sequenceName);
            }
        }

    }	

}
