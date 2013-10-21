using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Data;
using System.IO;
using System.Windows.Forms;

namespace AIDiagramPlugin
{
    public class AITableExport
    {
        /// <summary>
        /// 保存自定义动作到文件
        /// </summary>
        /// <param name="strClientPath">输出目录</param>
        /// <param name="conn">sql连接</param>
        public void UserActionToFile(string strClientPath, SqlConnection conn)
        {
            string sqlString = "SELECT actionid, ename, name, rettype FROM AI_Action_Define";
            DataTable actionTable = GetDataTable(sqlString, conn);
            StringBuilder content = new StringBuilder();
            content.AppendLine("ActionID\tEnglishName\tChineseName\tReturnValue");

            foreach(DataRow dataRow in actionTable.Rows)
            {
                content.AppendLine(string.Format("{0}\t{1}\t{2}\t{3}", dataRow["actionid"].ToString(), dataRow["ename"].ToString(),
                                                 dataRow["name"].ToString(), dataRow["rettype"].ToString()));
            }

            StringToFile(content.ToString(), Path.Combine(strClientPath, @"scripts\ai\AIActionDefine.tab"));

            // old code
            // string strContext = DBManager.DataBaseManager.GetDataBaseManager().GetUserActionContext(conn);
            // StringToFile(strContext, Path.Combine(strClientPath, "scripts\\ai\\customFuctions.lua"));
        }
        
        /// <summary>
        /// 保存文本到文件
        /// </summary>
        /// <param name="strContent">文本内容</param>
        /// <param name="strFileName">文件名</param>
        static public void StringToFile(string strContent, string strFileName)
        {
            StringToFile(strContent, strFileName, Encoding.GetEncoding("gb2312")); // 解决乱码关键在于Encoding
        }

        /// <summary>
        /// 保存文本到文件
        /// </summary>
        /// <param name="strContent">文本内容</param>
        /// <param name="strFileName">文件名</param>
        static public void WriteStringToFile(string strContent, string strFileName)
        {
            StringToFile(strContent, strFileName); // 解决乱码关键在于Encoding
        }

        /// <summary>
        /// 保存文本到文件
        /// </summary>
        /// <param name="strContent">文本内容</param>
        /// <param name="strFileName">文件名</param>
        /// <param name="encoding">编码</param>
        static public void StringToFile(string strContent, string strFileName, Encoding encoding)
        {
            try
            {
                if (!File.Exists(strFileName)) // 文件不存在则创建文件
                {
                    CreatePath(strFileName);
                    File.Create(strFileName).Close();
                }

                StreamWriter writer = new StreamWriter(strFileName, false, encoding);
                writer.Write(strContent);
                writer.Close();
            }
            catch (IOException ex)
            {
                MessageBox.Show(string.Format("保存文件{0}失败！", strFileName), "保存文件",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
            }            
        }

        /// <summary>
        /// 检查目录是否存在，不存在则创建新目录
        /// </summary>
        /// <param name="fileName">绝对文件路径</param>
        public static void CreatePath(string fileName)
        {
            string path = Path.GetDirectoryName(fileName); // 获取目录
            
            if (!Directory.Exists(path)) // 目录不存在
            {
                try
                {
                    Directory.CreateDirectory(path);
                }
                catch (IOException ex)
                {
                    MessageBox.Show(string.Format("创建目录{0}失败！", path), "创建目录",
                                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }                       
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询字符串</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sqlString, SqlConnection conn)
        {
            DataTable tb = null;
            ConnectionState state = conn.State;

            try
            {
                conn.Open();
                SqlDataAdapter adp = new SqlDataAdapter(sqlString, conn);
                adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
                DataSet ds = new DataSet();
                adp.Fill(ds);
                tb = ds.Tables[0];
            }
            catch (SqlException ex)
            {
                MessageBox.Show("SQL数据库错误：" + ex.Message);
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return tb;
        }
    }
}
