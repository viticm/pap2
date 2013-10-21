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

namespace UpdateMiddleMapNPCxyz
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

        public static void SceneToLogical(ref int X, ref int Y, ref int Z, float posX, float posY, float posZ, float fGroundHeight, float LogicalXStart, float LogicalZStart)
        {
            const int LOGICAL_CELL_CM_LENGTH = 50;
            const float ALTITUDE_UNIT = LOGICAL_CELL_CM_LENGTH / 4.0f;
            const float TERRAIN_MIN_HEIGHT = -65536.0f / 4.0f * ALTITUDE_UNIT;
            const int ALTITUDE_BIT_NUM = 6;
            const int POINT_PER_ALTITUDE = 1 << ALTITUDE_BIT_NUM;

            X = (int)((posX - LogicalXStart) / 50.0F * 32);
            Y = (int)((posZ - LogicalZStart) / 50.0F * 32);
            Z = (int)((posY - TERRAIN_MIN_HEIGHT) * POINT_PER_ALTITUDE / ALTITUDE_UNIT);
        }

        public static void SwapF(ref float f1, ref float f2)
        {
            float dummi = f1;
            f1 = f2;
            f2 = dummi;
        }

        public static int Abs(int i)
        {
            if (i > 0) return i;
            else return -i;
        }

    

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
        public static System.Data.DataTable GetDataTable(string sql, SqlConnection conn)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
        }
        public static void ExecuteSqlCmd(string sql, SqlConnection conn)
        {
            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }
            SqlCommand cmd = new SqlCommand(sql, conn);
            
            try
            {
                cmd.ExecuteNonQuery();
            }
            catch (System.Exception e)
            {
                MessageBox.Show("execute sql cmd error:" + e.ToString());
            }
            finally
            {
                conn.Close();
            }
            
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
    }
}
