using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;

namespace FlowChartEditor
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

            // 初始化环境变量
            EV ev = new EV();
            ev.AddToEnvironmentPath(Path.GetDirectoryName(Application.ExecutablePath));

            // 检查用户授权
            DBManager.DataBaseManager dataBaseManager = DBManager.DataBaseManager.GetDataBaseManager();
            Helper helper = Helper.GetHelper();
            string hostName = helper.GetHostName();
            List<string> addressList = helper.GetHostAddresses();
            bool valid = false;

            foreach (string s in addressList)
            {
                if (dataBaseManager.CheckAuthority(hostName, s))
                {
                    valid = true;
                    break;
                }
            }

            if (valid)
            {                
                Application.Run(new MainForm());
            }
            else
            {
                MessageBox.Show(string.Format("权限审核失败！\r\n\r\n用户名：{0}\r\nIP：{1}\r\n", hostName, addressList[0]), "授权检查",
                                              MessageBoxButtons.OK, MessageBoxIcon.Information);
            }            
        }

        class EV
        {
            /// <summary>
            /// 给某个目录下的所有子目录追加到path环境变量中
            /// </summary>
            /// <param name="path">目录名</param>
            public void AddToEnvironmentPath(string path)
            {
                string spath = Environment.GetEnvironmentVariable("path");
                AddToEnvironmentPath(path, ref spath);
                Environment.SetEnvironmentVariable("path", spath);
            }

            /// <summary>
            /// 递归给目录下所有子目录追加到path环境变量中
            /// </summary>
            /// <param name="path">当前路径</param>
            /// <param name="spath">总路径</param>
            private void AddToEnvironmentPath(string path, ref string spath)
            {
                DirectoryInfo di = new DirectoryInfo(path);

                foreach (DirectoryInfo d in di.GetDirectories())
                {
                    if (!d.Name.StartsWith("_"))
                    {
                        spath += ";" + d.FullName;
                    }
                }

                foreach (DirectoryInfo d in di.GetDirectories())
                {
                    if (!d.Name.StartsWith("_"))
                    {
                        AddToEnvironmentPath(d.FullName, ref spath);
                    }
                }
            }
        };
    }
}