using System;
using System.Xml;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;
using System.Collections;
using System.Diagnostics;
using System.Text;
using System.Runtime.InteropServices; // import


namespace autoConfig
{
    static class Program
    {
        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);

        /// <summary>
        /// 自动给GameDesingerTools.exe生成一个配置文件，用于给静态加载的.net dll们指明搜索路径
        /// </summary>
        [STAThread]
        static void Main()
        {
            string strStartPath = string.Empty;
            try
            {
                //strStartPath = Directory.GetParent(Application.StartupPath).FullName;
                string strPath = Path.GetDirectoryName(Process.GetCurrentProcess().MainModule.FileName);
                strStartPath = Directory.GetParent(strPath).FullName;
            }
            catch
            {

            }

            // 暂时加入检查是否退出，有的机子上io实在太慢。
            //string strIniFile = strStartPath + "/GameDesingerTools.ini";
            //StringBuilder sb = new StringBuilder(255);
            //ReadIniFile("PostRun", "autoConfig", "1", sb, 255, strIniFile);
            //if (sb.ToString() == "0")
            //    return;

            // 屏蔽之，IO实在是太慢，加上在乌龟svn时期。而且功能不是十分重要
            genConfig(strStartPath);
            //autoDel();

            return;
        }

        public static int ReadIniFile(string section, string key, string def, StringBuilder retVal, int size, string filePath)
        {
            return GetPrivateProfileString(section, key, def, retVal, size, filePath);
        }

        /// <summary>
        /// 自动生成config
        /// </summary>
        public static void genConfig(string strStartPath)
        {
            FileStream configfs = File.OpenRead(strStartPath + "\\app.config");
            
            StreamReader sr = new StreamReader(configfs);

            string appconfig = string.Empty;
            appconfig = sr.ReadLine();
            appconfig = sr.ReadLine();

            string strOut = "<configuration>\r\n";

            while ((appconfig = sr.ReadLine()) != null)
            {
                if (appconfig.Contains("<runtime>"))
                {
                    break;
                }

                strOut += appconfig + "\r\n";
            }

            sr.Close();
            configfs.Close();

            strOut += "  <runtime>\r\n";
            strOut += "    <assemblyBinding xmlns=\"urn:schemas-microsoft-com:asm.v1\">\r\n";
            strOut += "      <publisherPolicy apply=\"yes\" />\r\n";
            strOut += "      <probing privatePath=\"";


            System.Collections.ArrayList folder_list = new System.Collections.ArrayList();
            foreach (string itemFolder in Directory.GetDirectories(strStartPath, "*", SearchOption.AllDirectories))
            {
                if (!itemFolder.EndsWith("_cache") && !itemFolder.EndsWith("布局"))
                    folder_list.Add(itemFolder.Replace(strStartPath + "\\", ""));
            }
            strOut += String.Join(";", (string[])folder_list.ToArray(typeof(string)));

            strOut += "\" />\r\n";
            strOut += "    </assemblyBinding>\r\n";
            strOut += "    <gcConcurrent enabled=\"true\" />\r\n";
            strOut += "  </runtime>\r\n";
            strOut += "</configuration>";

            string strconfigdir = Path.Combine(strStartPath, "GameDesingerTools.exe.config");
            if (File.Exists(strconfigdir))
            {
                File.Delete(strconfigdir);
            }
            FileStream fs = File.OpenWrite(strconfigdir);
            StreamWriter sw = new StreamWriter(fs);
            sw.Write(strOut);
            sw.Close();
            fs.Close();
        }

        /// <summary>
        /// 删除无用文件/目录
        /// </summary>
        public static void autoDel()
        {
            string strStartPath = "";
            try
            {
                //strStartPath = Directory.GetParent(Application.StartupPath).FullName;
                string strPath = Path.GetDirectoryName(Process.GetCurrentProcess().MainModule.FileName);
                strStartPath = Directory.GetParent(strPath).FullName;
            }
            catch
            {
            }

            Hashtable ht = new Hashtable();
            string strXMLFile = Path.Combine(strStartPath, "local.xml");
            string strServerAddress = "";
            ht.Clear();
            try
            {
                //读取xml中文件列表到hashtable
                XmlDocument docLocal = new XmlDocument();
                docLocal.Load(strXMLFile);
                XmlElement rootLocal = docLocal.DocumentElement;
                strServerAddress = rootLocal.SelectSingleNode("serveraddress").InnerText;
                XmlNode nodeFileList = rootLocal.SelectSingleNode("filelist");
                foreach (XmlNode nodeFile in nodeFileList.ChildNodes)
                {
                    string fname = nodeFile.Attributes["name"].Value.Replace(strServerAddress, strStartPath).ToLower();
                    ht.Add(fname.ToLower(), Path.GetDirectoryName(fname).ToLower());
                }

                //附加不该删文件
                if (!ht.Contains("gamedesingertools.exe.config"))                
                    ht.Add("gamedesingertools.exe.config", strStartPath.ToLower());
                if (!ht.Contains("gamedesingertools.ini"))
                    ht.Add("gamedesingertools.ini", strStartPath.ToLower());
                if(!ht.Contains("gamedesingertools_public.ini"))
                    ht.Add("gamedesingertools_public.ini", strStartPath.ToLower());
                if (!ht.Contains("local.xml"))
                    ht.Add("local.xml", strStartPath.ToLower());

                //枚举本地文件/目录，比较后生成待删除列表，但不删“_”开头的文件以及目录
                ArrayList delList = new ArrayList();
                foreach (string itemFolder in Directory.GetDirectories(strStartPath, "*", SearchOption.AllDirectories))
                {
                    string newItemFolder = itemFolder.Replace(strStartPath + "\\", string.Empty);
                    if (!ht.ContainsValue(newItemFolder.ToLower()) && Directory.GetDirectories(itemFolder).Length == 0)
                    {
                        if (itemFolder.Replace(strStartPath.ToLower(), "").IndexOf("\\_") == -1)
                        {
                            delList.Add(itemFolder.ToLower());
                        }
                    }
                }
                foreach (string itemFile in Directory.GetFiles(strStartPath, "*", SearchOption.AllDirectories))
                {
                    string newItemFile = itemFile.Replace(strStartPath + "\\", string.Empty);
                    newItemFile = newItemFile.Replace("\\", "/");
                    if (!ht.ContainsKey(newItemFile.ToLower()))
                    {
                        if (itemFile.Replace(strStartPath.ToLower(), "").IndexOf("\\_") == -1)
                        {
                            delList.Add(itemFile.ToLower());
                        }
                    }
                }

                //确认删除文件或目录
                if(delList.Count >0)
                {
                    string smess = string.Join("\r\n", (string[])delList.ToArray(typeof(string))).Replace(strStartPath.ToLower() + "\\", "");
                    Application.EnableVisualStyles();
                    Application.SetCompatibleTextRenderingDefault(false);
                    Application.Run(new frmMain((string[])delList.ToArray(typeof(string)), "发现[" + delList.Count + "]个多余文件（目录），是否删除？"));

                }
            }
            catch (Exception e1)
            {
                MessageBox.Show(e1.Message);
            }
        }
    }
}