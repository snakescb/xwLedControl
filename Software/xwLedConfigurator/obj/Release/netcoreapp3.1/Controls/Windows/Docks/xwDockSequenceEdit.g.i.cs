﻿#pragma checksum "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequenceEdit.xaml" "{ff1816ec-aa5e-4d10-87f7-6f4963833460}" "496C6AE70B0468B11807140DB2B03D2BA98EE36C"
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
    /// xwDockSequenceEdit
    /// </summary>
    public partial class xwDockSequenceEdit : System.Windows.Controls.UserControl, System.Windows.Markup.IComponentConnector {
        
        
        #line 24 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequenceEdit.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button bClose;
        
        #line default
        #line hidden
        
        
        #line 31 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequenceEdit.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.TextBox sequenceName;
        
        #line default
        #line hidden
        
        
        #line 39 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequenceEdit.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.ComboBox dimMode;
        
        #line default
        #line hidden
        
        
        #line 44 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequenceEdit.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal xwLedConfigurator.brightnesSlider dimmingInfo;
        
        #line default
        #line hidden
        
        
        #line 52 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequenceEdit.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.ComboBox speedMode;
        
        #line default
        #line hidden
        
        
        #line 57 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequenceEdit.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal xwLedConfigurator.speedSlider speedInfo;
        
        #line default
        #line hidden
        
        
        #line 67 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequenceEdit.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button bSave;
        
        #line default
        #line hidden
        
        
        #line 74 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequenceEdit.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button bAbort;
        
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
            System.Uri resourceLocater = new System.Uri("/xwLedConfigurator;component/controls/windows/docks/xwdocksequenceedit.xaml", System.UriKind.Relative);
            
            #line 1 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequenceEdit.xaml"
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
            this.bClose = ((System.Windows.Controls.Button)(target));
            
            #line 24 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequenceEdit.xaml"
            this.bClose.Click += new System.Windows.RoutedEventHandler(this.bAbort_Click);
            
            #line default
            #line hidden
            return;
            case 2:
            this.sequenceName = ((System.Windows.Controls.TextBox)(target));
            return;
            case 3:
            this.dimMode = ((System.Windows.Controls.ComboBox)(target));
            
            #line 39 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequenceEdit.xaml"
            this.dimMode.SelectionChanged += new System.Windows.Controls.SelectionChangedEventHandler(this.dimMode_SelectionChanged);
            
            #line default
            #line hidden
            return;
            case 4:
            this.dimmingInfo = ((xwLedConfigurator.brightnesSlider)(target));
            return;
            case 5:
            this.speedMode = ((System.Windows.Controls.ComboBox)(target));
            
            #line 52 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequenceEdit.xaml"
            this.speedMode.SelectionChanged += new System.Windows.Controls.SelectionChangedEventHandler(this.speedMode_SelectionChanged);
            
            #line default
            #line hidden
            return;
            case 6:
            this.speedInfo = ((xwLedConfigurator.speedSlider)(target));
            return;
            case 7:
            this.bSave = ((System.Windows.Controls.Button)(target));
            
            #line 67 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequenceEdit.xaml"
            this.bSave.Click += new System.Windows.RoutedEventHandler(this.bSave_Click);
            
            #line default
            #line hidden
            return;
            case 8:
            this.bAbort = ((System.Windows.Controls.Button)(target));
            
            #line 74 "..\..\..\..\..\..\Controls\Windows\Docks\xwDockSequenceEdit.xaml"
            this.bAbort.Click += new System.Windows.RoutedEventHandler(this.bAbort_Click);
            
            #line default
            #line hidden
            return;
            }
            this._contentLoaded = true;
        }
    }
}
