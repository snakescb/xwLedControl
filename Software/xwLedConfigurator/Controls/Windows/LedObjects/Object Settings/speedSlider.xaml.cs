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
using System.ComponentModel;

namespace xwLedConfigurator {

    public partial class speedSlider : UserControl {

        public long value { get { return (long)(speedFactor * 65536.0); } set { updateSpeed(value); } }

        protected override void OnMouseMove(MouseEventArgs e) {
			base.OnMouseMove(e);

            if (!dragging) {
                Point p = e.GetPosition(slider);
                if ((p.X >= 0) && (p.X <= slider.ActualWidth) && (p.Y >= 0) && (p.Y <= slider.ActualHeight)) {
                    Cursor = Cursors.Hand;
                    if (e.LeftButton == MouseButtonState.Pressed) {
                        dragStartMouse = e.GetPosition(canvasSlider).X;
                        dragStartSlider = Canvas.GetLeft(slider);
                        dragging = true;
                    }
                }
                else Cursor = Cursors.Arrow;                
            }
            else {
                if(e.LeftButton == MouseButtonState.Pressed) {
                    double mouseDelta = e.GetPosition(canvasSlider).X - dragStartMouse;
                    double newLeft = dragStartSlider + mouseDelta;

                    if (newLeft < 0) newLeft = 0;
                    if (newLeft > canvasSlider.ActualWidth - slider.ActualWidth) newLeft = canvasSlider.ActualWidth - slider.ActualWidth;
                    sliderPosition = newLeft * 100 / (canvasSlider.ActualWidth - slider.ActualWidth);

                    refreshGraphics();
                }
                else dragging = false;
            }

        }

        double sliderPosition = 50.0; //in %
        double speedFactor = 1.0;
        double dragStartSlider = 0;
        double dragStartMouse = 0;
        bool dragging = false;

        public speedSlider() {
            InitializeComponent();
            this.Loaded += slider_Loaded;
        }

        private void slider_Loaded(object sender, RoutedEventArgs e) {
            refreshGraphics();
        }

        public void updateSpeed(long newSpeed) {
            speedFactor = newSpeed / 65536.0;
            sliderPosition = Math.Sqrt(((speedFactor - 0.1) * 10000) / 9.9);
            refreshGraphics();
        }

        public void refreshGraphics() {
            speedFactor = (9.9 * (sliderPosition * sliderPosition) / 10000) + 0.1;
            //always round to the next 0.1
            speedFactor = Math.Round(speedFactor * 10) / 10;
            display.Text = String.Format("x{0:0.0}", speedFactor);
            Canvas.SetLeft(slider, (canvasSlider.ActualWidth - slider.ActualWidth) * sliderPosition / 100.0);
        }

    }	

}
