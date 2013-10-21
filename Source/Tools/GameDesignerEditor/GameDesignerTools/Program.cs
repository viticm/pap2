using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Text;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Data.SqlClient;
using System.IO;
using System.Configuration;
using DataTableRemotingClient;

namespace GameDesingerTools
{
    static class Program
    {
        [DllImport("psapi.dll")]
        public static extern bool EnumProcesses(IntPtr processes, int bytes_cap, IntPtr bytes_ret);

        static public bool m_bLoadBackupDB = false; // 是否读取策划库内测前备份（2008.11.15）的数据库
        static private bool m_bRestart = false; // 是否重启程序
        static public string m_strrootdir = "";
        static private string m_strconnstr = "";
        static private string m_strEngineDir = "";
        static private MainForm m_MainForm = null;
        static public RightManager m_rightmgr = new RightManager();
        static public int m_3d_tmrRender = 50;

        static public FTPclient m_ftp = null;
        static string m_ftp_hostName = "changan"; // changan.rdev.kingsoft.net
        static string m_ftp_user = "ftpadmin";
        static string m_ftp_psw = "ddd12345678";
        static public string m_ftp_basedir = "/s3design_import_files/";

        static public bool m_bUseNewAcl = true;// 是否启用模块只读等权限限制。
        static public bool m_bCommentOnSave = true; // 是否启用提交必须填备注的功能。
        static public bool m_bLockDBForCompetition = false; // 策划锁库比赛
        static public bool forPublic = false; // 是否开启public版功能
        static public bool m_bSkillBranch = false; // 技能系统组分支2009-5-11

        static public bool EnableRemoting = false;

        static public string Database = string.Empty;

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            if (ConfigurationManager.AppSettings["remoting"] == "true")
            {
                EnableRemoting = true;
              
                
            }

            if (args.Length < 1)
            {
                MessageBox.Show(string.Format("启动失败，请使用 UpdatePrograme.exe 启动本程序。"), "错误",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }


#if (!_DEBUG)
            if (ProgramAlreadyRunning() && args[0] != "-Debug")
            {
                MessageBox.Show("此工具不能双开。", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
#endif

            //add by cuiwei 07.8.21
            EV ev = new EV();
            ev.evPath(Path.GetDirectoryName(Application.ExecutablePath));            
            //add end
            
            // ftp init
            m_ftp = new FTPclient(m_ftp_hostName, m_ftp_user, m_ftp_psw);
            //m_ftp.Upload("c:/path.txt", m_ftp_basedir + "xxxx");

            // 数据库选择
            if (!m_bSkillBranch)
            {
                if (forPublic)
                {
                    m_bLoadBackupDB = false;
                }
                else
                {
                    //DialogResult dr = MessageBox.Show("要读Head策划库，点<是>；\r\n要读分支库(1-0-8-1380，2009.03.25)，点<否>。", "选择策划库", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                    //if (dr == DialogResult.Yes)
                    //    m_bLoadBackupDB = false;
                    //else
                    //    m_bLoadBackupDB = true;

                    DialogResult dr = MessageBox.Show("要读Head策划库，点<是>；\r\n要退出程序，点<否>。", "是否继续", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                    if (dr == DialogResult.Yes)
                        m_bLoadBackupDB = false;
                    else
                        return;
                }
            }
            
            TimeLog log = null;
            
            do
            {
                try
                {
                    if (!InitSettings())
                        return;

                    if (m_bRestart)
                    {
                        m_bRestart = false;
                    }
                    m_MainForm = new MainForm();
                    
                    try
                    {
                        log = new TimeLog();
                        ClearRecordLocks(MainForm.conn);
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show(ex.Message);
                    }

                    if (RightsSettings.AllowSave)
                        log.Log(TimeLog.enumLogType.ltstart, "主窗口", "应用程序启动", "主版本=" + Helper.GetVersionInfo());
                    else
                        log.Log(TimeLog.enumLogType.ltstart, "主窗口", "应用程序启动", "主版本=" + Helper.GetVersionInfo() + " [只读版本]");

                    m_MainForm.Text += "(" + Helper.GetVersionInfo() + ")";

                    Application.Run(m_MainForm);

                    m_rightmgr.Uninit();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(string.Format("{0}", ex.Message), "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
                finally
                {
                    if (log != null)
                    {
                        log.Log(TimeLog.enumLogType.ltend, null, null, null);
                        log.Clear();
                    }
                }
            }
            while(m_bRestart); // 检查是否需要重新启动主窗口

            if (EnableRemoting)
            {
                DataTableServerProxy.DisconnectServer();
            }
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

        private static void ClearRecordLocks(SqlConnection Conn)
        {
            try
            {
                Conn.Open();
            }
            catch (Exception ex)
            {
                string err = string.Format("在SqlConnection.Open时发生错误。\r\n\r\n错误信息：{0}", ex.Message);
                MessageBox.Show(err);
            }

            try
            {
                string sql = string.Format("DELETE FROM sys_lock_record WHERE lockhostname='{0}' or lockhostname is null", Helper.GetHostName());
                /* remoting
                SqlCommand cmd = Conn.CreateCommand();
                cmd.CommandText = sql;
                cmd.ExecuteNonQuery();
                */

                Helper.ExecuteCommand("sys_lock_record", sql, Conn);

            }
            catch (Exception ex)
            {
                string err = string.Format("在清除行锁时发生错误。\r\n\r\n错误信息：{0}", ex.Message);
                MessageBox.Show(err);
            }
            Conn.Close();
        }

        private static bool InitSettings()
        {
            m_strrootdir = string.Empty;
            m_strconnstr = string.Empty;
            m_strEngineDir = string.Empty;
            
            StringBuilder sb = new StringBuilder(255);
            bool bRet = false;
            while (m_strrootdir.Length <= 0)
            {
                FileFolderHelper.ReadIniFile("General", "RootDir", "", sb, 255, Application.StartupPath + "/GameDesingerTools.ini");
                m_strrootdir = sb.ToString();
                m_strrootdir = m_strrootdir.Trim();
                if (m_strrootdir.Length <= 0)
                {
                    MessageBox.Show("设置文件根目录不能为空，请设置您的本地设置文件存在的根目录。如：E:\\work\\Sword3\\sword3DesignBase\\commit_swap)", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    AppOptionsForm fm = null;
                    fm = new AppOptionsForm();
                    if (fm.ShowDialog() == DialogResult.OK)
                        fm.Update();
                    else
                        break;
                }
                bRet = true;
            }

            if (EnableRemoting)
            {
                while (Database == string.Empty)
                {
                    FileFolderHelper.ReadIniFile("Remote", "DataBase", "", sb, 255, Application.StartupPath + "/GameDesingerTools_remote.ini");
                    Database = sb.ToString().Trim();
                    if (Database == string.Empty && Database.Length == 0)
                    {
                        AppOptionsForm frm = new AppOptionsForm();
                        if (frm.ShowDialog() == DialogResult.OK)
                        {
                            Database = frm.DataBase;
                            frm.Update();
                            break;
                        }
                        else
                        {
                            bRet = false;
                            MessageBox.Show("连接数据库名不能为空，请设置数据库。", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        }
                    }
                }

                bRet = true;
            }


            m_strconnstr = Database;

            while (m_strconnstr.Length <= 0 && bRet)
            {
//                 // crypt 
//                 string myString = @"[General]
// ConnString=Server = jx3web; DataBase = s3design_debug; Uid = s3design; Password = davidbowie;
// ";
// 
//                 Misc.SymmetricMethod _sm = new Misc.SymmetricMethod();
//                 string c = _sm.Encrypto(myString);
//                 c = _sm.Decrypto(c);

                bRet = false;

               

                if (!System.IO.File.Exists(Application.StartupPath + "\\GameDesingerTools_Public.ini"))
                {
                    MessageBox.Show("GameDesingerTools_Public.ini 不存在，程序不能启动!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    break;
                }

                
                string filename = Application.StartupPath + "/GameDesingerTools_Public.ini";
                string content = FileFolderHelper.FileToString(filename);
                Misc.SymmetricMethod sm = new Misc.SymmetricMethod();
                content = sm.Decrypto(content);
                Misc.IniStructure m_inis = new Misc.IniStructure();
                m_inis = Misc.IniStructure.ReadIniWithContent(content);
                m_strconnstr = m_inis.GetValue("General", "ConnString");
                


               



                if (m_bLoadBackupDB) // 强制读取策划备份库的信息
                {
                    m_strconnstr = "Server = jx3web; DataBase = s3design-1-0-8-1380-new; Uid = s3design; Password = davidbowie;";
                }
                if (m_bSkillBranch) // 技能系统组分支2009-5-11
                {
                    m_strconnstr = "Server = jx3web; DataBase = s3design_skill_branch; Uid = s3design; Password = davidbowie;";
                }
                
                if (m_strconnstr.Length <= 0)
                {
                    MessageBox.Show("连接字符串不能为空，请设置数据库连接。", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    AppOptionsForm fm = null;
                    fm = new AppOptionsForm();
                    if (fm.ShowDialog() == DialogResult.OK)
                        fm.Update();
                    else
                        break;
                }
                bRet = true;
            }

            while (m_strEngineDir.Length <= 0 && bRet)
            {
                bRet = false;
                FileFolderHelper.ReadIniFile("General", "3DEnginePath", "", sb, 255, Application.StartupPath + "/GameDesingerTools.ini");
                m_strEngineDir = sb.ToString();
                m_strEngineDir = m_strEngineDir.Trim();
                if (m_strEngineDir.Length <= 0)
                {
                    MessageBox.Show("3DEngine 路径设置不能为空，请设置 KG3DEngine.DLL 所在的路径", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    AppOptionsForm fm = null;
                    fm = new AppOptionsForm();
                    if (fm.ShowDialog() == DialogResult.OK)
                        fm.Update();
                    else
                        break;
                }
                bRet = true;
            }

            if (bRet)
            {
                MainForm.InitConn();
                m_rightmgr.Init();
            }

            // 读3d时钟参数
            FileFolderHelper.ReadIniFile("Advance", "tmrRenderInterval", "50", sb, 255, Application.StartupPath + "/GameDesingerTools.ini");
            int iInterval = 50;
            if (int.TryParse(sb.ToString(), out iInterval))
                Program.m_3d_tmrRender = iInterval;

            return bRet;
        }
        public static MainForm MainForm
        {
            get { return m_MainForm; }
        }
        public static string RootDir
        {
            get { return m_strrootdir; }
            set { m_strrootdir = value;
                if (m_strrootdir.Length > 0)
                if (m_strrootdir.Substring(m_strrootdir.Length - 1) != "\\")
                {
                    m_strrootdir += "\\";
                }
            }
        }
        public static string ConnetionString
        {
            get { return m_strconnstr; }
            set { m_strconnstr = value; }
        }
        public static string EngineDir
        {
            get { return m_strEngineDir; }
            set { m_strEngineDir = value; }
        }

        public static bool Restart
        {
            get{return m_bRestart;}
            set{m_bRestart = value;}
        }
    }

    //add by cuiwei 07.8.21
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
    //add end
}