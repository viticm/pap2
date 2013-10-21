using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Runtime.InteropServices;


namespace DBManager
{
    public class FileFolderHelper
    {
        // import
        [DllImport("kernel32")]
        private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);

        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);

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
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
                return "";
            }

            string strContent = reader.ReadToEnd();
            reader.Close();

            return strContent;
        }
    }
}
