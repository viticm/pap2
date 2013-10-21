using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Runtime.InteropServices; // import


namespace CareerIO
{
    public class FileFolderHelper : System.Windows.Forms.Design.FolderNameEditor
    {
        // import
        [DllImport("kernel32")]
        private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);

        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);


        // method
        // 浏览已存在文件
        static public string BrowserFile(string[] postfixes)
        {
            string filter = "";
            foreach (string postfix in postfixes)
            {
                if (postfix != "")
                    filter += string.Format("{0} 文件|*.{1}|", postfix, postfix);
            }

            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = filter + "全部文件|*.*";
            if (ofd.ShowDialog() == DialogResult.OK)
                return ofd.FileName;
            else
                return "";
        }
        static public string BrowserFile()
        {
            return BrowserFile(new string[] { "" });
        }
        // 浏览要保存的文件
        static public string BrowserFileSave()
        {
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "All Files|*.*";
            if (sfd.ShowDialog() == DialogResult.OK)
                return sfd.FileName;
            else
                return "";
        }
        // 浏览文件夹
        static public string BrowserFolder(string description)
        {
            folderBrowser.Description = description;
            if (folderBrowser.ShowDialog() == DialogResult.OK)
                return folderBrowser.DirectoryPath;
            else
                return "";
        }
        // 保存string到文件
        static public void StringToFile(string strContent, string strFileName)
        {
            StringToFile(strContent, strFileName, Encoding.GetEncoding("gb2312")); // 解决乱码关键在于Encoding
        }
        static public void WriteStringToFile(string strContent, string strFileName)
        {
            StringToFile(strContent, strFileName); // 解决乱码关键在于Encoding
        }
        static public void StringToFile(string strContent, string strFileName, Encoding encoding)
        {
            if (!File.Exists(strFileName))
            {
                CreatePath(strFileName);
                File.Create(strFileName).Close();
            }
            StreamWriter writer = new StreamWriter(strFileName, false, encoding);
            writer.Write(strContent);
            writer.Close();
        }

        /// <summary>
        /// 检查目录是否存在，不存在则创建新目录
        /// </summary>
        /// <param name="fileName">绝对文件路径</param>
        public static void CreatePath(string fileName)
        {
            string path = Path.GetDirectoryName(fileName); // 获取目录
            try
            { 
                if(!Directory.Exists(path)) // 目录不存在
                {
                    Directory.CreateDirectory(path);
                }
            }
            catch(IOException ex)
            {
                MessageBox.Show("在创建新目录时产生IO异常: " + ex.ToString());
            } 
        }

        // 读取文件到string
        static public string FileToString(string strFileName)
        {
            return FileToString(strFileName, Encoding.GetEncoding("gb2312"));
        }
        static public string FileToString(string strFileName, Encoding encoding)
        {
            StreamReader reader;
            try
            {
                reader = new StreamReader(strFileName, encoding);
            }
            catch //(Exception e)
            {
                //MessageBox.Show(e.Message);
                return "";
            }

            //StreamReader reader = new StreamReader(strFileName, encoding);
            string strContent = reader.ReadToEnd();
            reader.Close();
            return strContent;
        }
        // ini文件操作
        static public int ReadIniFile(string section, string key, string def, StringBuilder retVal, int size, string filePath)
        {
            return GetPrivateProfileString(section, key, def, retVal, size, filePath);
        }
        static public long WriteIniFile(string section, string key, string val, string filePath)
        {
            return WritePrivateProfileString(section, key, val, filePath);
        }
        // 文件绝对路径名 -> 纯文件名
        static public string FullPathToFileName(string strFullPath)
        {
            int lio1 = strFullPath.LastIndexOf('/');
            int lio2 = strFullPath.LastIndexOf('\\');
            return strFullPath.Substring(lio1 > lio2 ? lio1 + 1 : lio2 + 1);
        }


        // not recommended
        static System.Windows.Forms.Design.FolderNameEditor.FolderBrowser folderBrowser = new System.Windows.Forms.Design.FolderNameEditor.FolderBrowser();
        public FileFolderHelper()
        {
            folderBrowser.Style = FolderBrowserStyles.BrowseForEverything | FolderBrowserStyles.ShowTextBox;
            folderBrowser.StartLocation = FolderBrowserFolder.MyComputer;
        }
        ~FileFolderHelper()
        {
            folderBrowser.Dispose();
        }
    }
}
