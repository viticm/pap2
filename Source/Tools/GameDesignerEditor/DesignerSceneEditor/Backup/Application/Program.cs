using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Text;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Data.SqlClient;
using System.IO;

namespace DesignerSceneEditor
{
    static class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            // Splash Screen init
            SplashScreen.ShowSplashScreen();
            Application.DoEvents();

            SplashScreen.SetStatus("Setting Up Application Path ...");
            EV.SetFindPath(Path.GetDirectoryName(Application.ExecutablePath));

            SplashScreen.SetStatus("Registering COM dll ...");
            bool bSucReg = EV.RegComDll("AtlKG3DEngineProxy.dll");
            if (!bSucReg)
            {
                MessageBox.Show("COM注册失败！", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            SplashScreen.SetStatus("Checking Application Instancing ...");
            if (Helper.ProgramAlreadyRunning())
            {
                MessageBox.Show("此工具不能双开。", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            SplashScreen.SetStatus("Initing Editor Config ...");
            EditorLayer.Init();
            SplashScreen.SetStatus("Starting KG3DEngine ...");
            EngineLayer.Init();
            SplashScreen.SetStatus("Loading JX3 Logics ...");
            LogicsLayer.Init();

            // close Splash Screen
            SplashScreen.CheckForIllegalCrossThreadCalls = false;
            if (SplashScreen.SplashForm != null)
                SplashScreen.SplashForm.Owner = EditorLayer.MainFrm;
            SplashScreen.CloseForm();


            EditorLayer.Run();
            

            LogicsLayer.Uninit();
            EngineLayer.Uninit();
            EditorLayer.Uninit();

        }

        class EV
        {
            [DllImport("Kernel32")]
            public static extern int GetProcAddress(int handle, String funcname);
            [DllImport("Kernel32")]
            public static extern int LoadLibrary(String funcname);
            [DllImport("Kernel32")]
            public static extern int FreeLibrary(int handle);
            public delegate void DllEntryPoint();

            /// <summary>
            /// 给某个目录下的所有子目录追加到path路径
            /// </summary>
            /// <param name="path">目录名</param>
            public static void SetFindPath(string path)
            {
                string spath = Environment.GetEnvironmentVariable("path");
                addSysPath(path, ref spath);
                Environment.SetEnvironmentVariable("path", spath);
            }

            private static void addSysPath(string path, ref string spath)
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

            private static Delegate GetAddress(int dllModule, string functionname, Type t)
            {
                int addr = GetProcAddress(dllModule, functionname);
                if (addr == 0)
                    return null;
                else
                    return Marshal.GetDelegateForFunctionPointer(new IntPtr(addr), t);
            }

            public static bool RegComDll(string strFullFileName)
            {
                int hdll = 0;
                hdll = LoadLibrary(strFullFileName);
                try
                {
                    DllEntryPoint regdll = (DllEntryPoint)GetAddress(hdll, "DllRegisterServer", typeof(DllEntryPoint));
                    if (regdll == null)
                        return false;
                    regdll();
                }
                finally
                {
                    FreeLibrary(hdll);
                }
                return true;
            }
        }
    }
}