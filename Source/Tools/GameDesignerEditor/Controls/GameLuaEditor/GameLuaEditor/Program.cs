using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.IO;

namespace GameLuaEditor
{
    static class Program
    {
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            if (Helper.CheckProcessExist("GameLuaEditor", true))
            {
                MessageBox.Show("此工具不能双开！", "运行检查",
                                MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            Environment.CurrentDirectory = Application.StartupPath;
            EV ev = new EV();
            ev.evPath(Path.GetDirectoryName(Application.ExecutablePath));
            Environment.SetEnvironmentVariable("LUA_PATH", Application.ExecutablePath + @"\Plugins\LuaCheck\?;");
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainForm());
        }
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
            DirectoryInfo di = new DirectoryInfo(path);
            foreach (DirectoryInfo d in di.GetDirectories())
            {
                spath += ";" + d.FullName;
            }
            Environment.SetEnvironmentVariable("path", spath);
            foreach (DirectoryInfo d in di.GetDirectories())
            {
                evPath(d.FullName);
            }
        }
    }
}