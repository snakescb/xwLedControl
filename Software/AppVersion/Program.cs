using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace AppVersion {
    class Program {

        /*************************************************************************
        * Major und Minor Version der gesamten Lösung hier einstellen
        *************************************************************************/
        private const int AUTO_VERSION_MAJOR = 1;
        private const int AUTO_VERSION_MINOR = 0;


        /*************************************************************************
        * Programm
        *************************************************************************/
	    private static string fileName = "";	
        private static int autoBuild = 0;
        private static int autoRevision = 0;

	    [STAThread]
	    static void Main(string[] args) {

            fileName = args[0];

            if(fileName == "") {
                System.Console.WriteLine("No filename received");
                return;
            }

	        if (!File.Exists(fileName)) {
		        System.Console.WriteLine ("Error: Can not find file \"" + fileName + "\"");
		        return;
	        }

            //create autoBuild values
            TimeSpan diff = DateTime.Now.Subtract(new DateTime(2000, 1, 1));
            long ticks = diff.Ticks;
            autoBuild = (int)(ticks / TimeSpan.TicksPerDay);
            ticks = ticks - autoBuild * TimeSpan.TicksPerDay;
            autoRevision = (int)((ticks / TimeSpan.TicksPerSecond) / 2);

	        System.Console.Write("Processing \"" + fileName + "\"...");
	        StreamReader reader = new StreamReader(fileName);
            StreamWriter writer = new StreamWriter(fileName + ".out");
	        String line;

	        while ((line = reader.ReadLine()) != null) {
		        line = ProcessLine(line);
		        writer.WriteLine(line);
	        }
	        reader.Close();
	        writer.Close();

	        File.Delete(fileName);
	        File.Move(fileName + ".out", fileName);
	        System.Console.WriteLine("Done!");
	    }
	    private static string ProcessLine(string line) {

            bool replace = false;
            string sReplace = "";


            if(line.IndexOf("VERSION_BUILD") > 0) {
                replace = true;
                sReplace = autoBuild.ToString();
            }
            if(line.IndexOf("VERSION_REVISION") > 0) {
                replace = true;
                sReplace = autoRevision.ToString();
            }
            if(line.IndexOf("LAST_BUILD") > 0) {
                replace = true;
                sReplace = "\"" + DateTime.Now.ToString() + "\"";
            }

            if(replace) {
                int pos = line.IndexOf("=");
                line = line.Substring(0, pos);
                line = line + "= " + sReplace +  ";";
            }

            return line;
	    }
     }
}
