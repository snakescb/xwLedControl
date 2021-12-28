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
using FontAwesome.Sharp;

namespace xwLedConfigurator {
    /// <summary>
    /// Interaktionslogik für MenuItem.xaml
    /// </summary>
    public partial class xwMenuItem : UserControl {

        public string Text { get; set; }
        public FontAwesome.Sharp.IconChar Icon { get; set; }
        public event EventHandler click;

        bool _enabled;
        bool _active;

        public xwMenuItem() {
            InitializeComponent();
            _enabled = true;
            _active = false;
            this.DataContext = this;            
        }

        public bool active {
            get { return _active; }
            set {
                _active = value;
                Grid_MouseLeave(null, null);
            }
        }

        public bool enabled {
            get { return _enabled; }
            set {_enabled = value; }
        }

        private void Grid_MouseEnter(object sender, MouseEventArgs e) {
            if (_enabled && !_active) {
                border1.Background = (System.Windows.Media.Brush)Application.Current.Resources["MenuHoverColor"];
                this.MenuText.Foreground = (System.Windows.Media.Brush)Application.Current.Resources["MainFontColor"];
                this.MenuIcon.Foreground = (System.Windows.Media.Brush)Application.Current.Resources["MainFontColor"];
            }
        }

        private void Grid_MouseLeave(object sender, MouseEventArgs e) {
            if (_active) {
                border1.Background = (System.Windows.Media.Brush)Application.Current.Resources["MenuActiveColor"];
                this.MenuText.Foreground = (System.Windows.Media.Brush)Application.Current.Resources["MainFontColor"];
                this.MenuIcon.Foreground = (System.Windows.Media.Brush)Application.Current.Resources["MainFontColor"];
                this.MenuText.FontWeight = FontWeights.Bold;
                this.MenuIcon.FontWeight = FontWeights.Bold;
            }
            else {
                border1.Background = (System.Windows.Media.Brush)Application.Current.Resources["MenuInactiveColor"];
                this.MenuText.Foreground = (System.Windows.Media.Brush)Application.Current.Resources["2ndFontColor"];
                this.MenuIcon.Foreground = (System.Windows.Media.Brush)Application.Current.Resources["2ndFontColor"];
                this.MenuText.FontWeight = FontWeights.Normal;
                this.MenuIcon.FontWeight = FontWeights.Normal;
            }
        }

        private void Grid_MouseDown(object sender, MouseButtonEventArgs e) {
            if (this.click != null && e.ChangedButton==MouseButton.Left && _enabled) this.click(this, e);
        }
    }
}
