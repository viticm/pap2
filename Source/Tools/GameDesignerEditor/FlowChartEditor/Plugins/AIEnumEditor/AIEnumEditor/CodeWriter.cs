using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using System.Windows.Forms;
using System.IO;

namespace AIEnumEditor
{
    class CodeWriter
    {
        private SqlConnection conn; // sql连接
        DataTable diagramTable = null;
        DataTable enumTable = null;
        DataTable parameterTable = null;

        /// <summary>
        /// 构造函数
        /// </summary>
        public CodeWriter()
        {
            Init();
        }

        /// <summary>
        /// 初始化数据
        /// </summary>
        public void Init()
        {
            string connectString = "Server = 10.20.86.11; DataBase = s3design_rtm; Uid = s3design; Password = davidbowie;"; // 默认连接字符串           
            string filename = Path.Combine(Application.StartupPath, "GameDesingerTools_Public.ini");
            FileInfo fi = new FileInfo(filename);

            if (fi.Exists)
            {
                string content = FileFolderHelper.FileToString(filename);
                IniStructure m_inis = new IniStructure();
                m_inis = IniStructure.ReadIniWithContent(content);
                connectString = m_inis.GetValue("General", "ConnString");
            }
            else
            {
                filename = Path.Combine(Application.StartupPath, "GameDesingerTools_Public.ini");
                fi = new FileInfo(filename);
                if (fi.Exists) // 读取AutoExport.ini文件
                {
                    string content = FileFolderHelper.FileToString(filename);
                    SymmetricMethod sm = new SymmetricMethod();
                    content = sm.Decrypto(content);

                    IniStructure m_inis = new IniStructure();
                    m_inis = IniStructure.ReadIniWithContent(content);
                    connectString = m_inis.GetValue("General", "ConnString"); 
                }
            }

            conn = new SqlConnection(connectString);

            try
            {
                if(conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString = "SELECT * FROM AI_Enum_Define";
                enumTable = GetDataTable(sqlString);
                sqlString = "SELECT * FROM AI_TemplateParamTable";
                parameterTable = GetDataTable(sqlString);
                sqlString = "SELECT ID, FullPath FROM changan.s3diagram_rtm.dbo.diagram WHERE Class = 'AI图'";
                diagramTable = GetDataTable(sqlString);
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在读取数据表信息时产生sql异常：" + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }            
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询字符串</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sqlString)
        {
            DataTable tb = null;

            SqlDataAdapter adp = new SqlDataAdapter(sqlString, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            tb = ds.Tables[0];

            return tb;
        }

        /// <summary>
        /// 获取参数值
        /// </summary>
        /// <param name="diagramPath">流程图路径</param>
        /// <param name="value">参数原值</param>
        /// <returns>参数值</returns>
        public string GetArgumentsValue(string diagramPath, string value)
        {
            string newValue = "";
            string relevatePath = diagramPath.Replace("scripts\\ai\\", "");
            relevatePath = relevatePath.Replace(".lua", "");

            DataRow[] rows = diagramTable.Select(string.Format("FullPath = '{0}'", relevatePath));
            if(rows.Length > 0)
            {
                string diagramID = rows[0]["ID"].ToString();
                newValue = GetArgumentString(value, diagramID, parameterTable, enumTable);
            }

            return newValue;
        }

        /// <summary>
        /// 检查AI参数是否有效
        /// </summary>
        /// <param name="aiTypeTable">AI类型数据表</param>
        /// <param name="parameterTable">AI参数数据表</param>
        /// <returns>AI参数是否有效</returns>
        private bool CheckArgumentAvail(DataTable aiTypeTable, DataTable parameterTable)
        {
            bool avail = true;

            foreach(DataRow row in aiTypeTable.Rows)
            {
                string aiType = row["AIType"].ToString();
                if(row["AIDiagramID"] is DBNull || row["ArgValues"] is DBNull)
                {
                    MessageBox.Show(string.Format("ID为{0}的AI模板没有设置参数！", aiType), "完整性检查",
                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return false;
                }

                string diagramID = row["AIDiagramID"].ToString();
                string argValues = row["ArgValues"].ToString();
                string[] datas = argValues.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                DataRow[] rows = parameterTable.Select(string.Format("DiagramID = '{0}'", diagramID), "SortID");

                if(rows.Length != datas.Length)
                {
                    MessageBox.Show(string.Format("ID为{0}的AI模板的参数已经发生过改变，需要重新设置！", aiType), "完整性检查",
                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return false;
                }

                for(int i = 0; i < rows.Length; i++)
                {
                    DataRow dataRow = rows[i];
                    string[] values = datas[i].Split(new char[]{','});

                    if (dataRow["KeyID"].ToString() != values[0])
                    {
                        MessageBox.Show(string.Format("ID为{0}的AI模板的参数已经发生过改变，需要重新设置！", aiType), "完整性检查",
                            MessageBoxButtons.OK, MessageBoxIcon.Information);
                        return false;
                    }
                }
            }

            return avail;
        }

        /// <summary>
        /// 获取类型对应的值
        /// </summary>
        /// <param name="type">类型</param>
        /// <param name="value">参数值</param>
        /// <param name="enumTable">枚举表</param>
        /// <returns>类型对应的值</returns>
        private string GetTypeValue(string type, string value, DataTable enumTable)
        {
            string typeValue = value;

            switch(type)
            {
                case "-1": // 数字
                    {
                        break;
                    }
                case "-2": // 字符串
                    {
                        typeValue = string.Format("\"{0}\"", value);
                        break;
                    }
                default: // 枚举
                    {
                        //DataRow[] rows = enumTable.Select(string.Format("id = '{0}'", type));
                        //if(rows.Length > 0)
                        //{
                        //    DataRow row = rows[0];
                        //    string englishName = row["ename"].ToString();
                        //    string[] datas = row["values"].ToString().Split(new char[] { '\n' });
                        //    datas = datas[int.Parse(value)].TrimEnd(new char[] { '\r' }).Split(new char[] { ',' });
                        //    typeValue = string.Format("{0}.{1}", englishName, datas[1]);
                        //}

                        break;
                    }
            }

            return typeValue;
        }

        /// <summary>
        /// 获取AI参数字符串
        /// </summary>
        /// <param name="value">参数字符串</param>
        /// <param name="diagramID">流程图ID</param>
        /// <param name="parameterTable">参数数据表</param>
        /// <param name="enumTable">枚举数据表</param>
        /// <returns>AI参数字符串</returns>
        private string GetArgumentString(string value, string diagramID, DataTable parameterTable, DataTable enumTable)
        {
            string[] datas = value.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
            string argumentString = "";

            foreach(string data in datas)
            {
                string[] values = data.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                DataRow[] rows = parameterTable.Select(string.Format("KeyID = '{0}' AND DiagramID = '{1}'", values[0], diagramID), "SortID");
                if(rows.Length > 0)
                {
                    DataRow row = rows[0];
                    string type = row["type"].ToString();                    
                    string englishName = row["ename"].ToString();
                    argumentString += string.Format("{0} = {1}, ", englishName, GetTypeValue(type, values[1], enumTable));
                }
            }
            argumentString = argumentString.TrimEnd(new char[] { ',', ' ' });

            return argumentString;
        }

        /// <summary>
        /// 保存默认数据行
        /// </summary>
        /// <param name="fileName">文件名</param>
        /// <param name="content">数据内容</param>
        private void SaveData(string fileName, string content)
        {
            try
            {
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
                MessageBox.Show(string.Format("文件{0}导出成功！", fileName), "AI参数导出",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (Exception ex)
            {
                MessageBox.Show(string.Format("对文件{0}进行IO操作时产生异常: " + ex.ToString(), fileName));
            }
        }
    }
}
