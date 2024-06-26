﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace xwLedConfigurator {
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window {

        private Rect restoreLocation;

        public MainWindow() {

            InitializeComponent();
            //btnMenuConfig.active = true;
            //btnMenuLed.active = true;
            menuButton_click(btnMenuLed, null);

            //connect controls
            contentLed.simAuxRequest += dockAuxSim.simAuxRequestHandler;
            contentLed.downloadRequest += dockDownload.downloadRequestHandler;
            contentLed.uploadRequest += dockUpload.uploadRequestHandler;
            dockUpload.uploadFinishedEvent += contentLed.uploadFinished;

            //start connection
            Connection.start();     
        }

        private void menuButton_click(object sender, EventArgs e) {
            xwMenuItem itemClicked = sender as xwMenuItem;

            if (itemClicked != null) {
                if (!itemClicked.active) {
                    btnMenuConfig.active = false;
                    btnMenuFirmware.active = false;
                    btnMenuLed.active = false;

                    itemClicked.active = true;
                }

                contentLed.Visibility = Visibility.Collapsed;
                contentInfo.Visibility = Visibility.Collapsed;
                contentBootloader.Visibility = Visibility.Collapsed;

                if (btnMenuFirmware.active) contentBootloader.Visibility = Visibility.Visible;
                if (btnMenuConfig.active) contentInfo.Visibility = Visibility.Visible;
                if (btnMenuLed.active) contentLed.Visibility = Visibility.Visible;
            }
        }

        private void Border_MouseDown(object sender, MouseButtonEventArgs e) {

            if (e.ChangedButton == MouseButton.Left) { DragMove(); }
        }

        private void ButtonClose_Click(object sender, RoutedEventArgs e) {
            Application.Current.Shutdown();
        }

        private void ButtonMaximize_Click(object sender, RoutedEventArgs e) {

            System.Windows.Forms.Screen currentScreen;
            currentScreen = System.Windows.Forms.Screen.FromPoint(System.Windows.Forms.Cursor.Position);

            if ((Height == currentScreen.WorkingArea.Height) && (Width == currentScreen.WorkingArea.Width) && (Top == 0) && (Left == 0)) {
                Height = restoreLocation.Height;
                Width = restoreLocation.Width;
                Left = restoreLocation.X;
                Top = restoreLocation.Y;
            }
            else {
                restoreLocation = new Rect { Width = Width, Height = Height, X = Left, Y = Top };                
                Height = currentScreen.WorkingArea.Height;
                Width = currentScreen.WorkingArea.Width;
                Top = 0;
                Left = 0;
            }
        }

        private void ButtonMinimize_Click(object sender, RoutedEventArgs e) {
            SystemCommands.MinimizeWindow(this);
        }

        private void Button_Click(object sender, RoutedEventArgs e) {
            this.Close();
        }

        protected override void OnClosed(EventArgs e) {
            base.OnClosed(e);
            App.Current.Shutdown();
            Process.GetCurrentProcess().Kill();
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e) {
            var window = Window.GetWindow(this);
            window.KeyDown += contentLed.HandleKeyPress;
        }

    }
}
