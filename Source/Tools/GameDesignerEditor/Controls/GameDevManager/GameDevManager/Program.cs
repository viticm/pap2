using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace GameDevManager
{
    static class Program
    {
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            if(ProgramAlreadyRunning())
            {
                MessageBox.Show("不允许启动多个实例", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            App.frmMain = new FrmMain();
            Application.Run(App.frmMain);
        }

        [DllImport("psapi.dll")]
        public static extern bool EnumProcesses(IntPtr processes, int bytes_cap, IntPtr bytes_ret);

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
}