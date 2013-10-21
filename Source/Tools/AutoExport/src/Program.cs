using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;

namespace AutoExport
{
    static class Program
    {
        public static int GSTEP = 0;

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            //Application.EnableVisualStyles();
            //Application.SetCompatibleTextRenderingDefault(false);
            //Application.Run(new Form1());

            if (args.Length == 0)
                GSTEP = 0;
            else if (args[0] == "-step1")
                GSTEP = 1;
            else if (args[0] == "-step2")
                GSTEP = 2;
            else
                GSTEP = 0;

            ExportManager exportManager = new ExportManager();
            exportManager.Export();
        }
    }
}