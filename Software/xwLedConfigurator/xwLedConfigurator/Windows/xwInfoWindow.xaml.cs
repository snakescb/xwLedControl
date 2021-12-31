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

namespace xwLedConfigurator
{
    /// <summary>
    /// Interaktionslogik für xwInfoControl.xaml
    /// </summary>
    public partial class xwInfoWindow : UserControl {

		DateTime buildTime;
		string version;

        public xwInfoWindow() {
            InitializeComponent();
			buildTime = GetLinkerTime(Assembly.GetEntryAssembly());
			version = Assembly.GetEntryAssembly().GetName().Version.ToString();
		}

		public static DateTime GetLinkerTime(Assembly assembly) {
			const string BuildVersionMetadataPrefix = "+build";

			var attribute = assembly.GetCustomAttribute<AssemblyInformationalVersionAttribute>();
			if (attribute?.InformationalVersion != null) {
				var value = attribute.InformationalVersion;
				var index = value.IndexOf(BuildVersionMetadataPrefix);
				if (index > 0) {
					value = value[(index + BuildVersionMetadataPrefix.Length)..];
					return DateTime.ParseExact(value, "yyyy-MM-ddTHH:mm:ss:fffZ", CultureInfo.InvariantCulture);
				}
			}

			return default;
		}

	}

	

}
