using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Text;
using System.Data.SqlClient;
using System.Runtime.InteropServices; // import
using System.Windows.Forms;

namespace NpcTemplateIO
{
    class Helper
    {
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
                if (!Directory.Exists(path)) // 目录不存在
                {
                    Directory.CreateDirectory(path);
                }
            }
            catch (IOException ex)
            {
                //MessageBox.Show("在创建新目录时产生IO异常: " + ex.ToString());
                Console.WriteLine("在创建新目录时产生IO异常: " + ex.ToString());
            }
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="sql">sql语句</param>
        /// <returns>数据表</returns>
        public static DataTable GetDataTable(SqlConnection conn, string sql)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            System.Data.DataTable tbl = ds.Tables[0];
            return tbl;
        }
    }
}
