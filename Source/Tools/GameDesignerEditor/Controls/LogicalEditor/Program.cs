using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Design;
using System.Drawing.Drawing2D;
using System.Text;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.IO;

namespace LogicalEditor
{
    static class Program
    {
        [DllImport("psapi.dll")]
        public static extern bool EnumProcesses(IntPtr processes, int bytes_cap, IntPtr bytes_ret);

        // 全局的3d引擎指针
        public static atlaxLib.Test3DEngine Engine = null; 

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            if (ProgramAlreadyRunning())
            {
                MessageBox.Show("此工具不能双开。", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            EV ev = new EV();
            ev.evPath(Path.GetDirectoryName(Application.ExecutablePath));

            Application.Run(new Form1());
        }

        private static bool ProgramAlreadyRunning()
        {
            IntPtr pid_buffer = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(int)) * 1000);
            IntPtr pbytes_ret_buffer = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(int)));

            EnumProcesses(pid_buffer, Marshal.SizeOf(typeof(int)) * 1000, pbytes_ret_buffer);
            int[] bytes_ret = new int[1];
            Marshal.Copy(pbytes_ret_buffer, bytes_ret, 0, 1);
            int pcount = bytes_ret[0] / Marshal.SizeOf(typeof(int));
            int[] pids = new int[pcount];
            Marshal.Copy(pid_buffer, pids, 0, pcount);

            int GameDesignerToolsRunningInstanceCount = 0;
            foreach (int pid in pids)
            {
                try // 不能获得对应id的process也抛异常，太傻了
                {
                    Process p = Process.GetProcessById(pid);
                    string ToolName = typeof(Program).Namespace;
                    if (p.ProcessName.ToLower() == ToolName.ToLower())
                    {
                        GameDesignerToolsRunningInstanceCount++;
                    }
                }
                catch
                {
                    continue;
                }
            }

            if (GameDesignerToolsRunningInstanceCount > 1)
                return true;
            else
                return false;
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
    };
}