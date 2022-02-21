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

        public byte value { get { return currentValue; } set { currentValue = value; update(); } }
        public Color baseColor { get { return currentBaseColorEnd; } set { currentBaseColorEnd = value; update(); } }
        public Color baseColorStart { get { return currentBaseColorStart; } set { currentBaseColorStart = value; update(); } }

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
                    currentValue = (byte)(newLeft * 255 / (canvasSlider.ActualWidth - slider.ActualWidth));

                    update();
                }
                else dragging = false;
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
            this.Loaded += Brightness_Loaded;
        }

        private void Brightness_Loaded(object sender, RoutedEventArgs e) {
            update();
        }

        public void update() {
            sliderBaseColorEnd.Color = currentBaseColorEnd;
            sliderBaseColorStart.Color = currentBaseColorStart;
            display.Text = Math.Round((double)currentValue*100 / 255).ToString() + "%";
            Canvas.SetLeft(slider, (canvasSlider.ActualWidth - slider.ActualWidth) * currentValue / 255);
        }

    }	

}
