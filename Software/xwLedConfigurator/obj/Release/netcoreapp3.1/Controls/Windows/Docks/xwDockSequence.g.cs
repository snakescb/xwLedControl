﻿#pragma checksum "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequence.xaml" "{ff1816ec-aa5e-4d10-87f7-6f4963833460}" "5959F234D1119392BCBAA00D42BBB8F81F3D6698"
//------------------------------------------------------------------------------
// <auto-generated>
//     Dieser Code wurde von einem Tool generiert.
//     Laufzeitversion:4.0.30319.42000
//
//     Änderungen an dieser Datei können falsches Verhalten verursachen und gehen verloren, wenn
//     der Code erneut generiert wird.
// </auto-generated>
//------------------------------------------------------------------------------

using FontAwesome.Sharp;
using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Controls.Ribbon;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Forms.Integration;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Effects;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
using System.Windows.Media.TextFormatting;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Shell;
using xwLedConfigurator;


namespace xwLedConfigurator {
    
    
    /// <summary>
    /// xwDockSequence
    /// </summary>
    public partial class xwDockSequence : System.Windows.Controls.UserControl, System.Windows.Markup.IComponentConnector {
        
        
        #line 21 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequence.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button bClose;
        
        #line default
        #line hidden
        
        
        #line 37 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequence.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button bCreateSequence;
        
        #line default
        #line hidden
        
        
        #line 43 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequence.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button bLoadFromFile;
        
        #line default
        #line hidden
        
        
        #line 49 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequence.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button bSaveToFile;
        
        #line default
        #line hidden
        
        
        #line 55 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequence.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button bLoadFromDevice;
        
        #line default
        #line hidden
        
        
        #line 61 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequence.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button bSaveToDevice;
        
        #line default
        #line hidden
        
        
        #line 80 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequence.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.StackPanel sequenceItemList;
        
        #line default
        #line hidden
        
        
        #line 87 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequence.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal xwLedConfigurator.xwDockSequenceNew sequenceCreate;
        
        #line default
        #line hidden
        
        private bool _contentLoaded;
        
        /// <summary>
        /// InitializeComponent
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        [System.CodeDom.Compiler.GeneratedCodeAttribute("PresentationBuildTasks", "6.0.1.0")]
        public void InitializeComponent() {
            if (_contentLoaded) {
                return;
            }
            _contentLoaded = true;
            System.Uri resourceLocater = new System.Uri("/xwLedConfigurator;component/controls/windows/docks/xwdocksequence.xaml", System.UriKind.Relative);
            
            #line 1 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequence.xaml"
            System.Windows.Application.LoadComponent(this, resourceLocater);
            
            #line default
            #line hidden
        }
        
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        [System.CodeDom.Compiler.GeneratedCodeAttribute("PresentationBuildTasks", "6.0.1.0")]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1811:AvoidUncalledPrivateCode")]
        internal System.Delegate _CreateDelegate(System.Type delegateType, string handler) {
            return System.Delegate.CreateDelegate(delegateType, this, handler);
        }
        
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        [System.CodeDom.Compiler.GeneratedCodeAttribute("PresentationBuildTasks", "6.0.1.0")]
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Never)]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Design", "CA1033:InterfaceMethodsShouldBeCallableByChildTypes")]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Maintainability", "CA1502:AvoidExcessiveComplexity")]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1800:DoNotCastUnnecessarily")]
        void System.Windows.Markup.IComponentConnector.Connect(int connectionId, object target) {
            switch (connectionId)
            {
            case 1:
            
            #line 9 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequence.xaml"
            ((xwLedConfigurator.xwDockSequence)(target)).IsVisibleChanged += new System.Windows.DependencyPropertyChangedEventHandler(this.UserControl_IsVisibleChanged);
            
            #line default
            #line hidden
            return;
            case 2:
            this.bClose = ((System.Windows.Controls.Button)(target));
            
            #line 21 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequence.xaml"
            this.bClose.Click += new System.Windows.RoutedEventHandler(this.bClose_Click);
            
            #line default
            #line hidden
            return;
            case 3:
            this.bCreateSequence = ((System.Windows.Controls.Button)(target));
            
            #line 37 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequence.xaml"
            this.bCreateSequence.Click += new System.Windows.RoutedEventHandler(this.bCreateSequence_Click);
            
            #line default
            #line hidden
            return;
            case 4:
            this.bLoadFromFile = ((System.Windows.Controls.Button)(target));
            
            #line 43 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequence.xaml"
            this.bLoadFromFile.Click += new System.Windows.RoutedEventHandler(this.bLoadFromFile_Click);
            
            #line default
            #line hidden
            return;
            case 5:
            this.bSaveToFile = ((System.Windows.Controls.Button)(target));
            
            #line 49 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequence.xaml"
            this.bSaveToFile.Click += new System.Windows.RoutedEventHandler(this.bSaveToFile_Click);
            
            #line default
            #line hidden
            return;
            case 6:
            this.bLoadFromDevice = ((System.Windows.Controls.Button)(target));
            
            #line 55 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequence.xaml"
            this.bLoadFromDevice.Click += new System.Windows.RoutedEventHandler(this.bLoadFromDevice_Click);
            
            #line default
            #line hidden
            return;
            case 7:
            this.bSaveToDevice = ((System.Windows.Controls.Button)(target));
            
            #line 61 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequence.xaml"
            this.bSaveToDevice.Click += new System.Windows.RoutedEventHandler(this.bSaveToDevice_Click);
            
            #line default
            #line hidden
            return;
            case 8:
            this.sequenceItemList = ((System.Windows.Controls.StackPanel)(target));
            return;
            case 9:
            this.sequenceCreate = ((xwLedConfigurator.xwDockSequenceNew)(target));
            return;
            }
            this._contentLoaded = true;
        }
    }
}

