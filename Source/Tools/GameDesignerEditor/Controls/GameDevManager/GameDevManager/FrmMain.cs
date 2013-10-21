using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Xml;

namespace GameDevManager
{
    public partial class FrmMain : Form
    {
        private Process[] m_aProc = { null, null, null, null};
        private Thread[] m_aThread = { null, null, null, null};
        private RECT[] m_aRect = { new RECT(), new RECT(), new RECT(), new RECT() };


        public static string[] aFileNames = { "", "", "","","" };

 
//{ @"D:\Kingsoft\sword3-products\trunk\bishop\KG_BishopD.exe", 
  //                      @"D:\Kingsoft\sword3-products\trunk\server\SO3GameCenter.exe",
    //                    @"D:\Kingsoft\sword3-products\trunk\server\SO3GameServer.exe" };

        public FrmMain()
        {
            Control.CheckForIllegalCrossThreadCalls = false;
            InitializeComponent();
            ReadSetting();
        }

        private void notifyIcon1_DoubleClick(object sender, EventArgs e)
        {
            this.Visible = true;
            this.WindowState = FormWindowState.Normal;
            this.Activate();
        }

        private void CreateGameProcess(int index, string strScript)
        {
            Process proc = new Process();
            proc.StartInfo.FileName = strScript;
            proc.StartInfo.WorkingDirectory = Path.GetDirectoryName(strScript);
            //proc.StartInfo.CreateNoWindow = false;
            proc.StartInfo.UseShellExecute = false; 
            //proc.StartInfo.WindowStyle = ProcessWindowStyle.Minimized;
            proc.Start();
            Thread.Sleep(500);

            //尝试5次捕捉MainWindowHandle
            for (int i = 0; i < 5; i++)
            {
                if (proc.MainWindowHandle == IntPtr.Zero)
                {
                    proc.Kill();
                    proc.Start();
                    Thread.Sleep(500);
                }
            }


            m_aProc[index] = proc;
            Debug.Print("MainWindowHandle:" + proc.MainWindowHandle.ToString());
            ResetAllWindow();
            proc.WaitForExit();
            m_aProc[index] = null;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            btn1Start.Enabled = m_aProc[0] == null;
            btn1Restart.Enabled = btn1Stop.Enabled = m_aProc[0] != null;
            btn2Start.Enabled = m_aProc[1] == null;
            btn2Restart.Enabled = btn2Stop.Enabled = m_aProc[1] != null;
            btn3Start.Enabled = m_aProc[2] == null;
            btn3Restart.Enabled = btn3Stop.Enabled = m_aProc[2] != null;
            btn4Start.Enabled = m_aProc[3] == null;
            btn4Restart.Enabled = btn4Stop.Enabled = m_aProc[3] != null;
        }

        private void tabControl1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void FrmMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            if(this.Visible == true)
            {
                this.WindowState = FormWindowState.Minimized;
                e.Cancel = true;
                this.Visible = false;
            }
        }

        private void FrmMain_Load(object sender, EventArgs e)
        {

        }

        private void FrmMain_FormClosed(object sender, FormClosedEventArgs e)
        {
            //这里不要放任何代码，因为肯定不会执行到这里的
        }

        private void tuicToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveSetting();
            
            for (int i = 0; i < 4; i++)
            {
                if (m_aProc[i] != null)
                {
                    m_aProc[i].Kill();
                    m_aProc[i].CloseMainWindow();
                }
            }
            this.notifyIcon1.Visible = false;
            Environment.Exit(0);
        }

        private void viewToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Visible = !this.Visible;
        }

        private void FrmMain_Move(object sender, EventArgs e)
        {
            //ResetAllWindow(false);
        }

        private void ResetAllWindow()
        {
            ResetAllWindow(aFileNames[4] == "true");
        }

        private void ResetAllWindow(bool bMove)     //是否忽略坐标移动
        {
            CheckBox[] m_aCheckBoxTop = { cb1Top, cb2Top, cb3Top};
            CheckBox[] m_aCheckBoxView = { cb1View, cb2View, cb3View};
            for (int i = 0; i < 3; i++)
            {
                if (m_aProc[i] != null)
                {
                    int intadd = 0x10;
                    if (m_aRect[i].right == 0 || m_aRect[i].bottom == 0)
                    {
                        intadd += 1;
                    }
                    if (bMove == false)
                    {
                        intadd += 2;
                    }
                    if (m_aRect[i].left == 0 && m_aRect[i].top ==0 )
                    {
                        m_aRect[i].left -= this.Left;
                        m_aRect[i].top -= this.Height;
                    }
                    System.Diagnostics.Debug.Print(String.Format( i.ToString() + ":SetWindowPos({0},{1},{2},{3},{4},{5},{6})",
                        m_aProc[i].MainWindowHandle,
                        m_aCheckBoxTop[i].Checked ? -1 : -2,                        //TopMost
                        this.Left + m_aRect[i].left ,                               //left
                        this.Top + m_aRect[i].top,                                  //top
                        m_aRect[i].right,                                           //width
                        m_aRect[i].bottom,                                          //height
                        intadd + (m_aCheckBoxView[i].Checked ? 0x40 : 0x80)         //其他参数                        
                        ));
                    SetWindowPos(m_aProc[i].MainWindowHandle,
                        m_aCheckBoxTop[i].Checked ? -1 : -2,                        //TopMost
                        m_aRect[i].left ,                                           //left
                        m_aRect[i].top,                                             //top
                        m_aRect[i].right,                                           //width
                        m_aRect[i].bottom,                                          //height
                        intadd + (m_aCheckBoxView[i].Checked ? 0x40 : 0x80)         //其他参数
                        );                                          
                }
                else
                {
                    System.Diagnostics.Debug.Print("Find Proc:" + i.ToString() + " Null!");
                }
            }
        }


        [DllImport("user32.dll", EntryPoint = "SendMessageA")]
        public static extern int SendMessage(IntPtr hwnd, int wMsg, int wParam, int lParam);

        [DllImport("user32.dll", EntryPoint = "SetWindowPos")]
        public static extern int SetWindowPos(IntPtr hwnd, int hWndInsertAfter, int x, int y, int cx, int cy, int wFlags);

        [DllImport("user32.dll", EntryPoint = "GetWindowRect")]
        public static extern int GetWindowRect(IntPtr hwnd, ref RECT lpRect);

        [StructLayout(LayoutKind.Sequential)]
        public struct RECT
        {
            public int left;
            public int top;
            public int right;
            public int bottom;
        }



        private void ReadSetting()
        {
            CheckBox[] m_aCheckBoxTop = { cb1Top, cb2Top, cb3Top, cb3Top };
            CheckBox[] m_aCheckBoxView = { cb1View, cb2View, cb3View, cb3View };
            string path = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().GetName().CodeBase).Replace("file:\\", "");
            string striniFile = System.IO.Path.Combine(path, "_setting.ini");
            try
            {
                for (int i = 0; i < 4; i++)
                {
                    m_aCheckBoxView[i].Checked = Convert.ToBoolean(IniIO.IniReadValue(i.ToString(), "cb_view", striniFile));
                    m_aCheckBoxTop[i].Checked = Convert.ToBoolean(IniIO.IniReadValue(i.ToString(), "cb_top", striniFile));
                    m_aRect[i].left = Int32.Parse(IniIO.IniReadValue(i.ToString(), "left", striniFile));
                    m_aRect[i].top = Int32.Parse(IniIO.IniReadValue(i.ToString(), "top", striniFile));
                    m_aRect[i].right = Int32.Parse(IniIO.IniReadValue(i.ToString(), "width", striniFile));
                    m_aRect[i].bottom = Int32.Parse(IniIO.IniReadValue(i.ToString(), "height", striniFile));
                    aFileNames[i] = IniIO.IniReadValue(i.ToString(), "filepath", striniFile);
                    //if (m_aRect[i].left < 0) m_aRect[i].left = 0;
                    //if (m_aRect[i].top < 0) m_aRect[i].top = 0;
                }
                aFileNames[4] = IniIO.IniReadValue("other", "savepos", striniFile);

                //兼容旧版本
                if (aFileNames[3] == "")
                {
                    aFileNames[3] = IniIO.IniReadValue("client", "filepath", striniFile);
                }
                this.Left = Int32.Parse(IniIO.IniReadValue("other", "left", striniFile));
                this.Top = Int32.Parse(IniIO.IniReadValue("other", "top", striniFile));
            }
            catch(Exception ex)
            {

                //读不到就算了吧
            }
        }


        //保存设置
        public void SaveSetting()
        {
            CheckBox[] m_aCheckBoxTop = { cb1Top, cb2Top, cb3Top, cb3Top };
            CheckBox[] m_aCheckBoxView = { cb1View, cb2View, cb3View, cb3View };
            string path = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().GetName().CodeBase).Replace("file:\\", "");
            string striniFile = System.IO.Path.Combine(path, "_setting.ini");

            for (int i = 0; i < 4; i++)
            {
                IniIO.IniWriteValue(i.ToString(), "cb_view", m_aCheckBoxView[i].Checked.ToString(), striniFile);
                IniIO.IniWriteValue(i.ToString(), "cb_top", m_aCheckBoxTop[i].Checked.ToString(), striniFile);
                IniIO.IniWriteValue(i.ToString(), "left", m_aRect[i].left.ToString() , striniFile);
                IniIO.IniWriteValue(i.ToString(), "top", m_aRect[i].top.ToString(), striniFile);
                IniIO.IniWriteValue(i.ToString(), "width", m_aRect[i].right.ToString(), striniFile);
                IniIO.IniWriteValue(i.ToString(), "height", m_aRect[i].bottom.ToString(), striniFile);
                IniIO.IniWriteValue(i.ToString(), "filepath", aFileNames[i], striniFile);
            }
            IniIO.IniWriteValue("other", "savepos", aFileNames[4], striniFile);

            //兼容旧版本
            IniIO.IniWriteValue("client", "filepath", aFileNames[3], striniFile);
            IniIO.IniWriteValue("other", "left", this.Left.ToString(), striniFile);
            IniIO.IniWriteValue("other", "top", this.Top.ToString(), striniFile);
        }

        private void FrmMain_Activated(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.Print("actived");

            //记录子窗口的位置
            for (int i = 0; i < 4; i++)
            {
                if (m_aProc[i] != null)
                {
                    RECT rect = new RECT();
                    Debug.Print(i.ToString() + "GetWindowRect()");
                    GetWindowRect(this.m_aProc[i].MainWindowHandle, ref rect);//ref m_aRect[i]);
                    if (rect.right < 0 || rect.bottom < 0) continue;
                    m_aRect[i].left = rect.left;
                    m_aRect[i].top = rect.top;
                    m_aRect[i].right = rect.right - rect.left;
                    m_aRect[i].bottom = rect.bottom - rect.top;
                    System.Diagnostics.Debug.Print("rect" + i.ToString() + ":" + rect.left.ToString());
                }
            }
            //无法添加这一句，否则主窗口无法click了，反复Activeted
            //FrmMain_SizeChanged(this, new EventArgs());
        }

#region 鸡肋代码
        private void btn1Start_Click(object sender, EventArgs e)
        {
            if (aFileNames[0] == "" || !File.Exists(aFileNames[0]))
            {
                MessageBox.Show("请设置文件路径", "错误",  MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            m_aThread[0] = new Thread(new ThreadStart(StartThread1));
            m_aThread[0].Start();
        }

        private void btn2Start_Click(object sender, EventArgs e)
        {
            if (aFileNames[1] == "" || !File.Exists(aFileNames[1]))
            {
                MessageBox.Show("请设置文件路径", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            m_aThread[1] = new Thread(new ThreadStart(StartThread2));
            m_aThread[1].Start();
        }

        private void btn3Start_Click(object sender, EventArgs e)
        {
            if (aFileNames[2] == "" || !File.Exists(aFileNames[2]))
            {
                MessageBox.Show("请设置文件路径", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            m_aThread[2] = new Thread(new ThreadStart(StartThread3));
            m_aThread[2].Start();
        }

        private void StartThread1()
        {
            CreateGameProcess(0, aFileNames[0]);
        }

        private void StartThread2()
        {
            CreateGameProcess(1, aFileNames[1]);
        }

        private void StartThread3()
        {
            CreateGameProcess(2, aFileNames[2]);
        }

        private void StartThread4()
        {
            CreateGameProcess(3, aFileNames[3]);
        }

        private void btn1Stop_Click(object sender, EventArgs e)
        {
            if (m_aProc[0] != null)
            {
                m_aProc[0].Kill();
                m_aProc[0].CloseMainWindow();
            }
        }

        private void btn2Stop_Click(object sender, EventArgs e)
        {
            if (m_aProc[1] != null)
            {
                m_aProc[1].Kill();
                m_aProc[1].CloseMainWindow();
            }
        }

        private void btn3Stop_Click(object sender, EventArgs e)
        {
            if (m_aProc[2] != null)
            {
                m_aProc[2].Kill();
                m_aProc[2].CloseMainWindow();
            }
        }

        private void checkBox4_CheckedChanged(object sender, EventArgs e)
        {
            ResetAllWindow();
        } 

        private void cb1View_CheckedChanged(object sender, EventArgs e)
        {
            ResetAllWindow();
        }

        private void cb2View_CheckedChanged(object sender, EventArgs e)
        {
            ResetAllWindow();
        }

        private void cb2Top_CheckedChanged(object sender, EventArgs e)
        {
            ResetAllWindow();
        }

        private void cb3View_CheckedChanged(object sender, EventArgs e)
        {
            ResetAllWindow();
        }

        private void cb3Top_CheckedChanged(object sender, EventArgs e)
        {
            ResetAllWindow();
        }

        private void btn1Restart_Click(object sender, EventArgs e)
        {
            btn1Restart.Enabled = false;
            this.btn1Stop_Click(this, new EventArgs());
            Thread.Sleep(500);
            this.btn1Start_Click(this, new EventArgs());
        }

        private void btn2Restart_Click(object sender, EventArgs e)
        {
            btn2Restart.Enabled = false;
            this.btn2Stop_Click(this, new EventArgs());
            Thread.Sleep(500);
            this.btn2Start_Click(this, new EventArgs());
        }

        private void btn3Restart_Click(object sender, EventArgs e)
        {
            btn3Restart.Enabled = false;
            this.btn3Stop_Click(this, new EventArgs());
            Thread.Sleep(500);
            this.btn3Start_Click(this, new EventArgs());
        }

        private void btn4Start_Click(object sender, EventArgs e)
        {
            if (aFileNames[3] == "" || !File.Exists(aFileNames[3]))
            {
                MessageBox.Show("请设置文件路径", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            m_aThread[3] = new Thread(new ThreadStart(StartThread4));
            m_aThread[3].Start();
        }

        private void btn4Stop_Click(object sender, EventArgs e)
        {
            if (m_aProc[3] != null)
            {
                m_aProc[3].Kill();
                m_aProc[3].CloseMainWindow();
            }
        }

        private void btn4Restart_Click(object sender, EventArgs e)
        {
            btn4Restart.Enabled = false;
            this.btn4Stop_Click(this, new EventArgs());
            Thread.Sleep(500);
            this.btn4Start_Click(this, new EventArgs());
        }


#endregion

        private void FrmMain_SizeChanged(object sender, EventArgs e)
        {
            CheckBox[] m_aCheckBoxTop = { cb1Top, cb2Top, cb3Top };
            CheckBox[] m_aCheckBoxView = { cb1View, cb2View, cb3View };
            
            //忽略客户端窗口，故循环3次而已
            if (this.WindowState == FormWindowState.Minimized)
            {
                for (int i = 0; i < 3; i++)
                {
                    if (m_aProc[i] != null)
                    {
                        //最小化
                        if (m_aCheckBoxView[i].Checked)
                            SendMessage(m_aProc[i].MainWindowHandle, 0x112, 0xf020, 0);
                        this.Visible = false;
                    }
                }
            }
            else if (this.WindowState == FormWindowState.Normal)
            {
                for (int i = 0; i < 3; i++)
                {
                    if (m_aProc[i] != null)
                    {
                        //正常
                        if (m_aCheckBoxView[i].Checked)
                        {
                           //Z到最前
                            SetWindowPos(m_aProc[i].MainWindowHandle,
                                -1,           //TopMost
                                0, 0, 0, 0,   //height
                                3             //维持坐标和高宽
                                );
                            SetWindowPos(m_aProc[i].MainWindowHandle,
                                -2,               //NoTopMost
                                0, 0, 100, 100,   //height
                                3                 //维持坐标和高宽
                                ); 
                            /*
                            SetWindowPos(m_aProc[i].MainWindowHandle,
                                (Int32)this.Handle,           //顶
                                0, 0, 100, 100,               //height
                                3                             //维持坐标和高宽
                                );                                        
                             */
                            //显示窗口
                            SendMessage(m_aProc[i].MainWindowHandle, 0x112, 0xf120, 0);
                            ResetAllWindow();
                        }
                    }
                }
            }
        }

        private void 设置ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            FrmSetup fs = new FrmSetup(this);
            fs.ShowDialog();
        }

        private void btnOpenDir_Click(object sender, EventArgs e)
        {
            if(aFileNames[1] != "" && Directory.Exists(Path.GetDirectoryName(aFileNames[1])))
            {
                Process proc = new Process();
                proc.StartInfo.FileName = "explorer.exe";
                proc.StartInfo.Arguments = "\"" + Path.GetDirectoryName(aFileNames[1]) + "\"";
                proc.Start();
            }
        }

        private void btnSetup_Click(object sender, EventArgs e)
        {
            FrmSetup fs = new FrmSetup(this);
            fs.ShowDialog();
        }

        private void contextMenuStrip1_Opening(object sender, CancelEventArgs e)
        {
            if(this.Visible == true)
            {
                this.popViewToolStripMenuItem.Text = "隐藏";
            }
            else
            {
                this.popViewToolStripMenuItem.Text = "显示";
            }
        }

        private void btnOpenClientFolder_Click(object sender, EventArgs e)
        {
            if (aFileNames[1] != "" && Directory.Exists(Path.GetDirectoryName(aFileNames[3])))
            {
                Process proc = new Process();
                proc.StartInfo.FileName = "explorer.exe";
                proc.StartInfo.Arguments = "\"" + Path.GetDirectoryName(aFileNames[3]) + "\"";
                proc.Start();
            }
        }


    }


    public class IniIO
    {
        [DllImport("kernel32")]
        private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);
        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);

        public static void IniWriteValue(string Section, string Key, string Value, string filepath)     //对ini文件进行写操作的函数
        {
            long i = 0;
            i = WritePrivateProfileString(Section, Key, Value, filepath);
        }

        public static string IniReadValue(string Section, string Key, string filepath)                  //对ini文件进行读操作的函数
        {
            StringBuilder temp = new StringBuilder(255);
            int i = GetPrivateProfileString(Section, Key, "", temp, 255, filepath);
            return temp.ToString();
        }
    }
 

}