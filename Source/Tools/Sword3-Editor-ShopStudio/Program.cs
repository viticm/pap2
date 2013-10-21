using System;
using System.Collections.Generic;
using System.Windows.Forms;
using XSJ.Sword3.Editor.Studio;

namespace Sword3_Editor_ShopStudio
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new frmMain());
        }
    }
}