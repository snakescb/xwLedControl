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

    public partial class voltageSlider : UserControl {

        public byte value { get { return currentValue; } set { updateValue(value); } }

        protected override void OnMouseMove(MouseEventArgs e) {
			base.OnMouseMove(e);

            if (!dragging) {
                Point p = e.GetPosition(slider);
                if ((p.X >= 0) && (p.X <= slider.ActualWidth) && (p.Y >= 0) && (p.Y <= slider.ActualHeight)) {
                    Cursor = Cursors.Hand;
                    if (e.LeftButton == MouseButtonState.Pressed) {
                        dragStartMouse = e.GetPosition(sliderCanvas).X;
                        dragStartSlider = Canvas.GetLeft(slider);
                        Mouse.Capture(this);
                        dragging = true;
                    }
                }
                else Cursor = Cursors.Arrow;
            }
            else {
                if (e.LeftButton == MouseButtonState.Pressed) {
                    double mouseDelta = e.GetPosition(sliderCanvas).X - dragStartMouse;
                    double newLeft = dragStartSlider + mouseDelta;
                    if (newLeft < 0) newLeft = 0;
                    if (newLeft > sliderCanvas.ActualWidth - slider.ActualWidth) newLeft = sliderCanvas.ActualWidth - slider.ActualWidth;
                    currentSliderPosition = newLeft * 100 / (sliderCanvas.ActualWidth - slider.ActualWidth);
                    updateGraphics();
                }
                else {
                    Mouse.Capture(null);
                    dragging = false;
                }
            }
        }

        byte currentValue = 127;
        double currentSliderPosition = 50; //%
        Color currentBaseColorEnd = Colors.White;
        Color currentBaseColorStart = Colors.Black;

        double dragStartSlider = 0;
        double dragStartMouse = 0;
        bool dragging = false;

        public voltageSlider() {
            InitializeComponent();
        }

        private void UserControl_IsVisibleChanged(object sender, DependencyPropertyChangedEventArgs e) {
            updateGraphics();
        }

        private void Brightness_Loaded(object sender, RoutedEventArgs e) {
            updateGraphics();
        }

        public void updateValue(byte newValue) {
            currentValue = newValue;
            if (currentValue == 0) currentSliderPosition = 0;
            else {
                if (currentValue < 50) currentValue = 50;
                currentSliderPosition = currentValue - 40;
            }            
            updateGraphics();
        }

        public void updateGraphics() {

            if (currentSliderPosition < 10) {
                currentValue = 0;
                display.Text = "Off";
            }
            else {
                currentValue = (byte)(currentSliderPosition + 40);
                double currentVoltage = currentValue / 10.0;
                display.Text = String.Format("{0:0.0}V", currentVoltage);
            }

            Canvas.SetLeft(slider, (sliderCanvas.ActualWidth - slider.ActualWidth) * currentSliderPosition / 100);
            Canvas.SetTop(slider, (sliderCanvas.ActualHeight - slider.ActualHeight) / 2);
        }

        
    }	

}
