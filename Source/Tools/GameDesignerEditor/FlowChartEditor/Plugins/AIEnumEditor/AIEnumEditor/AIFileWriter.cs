using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Data;
using System.IO;

namespace AIEnumEditor
{
    class AIFileWriter
    {
        private string rootDir; // 客户端根目录
        private SqlConnection conn; // sql连接
        private SqlConnection headConn; // head库的sql连接

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="rootDir">客户端根目录</param>
        /// <param name="conn">sql连接</param>
        public AIFileWriter(string rootDir, SqlConnection conn)
        {
            this.rootDir = rootDir;
            this.conn = conn;
            InitHeadConnection();
        }

        private void InitHeadConnection()
        {
            string connectString = "Server = 10.20.86.11; DataBase = s3design_rtm; Uid = s3design; Password = davidbowie;";
            headConn = new SqlConnection(connectString);
        }

        /// <summary>
        /// 导出ls文件
        /// </summary>
        public void ExportLSFile()
        {
            string filePath = Path.Combine(rootDir, @"scripts\ai\argumentStrings.ls");
            string content = ConstructLSFile();
            SaveData(filePath, content);
        }

        /// <summary>
        /// 导出自定义动作文件
        /// </summary>
        public void ExportCustomFunctionsFile()
        {
            string filePath = Path.Combine(rootDir, @"scripts\ai\customFuctions.lua");
            string content = GetUserActionContext();
            SaveData(filePath, content);
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询字符串</param>
        /// <param name="connection">sql连接</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sqlString, SqlConnection connection)
        {
            DataTable tb = null;

            try
            {
                if (connection.State == ConnectionState.Closed)
                {
                    connection.Open();
                }

                SqlDataAdapter adp = new SqlDataAdapter(sqlString, connection);
                adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
                DataSet ds = new DataSet();
                adp.Fill(ds);
                tb = ds.Tables[0];
            }
            finally
            {
                if (connection.State == ConnectionState.Open)
                {
                    connection.Close();
                }
            }
                       
            return tb;
        }

        /// <summary>
        /// 获取AI自定义动作的内容
        /// </summary>
        /// <returns>自定义动作的内容</returns>
        private string GetUserActionContext()
        {
            string sql = "SELECT script, ename, parms FROM ai_action_define WHERE actionid > 1000";
            StringBuilder strRet = new StringBuilder();
            strRet.AppendLine("-- 自定义Action");
            strRet.AppendLine("Include('scripts/ai/argumentStrings.ls')");
            strRet.AppendLine();
            DataTable dt = GetDataTable(sql, conn);

            foreach (DataRow dr in dt.Rows)
            {
                string str = "function {0}(character, actionData)\r\n        local p1, p2, p3, p4, p5 = actionData.GetParam();\r\n";
                strRet.Append(string.Format(str, dr["ename"].ToString()));
                string[] strPList = dr["parms"].ToString().Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);

                for (int i = 1; i <= strPList.Length; i++)
                {
                    string l = strPList[i - 1] + ",";

                    if (l.IndexOf(",-2,") != -1)
                    {
                        strRet.AppendLine(string.Format("        p{0} = ARGUMENT_STRING_TABLE[p{0}];", i.ToString()));
                    }
                }

                strRet.AppendLine(string.Format("        return _{0}(character, p1, p2, p3, p4, p5);\r\nend", dr["ename"].ToString()));
                strRet.Append(dr[0].ToString());
                strRet.AppendLine();
            }

            return strRet.ToString();
        }

        /// <summary>
        /// 构建LS表
        /// </summary>
        /// <returns>LS表内容</returns>
        private string ConstructLSFile()
        {
            string sqlString = "SELECT * FROM AI_LS";
            DataTable lsTable = GetDataTable(sqlString, headConn);
            StringBuilder code = new StringBuilder();

            code.AppendLine("ARGUMENT_STRING_TABLE = {");

            for (int i = 0; i < lsTable.Rows.Count; i++)
            {
                DataRow row = lsTable.Rows[i];
                string id = row["ID"].ToString();
                string word = row["Content"].ToString();

                if (i == lsTable.Rows.Count - 1) // 最后一个数据
                {
                    if ((word.StartsWith("'") && word.EndsWith("'")) ||
                        (word.StartsWith("\"") && word.EndsWith("\"") ||
                        (word.StartsWith("[[") && word.EndsWith("]]")))) // 检查是否已经带字符串标记
                    {
                        code.AppendLine(string.Format("    [{0}] = {1}", id, word));
                    }
                    else
                    {
                        code.AppendLine(string.Format("    [{0}] = '{1}'", id, word));
                    }
                }
                else
                {
                    if ((word.StartsWith("'") && word.EndsWith("'")) ||
                        (word.StartsWith("\"") && word.EndsWith("\"") ||
                        (word.StartsWith("[[") && word.EndsWith("]]")))) // 检查是否已经带字符串标记
                    {
                        code.AppendLine(string.Format("    [{0}] = {1},", id, word));
                    }
                    else
                    {
                        code.AppendLine(string.Format("    [{0}] = '{1}',", id, word));
                    }
                }
            }

            code.Append("}");

            return code.ToString();
        }

        /// <summary>
        /// 保存默认数据行
        /// </summary>
        /// <param name="fileName">文件名</param>
        /// <param name="content">数据内容</param>
        private void SaveData(string fileName, string content)
        {
            // 因为可能被自动导出调用，不捕获IO异常，让上层处理输出
            FileInfo fi = new FileInfo(fileName);
            StreamWriter sw;

            if (!fi.Exists) // 文件不存在
            {
                string path = Path.GetDirectoryName(fileName); // 获取目录
                if (!Directory.Exists(path)) // 目录不存在
                {
                    Directory.CreateDirectory(path); // 先创建目录，再创建文件
                }

                FileStream fs = fi.Create();
                fs.Close();
                sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
            }
            else
            {
                sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
            }

            sw.Write(content);
            sw.Close();            
        }
    }
}
