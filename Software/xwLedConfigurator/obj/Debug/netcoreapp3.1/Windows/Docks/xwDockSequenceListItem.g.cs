﻿#pragma checksum "..\..\..\..\..\Windows\Docks\xwDockSequenceListItem.xaml" "{ff1816ec-aa5e-4d10-87f7-6f4963833460}" "656FC2D01219F1DD13769773FB7A9773B71C9CE9"
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
    /// xwDockSequenceListItem
    /// </summary>
    public partial class xwDockSequenceListItem : System.Windows.Controls.UserControl, System.Windows.Markup.IComponentConnector {
        
        
        #line 23 "..\..\..\..\..\Windows\Docks\xwDockSequenceListItem.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.TextBlock sequenceDisplayName;
        
        #line default
        #line hidden
        
        
        #line 27 "..\..\..\..\..\Windows\Docks\xwDockSequenceListItem.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button bEdit;
        
        #line default
        #line hidden
        
        
        #line 34 "..\..\..\..\..\Windows\Docks\xwDockSequenceListItem.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Button bDelete;
        
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
            System.Uri resourceLocater = new System.Uri("/xwLedConfigurator;component/windows/docks/xwdocksequencelistitem.xaml", System.UriKind.Relative);
            
            #line 1 "..\..\..\..\..\Windows\Docks\xwDockSequenceListItem.xaml"
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
            this.sequenceDisplayName = ((System.Windows.Controls.TextBlock)(target));
            return;
            case 2:
            this.bEdit = ((System.Windows.Controls.Button)(target));
            
            #line 27 "..\..\..\..\..\Windows\Docks\xwDockSequenceListItem.xaml"
            this.bEdit.Click += new System.Windows.RoutedEventHandler(this.bEdit_Click);
            
            #line default
            #line hidden
            return;
            case 3:
            this.bDelete = ((System.Windows.Controls.Button)(target));
            
            #line 34 "..\..\..\..\..\Windows\Docks\xwDockSequenceListItem.xaml"
            this.bDelete.Click += new System.Windows.RoutedEventHandler(this.bDelete_Click);
            
            #line default
            #line hidden
            return;
            }
            this._contentLoaded = true;
        }
    }
}

