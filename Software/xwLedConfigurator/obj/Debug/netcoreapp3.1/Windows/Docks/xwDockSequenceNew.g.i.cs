﻿#pragma checksum "..\..\..\..\..\Windows\Docks\xwDockSequenceNew.xaml" "{ff1816ec-aa5e-4d10-87f7-6f4963833460}" "F000A9FD7B587232A9040B436D34E0F3FE8DDA56"
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
    /// xwDockSequenceNew
    /// </summary>
    public partial class xwDockSequenceNew : System.Windows.Controls.UserControl, System.Windows.Markup.IComponentConnector {
        
        
        #line 34 "..\..\..\..\..\Windows\Docks\xwDockSequenceNew.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.TextBox sequenceName;
        
        #line default
        #line hidden
        
        
        #line 36 "..\..\..\..\..\Windows\Docks\xwDockSequenceNew.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button bCreateSequence;
        
        #line default
        #line hidden
        
        
        #line 43 "..\..\..\..\..\Windows\Docks\xwDockSequenceNew.xaml"
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
            System.Uri resourceLocater = new System.Uri("/xwLedConfigurator;component/windows/docks/xwdocksequencenew.xaml", System.UriKind.Relative);
            
            #line 1 "..\..\..\..\..\Windows\Docks\xwDockSequenceNew.xaml"
            System.Windows.Application.LoadComponent(this, resourceLocater);
            
            #line default
            #line hidden
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
            this.sequenceName = ((System.Windows.Controls.TextBox)(target));
            return;
            case 2:
            this.bCreateSequence = ((System.Windows.Controls.Button)(target));
            
            #line 36 "..\..\..\..\..\Windows\Docks\xwDockSequenceNew.xaml"
            this.bCreateSequence.Click += new System.Windows.RoutedEventHandler(this.bCreateSequence_Click);
            
            #line default
            #line hidden
            return;
            case 3:
            this.bAbort = ((System.Windows.Controls.Button)(target));
            
            #line 43 "..\..\..\..\..\Windows\Docks\xwDockSequenceNew.xaml"
            this.bAbort.Click += new System.Windows.RoutedEventHandler(this.bAbort_Click);
            
            #line default
            #line hidden
            return;
            }
            this._contentLoaded = true;
        }
    }
}

