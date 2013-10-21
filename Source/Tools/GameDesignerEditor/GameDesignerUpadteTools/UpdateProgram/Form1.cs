using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.Net;
using System.Threading;
using System.IO;
using System.Diagnostics;


namespace UpdateProgram
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        static bool updating = true;

        static string text = string.Empty;
        private static int percent = 0;

        private string strXMLFile = string.Empty;
        private string AppStartPath = string.Empty;
        private string strServerAddress = string.Empty;
        private string strLocalVersion = string.Empty;
        private string strRemoteVersion = string.Empty;

        private WebClient client = new WebClient();
        private FTPclient ftp;
        private string hostName = "192.168.27.12"; //changan.rdev.kingsoft.net
        private string user = "anonymous";
        private string psw = string.Empty;
        private string ftpBaseDir = "/newtools/";
        //private string ftpBaseDir = "/newtools_skill_branch/";

//         private string hostName = "192.168.41.202";
//         private string user = "normaluser";
//         private string psw = "1234";
//         private string ftpBaseDir = "/newtools/";


        private ArrayList szFileList = new ArrayList(); // 此次更新需要更新的文件列表
        private ArrayList szAllFileList = new ArrayList(); // 服务器上所有文件的列表

        private Hashtable htLocalTimes = new Hashtable(); // 本地文件的 [文件名, 修改时间] 散列表
        private Hashtable htServerTimes = new Hashtable(); // 服务器文件的 散列表

        private static bool forPublic = false; // 是否开始public版功能

        private void Form1_Load(object sender, EventArgs e)
        {
            strXMLFile = Application.StartupPath + "\\local.xml";
            AppStartPath = Application.StartupPath + "\\";

            // 初始化ftp
            ftp = new FTPclient(hostName, user, psw);
            //RunExternCmd();

            if (updating = IsUpdate())
            {
                Thread thread = new Thread(new ThreadStart(progress));
                thread.Start();
            }
            else
            {
                Application.Exit();    
            }
        }

        private void RunExternCmd() // 已经没用了。
        {
            // 运行外部程序，加入网络连接
            Process p = new Process();
            p.StartInfo.FileName = "cmd.exe";
            p.StartInfo.UseShellExecute = false;
            p.StartInfo.CreateNoWindow = true;
            p.StartInfo.Arguments = string.Format(" /c net use \\\\changan \"\"/user:\"{0}\"", Dns.GetHostName());
            p.Start();
            p.WaitForExit();
            p.Close();

            p = new Process();
            p.StartInfo.FileName = "cmd.exe";
            p.StartInfo.UseShellExecute = false;
            p.StartInfo.CreateNoWindow = true;
            p.StartInfo.Arguments = " /c net use \\\\changan /del";
            p.Start();
            p.WaitForExit();
            p.Close();
        }

        protected void progress()
        {    
            UpdateLocalData();
            updating = false;
            //Process.Start("GameDesingerTools.exe");
            Application.Exit();
        }
     
        public bool GetLocalXmlInfo()
        {
            // 加载本地XML
            try
            {
                XmlDocument docLocal = new XmlDocument();
                docLocal.Load(strXMLFile);
                XmlElement rootLocal = docLocal.DocumentElement;

                // 获得下载信息的URL
                strServerAddress = rootLocal.SelectSingleNode("serveraddress").InnerText;
                strLocalVersion = rootLocal.SelectSingleNode("version").InnerText;

                XmlNode nodeFileList = rootLocal.SelectSingleNode("filelist");
                
                int count = nodeFileList.ChildNodes.Count;
                for (int i = 0; i < count; i++)
                {
                    XmlNode itemNode = nodeFileList.ChildNodes[i];
                    string filename = itemNode.Attributes["name"].Value;
                    string local_file_name = filename.Replace(strServerAddress, Application.StartupPath);
                    DateTime time = File.GetLastWriteTime(local_file_name);
                    htLocalTimes[filename] = time;
                }
                
            }
            catch (Exception e) 
            {
                MessageBox.Show(e.Message);
                return false;
            }
            return true;
        }

        public bool GetServerXmlInfo()
        {
            XmlDocument docRemote = new XmlDocument();
            try
            {                
                if (forPublic)
                {
                    string strServerXml = "/newtools/update_public.xml";
                    ftp.Download(strServerXml, Application.StartupPath + "\\update_public.xml", true);
                }
                else
                {
                    string strServerXml = "/newtools/update.xml";
                    ftp.Download(strServerXml, Application.StartupPath + "\\update.xml", true);
                }                
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
                return false;
            }

            if (forPublic)
            {
                docRemote.Load(Application.StartupPath + "\\update_public.xml");
            }
            else
            {
                docRemote.Load(Application.StartupPath + "\\update.xml");
            }            
            
            XmlElement rootRemote = docRemote.DocumentElement;
            strRemoteVersion = rootRemote.SelectSingleNode("version").InnerText;

            XmlNode updateNode = rootRemote.SelectSingleNode("filelist");

            int count = updateNode.ChildNodes.Count;
            for (int i = 0; i < count; i++)
            {
                XmlNode itemNode = updateNode.ChildNodes[i];
                string filename = itemNode.Attributes["name"].Value;
                string strWholePath = string.Format("ftp://{0}{1}{2}", hostName, ftpBaseDir, filename);
                DateTime time = ftp.GetFtpFileTime(strWholePath);
                htServerTimes[filename] = time;
            }
            
            MakeSureDateTimeOfEachServerFileIsGetCorrectly();

            // 比较并加入update的list
            foreach (DictionaryEntry entry in htServerTimes)
            {
                if (htLocalTimes[entry.Key] == null)
                    szFileList.Add(entry.Key);
                else if (DateTime.Compare(Convert.ToDateTime(htLocalTimes[entry.Key]), Convert.ToDateTime(htServerTimes[entry.Key])) < 0)
                    szFileList.Add(entry.Key);

                szAllFileList.Add(entry.Key);
            }

            return true;
        }

        private void MakeSureDateTimeOfEachServerFileIsGetCorrectly()
        {
            // ftp取文件时间 有一个文件总是出错，但再取一次就好了。所以多了这个废函数。
            for (int i = 0; i < 10; i++)
            {
                bool bExistInvalidTime = false;
                Hashtable htTempAdjust = new Hashtable();
                foreach (DictionaryEntry item in htServerTimes)
                {
                    DateTime time = Convert.ToDateTime(item.Value);
                    if (time == DateTime.MinValue)
                    {
                        bExistInvalidTime = true;
                        string strWholePath = string.Format("ftp://{0}{1}{2}", hostName, ftpBaseDir, item.Key);
                        time = ftp.GetFtpFileTime(strWholePath);
                        htTempAdjust[item.Key] = time;
                    }
                }
                foreach (DictionaryEntry itemAdjust in htTempAdjust)
                {
                    htServerTimes[itemAdjust.Key] = itemAdjust.Value;
                }
                if (!bExistInvalidTime)
                    break;
            }
        }

        public bool IsUpdate()
        {
            bool bRetValue = false;
            if (!(bRetValue = GetLocalXmlInfo()))
                goto Exit1;
            if(!(bRetValue = GetServerXmlInfo()))
                goto Exit1;

            if (string.Compare(strLocalVersion, strRemoteVersion) != 0) // 如果更新了版本号，全部下载一次。
            {
                bRetValue = true;
                this.label1.Text = "此版本是旧版本，开始升级";
                this.timer1.Enabled = true;
                szFileList.Clear();
                szFileList = szAllFileList;
            }
            else if (szFileList.Count > 0) // 有需要更新的项
            {
                bRetValue = true;
                this.label1.Text = "此版本是旧版本，开始升级";
                this.timer1.Enabled = true;
            }
            else
            {
                bRetValue = false;
                this.label1.Text = "此版本是最新版本";
            }

            return (bRetValue);

         Exit1:
            this.label1.Text = "网络不通或服务器地不正确！无法更新。";
            return (bRetValue);
        }

        /// <summary>
        /// 更新本地的信息库
        /// </summary>
        public void UpdateLocalData()
        {
            string LocalAdd = "";
        
            //for (int i = 0; i < szFileList.Count; i++)
            int i = 0;
            foreach (string URL in szFileList)
            {
                string name = URL.Replace(strServerAddress, "");

                if (forPublic)
                {
                    if (name == "UpdateProgram_Public.exe")
                    {
                        LocalAdd = AppStartPath + name + ".temp";
                    }
                    else
                    {
                        LocalAdd = AppStartPath + name;
                    }
                }
                else
                {
                    if (name == "UpdateProgram.exe")
                    {
                        LocalAdd = AppStartPath + name + ".temp";
                    }
                    else
                    {
                        LocalAdd = AppStartPath + name;
                    }
                }

                if(!Directory.Exists(Path.GetDirectoryName(LocalAdd)))
                {
                    Directory.CreateDirectory(Path.GetDirectoryName(LocalAdd));
                }

                try
                {
                    text = name;
                    //client.DownloadFile(URL, LocalAdd);
                    if (File.Exists(LocalAdd)) File.Delete(LocalAdd);
                    ftp.Download(ftpBaseDir + URL, LocalAdd, true);
                    File.SetLastWriteTime(LocalAdd, Convert.ToDateTime(htServerTimes[URL]));

                    percent = (int)((++i) * ((float)100 / (float)szFileList.Count));  
                }
                catch (Exception e)
                {
                    MessageBox.Show(e.Message, "错误");
                }
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (updating)
            {
                label2.Text = smoothProgressBar1.Value.ToString() + "%";
                smoothProgressBar1.Value = percent;
                this.label1.Text = "正在下载文件：" + text;
            }
            else
            {
                smoothProgressBar1.Value = 100;
                this.label1.Text = "现在是最新版本了！！";
                this.label2.Text = "100%";
                this.timer1.Enabled = false;
            }
        }

  
    }
   
   
}