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

    public partial class brightnesSlider : UserControl {

        public byte value { get { return currentValue; } set { currentValue = value; updateGraphics(); } }
        public Color baseColor { get { return currentBaseColorEnd; } set { currentBaseColorEnd = value; updateGraphics(); } }
        public Color baseColorStart { get { return currentBaseColorStart; } set { currentBaseColorStart = value; updateGraphics(); } }

        protected override void OnMouseLeftButtonDown(MouseButtonEventArgs e) {
            base.OnMouseLeftButtonDown(e);

            if (sliderborder.IsMouseOver && !slider.IsMouseOver) {
                double newLeft = Mouse.GetPosition(sliderCanvas).X - slider.ActualWidth / 2;
                if (newLeft < 0) newLeft = 0;
                if (newLeft > sliderCanvas.ActualWidth - slider.ActualWidth) newLeft = sliderCanvas.ActualWidth - slider.ActualWidth;
                currentValue = (byte)(newLeft * 255 / (sliderCanvas.ActualWidth - slider.ActualWidth));
                updateGraphics();
            }
        }

        protected override void OnMouseMove(MouseEventArgs e) {
			base.OnMouseMove(e);

            if (!dragging) {
                if (slider.IsMouseOver) {
                    Cursor = Cursors.Hand;
                    if (e.LeftButton == MouseButtonState.Pressed) {
                        dragStartMouse = e.GetPosition(sliderCanvas).X;
                        dragStartSlider = Canvas.GetLeft(slider);
                        Mouse.Capture(this);
                        dragging = true;
                    }
                }
                else if (sliderborder.IsMouseOver) Cursor = Cursors.Hand;
                else Cursor = Cursors.Arrow;
            }
            else {
                if (e.LeftButton == MouseButtonState.Pressed) {
                    double mouseDelta = e.GetPosition(sliderCanvas).X - dragStartMouse;
                    double newLeft = dragStartSlider + mouseDelta;
                    if (newLeft < 0) newLeft = 0;
                    if (newLeft > sliderCanvas.ActualWidth - slider.ActualWidth) newLeft = sliderCanvas.ActualWidth - slider.ActualWidth;
                    currentValue = (byte)(newLeft * 255 / (sliderCanvas.ActualWidth - slider.ActualWidth));
                    updateGraphics();
                }
                else {
                    Mouse.Capture(null);
                    dragging = false;
                }
            }
        }

        byte currentValue = 127;
        Color currentBaseColorEnd = Colors.White;
        Color currentBaseColorStart = Colors.Black;

        double dragStartSlider = 0;
        double dragStartMouse = 0;
        bool dragging = false;

        public brightnesSlider() {
            InitializeComponent();
            this.SizeChanged += SizeChangedEventHandler;
        }

        private void SizeChangedEventHandler(object sender, SizeChangedEventArgs e) {            
            updateGraphics();
        }

        public void updateGraphics() {
            sliderBaseColorEnd.Color = currentBaseColorEnd;
            sliderBaseColorStart.Color = currentBaseColorStart;
            display.Text = Math.Round((double)currentValue*100 / 255).ToString() + "%";
            Canvas.SetLeft(slider, (sliderCanvas.ActualWidth - slider.ActualWidth) * currentValue / 255);
            Canvas.SetTop(slider, (sliderCanvas.ActualHeight - slider.ActualHeight) / 2);
        }

        
    }	

}
