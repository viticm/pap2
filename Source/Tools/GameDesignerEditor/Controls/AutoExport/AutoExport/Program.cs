using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;

namespace AutoExport
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            //Application.EnableVisualStyles();
            //Application.SetCompatibleTextRenderingDefault(false);
            //Application.Run(new Form1());

            EV ev = new EV();
            ev.evPath(Path.GetDirectoryName(Application.ExecutablePath));

            ExportManager exportManager = new ExportManager();
            exportManager.Export();
        }

        class EV
        {
            /// <summary>
            /// 给某个目录下的所有子目录追加到path路径
            /// </summary>
            /// <param name="path">目录名</param>
            public void evPath(string path)
            {
                string spath = Environment.GetEnvironmentVariable("path");
                addSysPath(path, ref spath);
                Environment.SetEnvironmentVariable("path", spath);
            }

            private void addSysPath(string path, ref string spath)
            {
                DirectoryInfo di = new DirectoryInfo(path);
                foreach (DirectoryInfo d in di.GetDirectories())
                {
                    if (!d.Name.StartsWith("_"))
                        spath += ";" + d.FullName;
                }
                foreach (DirectoryInfo d in di.GetDirectories())
                {
                    if (!d.Name.StartsWith("_"))
                        addSysPath(d.FullName, ref spath);
                }
            }
        };
    }
}