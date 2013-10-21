using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace AutoQuery
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            QueryDir qd = new QueryDir();
            if (qd.Init())
                qd.StartQuery();
        }
    }
}