using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Data;
using System.Data.SqlClient;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Reflection;
using System.Windows.Forms;

namespace MapLogicalFileReader
{
    public class Helper
    {
        // import
        [DllImport("kernel32")]
        private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);
        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);

        public static IniStructure m_inis = new IniStructure();
        private static string m_filename = string.Empty;

        public static void CreatePath(string fileName)
        {
            string path = Path.GetDirectoryName(fileName); // 获取目录
            try
            {
                if (!Directory.Exists(path)) // 目录不存在
                {
                    Directory.CreateDirectory(path);
                }
            }
            catch (IOException ex)
            {
                MessageBox.Show("在创建新目录时产生IO异常: " + ex.ToString());
            }
        }
        public static void StringToFile(string strContent, string strFileName, Encoding encoding)
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
        public static string FileToString(string strFileName)
        {
            return FileToString(strFileName, Encoding.GetEncoding("gb2312"));
        }
        public static string FileToString(string strFileName, Encoding encoding)
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
            string strContent = reader.ReadToEnd();
            reader.Close();
            return strContent;
        }

        // 系统ini文件操作
        public static int ReadIniFile(string section, string key, string def, StringBuilder retVal, int size, string filePath)
        {
            return GetPrivateProfileString(section, key, def, retVal, size, filePath);
        }
        public static long WriteIniFile(string section, string key, string val, string filePath)
        {
            return WritePrivateProfileString(section, key, val, filePath);
        }
        // 优化的ini文件操作
        public static bool OpenIniFile(string filename, string content)
        {
            //m_inis = IniStructure.ReadIni(filename);
            m_inis = IniStructure.ReadIniWithContent(content);
            m_filename = filename;
            return true;
        }
        public static void CloseIniFile()
        {
            m_inis = null;
            m_filename = string.Empty;
        }
        public static string GetIniString(string section, string key)
        {
            return m_inis.GetValue(section, key);
        }

        // db相关
        private System.Data.DataTable GetDataTable(string sql, SqlConnection conn)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
        }

        //执行sql非查询语句
        public static int SqlNoneQureyExcute(SqlConnection conn, string sql)
        {
            int affectedRowCnt = 0;

            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }
            SqlCommand cmd = new SqlCommand(sql, conn);

            try
            {
                affectedRowCnt = cmd.ExecuteNonQuery();
            }
            catch (System.Exception e)
            {
                MessageBox.Show("execute sql cmd error:" + e.ToString());
            }
            finally
            {
                conn.Close();
            }

            return affectedRowCnt;
        }

        // 浏览已存在文件
        public static string BrowserFile(string[] postfixes)
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
        public static string BrowserFile()
        {
            return BrowserFile(new string[] { "" });
        }

        public static bool IsFileExists(string filePath)
        {
            return File.Exists(filePath);
        }
        public static bool IsDirectoryExists(string dir)
        {
            return Directory.Exists(dir);
        }
        //判断字符串能否转换为数字（整数，浮点数）
        public static bool CanConverToNumber(string ConvertingStr)
        {
            bool bRet = System.Text.RegularExpressions.Regex.IsMatch(ConvertingStr, @"^[+-]?\d+([\.]\d+)?$");
            return bRet;
        }
    }
}
