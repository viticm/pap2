using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using System.IO;
using System.Windows.Forms;
using System.Data.OleDb;

namespace StandardNpcCreate
{
    class Helper
    {
        private static Helper helper;

        private Helper()
        {

        }

        /// <summary>
        /// 获取辅助类实例
        /// </summary>
        /// <returns>辅助类实例</returns>
        public static Helper GetHelper()
        {
            if (helper == null)
            {
                helper = new Helper();
            }

            return helper;
        }

        /// <summary>
        /// 获取文件文本内容
        /// </summary>
        /// <param name="filePath">文件路径</param>
        /// <returns>文本内容</returns>
        public string GetFileContent(string filePath)
        {
            string content = null;

            if (File.Exists(filePath))
            {
                StreamReader reader = new StreamReader(filePath, Encoding.GetEncoding("gb2312"));
                content = reader.ReadToEnd();
                reader.Close();
            }    
            else
            {
                MessageBox.Show(string.Format("文件{0}不存在！", filePath), "读取数据文件",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            return content;
        }

        /// <summary>
        /// 创建数据表
        /// </summary>
        /// <param name="content">内容</param>
        /// <returns>数据表</returns>
        public DataTable CreateDataTable(string content)
        {
            DataTable table = new DataTable();
            List<string> fieldList = new List<string>();

            bool firstLine = true;
            string[] lines = content.Split(new char[] { '\n' });

            foreach (string s in lines)
            {
                string line = s.TrimEnd(new char[] { '\r' });

                if (line != "") // 跳过空的数据行
                {
                    string[] data = line.Split(new char[] { '\t' });

                    if (firstLine)
                    {
                        firstLine = false;

                        for (int i = 0; i < data.Length; i++)
                        {
                            DataColumn dataColumn = new DataColumn(data[i]);
                            table.Columns.Add(dataColumn);
                            fieldList.Add(data[i]);
                        }
                    }
                    else
                    {
                        DataRow dataRow = table.NewRow();

                        for (int i = 0; i < data.Length; i++)
                        {
                            dataRow[i] = data[i];
                        }

                        table.Rows.Add(dataRow);
                    }
                }

            }

            return table;
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <param name="sql">sql语句</param>
        /// <returns>数据表</returns>
        public DataTable GetDataTable(SqlConnection conn, string sql)
        {
            DataTable table = null;

            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                SqlDataAdapter adp = new SqlDataAdapter(sql, conn);
                adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
                DataSet ds = new DataSet();
                adp.Fill(ds);
                table = ds.Tables[0];
            }
            catch (SqlException ex)
            {
                ; // 暂时不需要弹出报错消息
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            return table;
        }

        /// <summary>
        /// 检查字符串是否有效
        /// </summary>
        /// <param name="inputString">输入字符串</param>
        /// <returns>是否出错</returns>
        public bool CheckStringValid(string inputString)
        {
            bool valid = false;

            if (!string.IsNullOrEmpty(inputString))
            {
                string newString = inputString.Trim(new char[] { ' ', '　' });

                if (!string.IsNullOrEmpty(newString))
                {
                    valid = true;
                }
            }

            return valid;
        }

        /// <summary>
        /// 将数据写入文件
        /// </summary>
        /// <param name="fileName">文件路径</param>
        /// <param name="content">文本内容</param>
        public void SaveDataToFile(string fileName, string content)
        {
            try
            {
                string filePath = Path.GetDirectoryName(fileName);
                FileInfo fi = new FileInfo(fileName);
                StreamWriter sw;

                if (!fi.Exists) // 文件不存在
                {
                    if (!Directory.Exists(filePath)) // 目录不存在
                    {
                        Directory.CreateDirectory(filePath); // 先创建目录，再创建文件
                    }

                    FileStream fs = fi.Create();
                    fs.Close();
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                }
                else // 文件已经存在
                {
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                }

                sw.Write(content);
                sw.Close();
            }
            catch (IOException ex)
            {
                ;
            }
        }

        /// <summary>
        /// 从excel文件中创建数据表
        /// </summary>
        /// <param name="filePath">excel文件路径</param>
        /// <returns>数据表</returns>
        public DataTable CreateDataTableFromExcelFile(string filePath, string sheetName)
        {
            DataTable table = new DataTable();

            string connectString = string.Format("Provider=Microsoft.Jet.OLEDB.4.0;Data Source={0};Extended Properties=\"Excel 8.0;HDR=YES;IMEX=1\"", filePath);
            OleDbConnection oledbConnection = new OleDbConnection(connectString);
            OleDbDataAdapter myCommand = null;
            DataSet ds = new DataSet();
            string sqlString = string.Format("SELECT * FROM [{0}$]", sheetName);

            try
            {
                oledbConnection.Open();
                myCommand = new OleDbDataAdapter(sqlString, oledbConnection);
                myCommand.Fill(ds);
            }
            catch (Exception ex)
            {
                MessageBox.Show("读取Excel信息时产生异常：" + ex.Message, "读取数据",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            finally
            {
                oledbConnection.Close();
                oledbConnection.Dispose();
            }

            table = ds.Tables[0];

            return table;
        }
    }
}
