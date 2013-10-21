using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using System.Windows.Forms;
using System.IO;
using System.Net;
using System.Collections;
using LuaInterface;

namespace AutoExport
{
    class ExportManager
    {
        private SqlConnection conn; // sql连接
        private StringBuilder logText = new StringBuilder(); // log文本
        private string rootPath; // 导出根目录
        private List<string> autoTableList = new List<string>(); // 自动导出表的链表
        private List<string> customTableList = new List<string>(); // 自定义导出表的链表
        private List<string> fileList = new List<string>(); // 需要的文件
        DataTable configTable = null; // 导出配置表
        Lua exportLua; // lua虚拟机
        Lua postExportLua; // lua虚拟机


        // -------------------------  重要 --------------------------------
        // -------------------------  重要 --------------------------------
        // -------------------------  重要 --------------------------------
        // -------------------------  重要 --------------------------------

        // 代码已经移到 Sword3\Source\Tools\AutoExport\src 不要在此修改了。

        // -------------------------  重要 --------------------------------
        // -------------------------  重要 --------------------------------
        // -------------------------  重要 --------------------------------
        // -------------------------  重要 --------------------------------


        /// <summary>
        /// 构造函数
        /// </summary>
        public ExportManager()
        {
            Init();
        }        

        /// <summary>
        /// 初始化数据
        /// </summary>
        private void Init()
        {
            try
            {
                // 初始化sql连接
                string fileName = Path.Combine(Application.StartupPath, "AutoExport.ini");
                string content = FileFolderHelper.FileToString(fileName);

                IniStructure m_inis = new IniStructure();
                m_inis = IniStructure.ReadIniWithContent(content);
                string connectString = m_inis.GetValue("General", "ConnString");
                conn = new SqlConnection(connectString);

                // 读取根目录路径
                rootPath = m_inis.GetValue("General", "RootDir");

                // 读取导出表列表            
                string[] autoTableArray = m_inis.GetKeys("AutoExport");
                autoTableList.AddRange(autoTableArray);

                string[] customTableArray = m_inis.GetKeys("CustomExport");
                customTableList.AddRange(customTableArray);

                // 读取资源文件列表
                string[] fileArray = m_inis.GetKeys("Resource");
                fileList.AddRange(fileArray);

                // 读取自动导出表的配置信息
                string sqlString = string.Format("SELECT * FROM sys_export_table_cfg");
                configTable = GetDataTable(sqlString);

                // 更新资源文件
                DownLoadResource();

                // 初始化lua虚拟机
                exportLua = new Lua();
                exportLua["Conn"] = conn;
                exportLua["RootDir"] = rootPath;
                string luaFile = Path.Combine(Application.StartupPath, "export.lua");
                exportLua.DoFile(luaFile);

                postExportLua = new Lua();
                postExportLua["Conn"] = conn;
                postExportLua["RootDir"] = rootPath;
                postExportLua["___GIsServer"] = true;
                luaFile = Path.Combine(Application.StartupPath, "post_export.lua");
                postExportLua.DoFile(luaFile);
            }
            catch (Exception ex)
            {
                logText.Append(string.Format("{0} —— 产生异常：{1}\r\n", DateTime.Now, ex.Message));
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

            try
            {
                if(conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                SqlDataAdapter adp = new SqlDataAdapter(sqlString, conn);
                adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
                DataSet ds = new DataSet();
                adp.Fill(ds);
                tb = ds.Tables[0];
            }
            catch (SqlException ex)
            {
                
            }
            finally
            {
                if(conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }            

            return tb;
        }

        /// <summary>
        /// 比较并下载资源文件
        /// </summary>
        private void DownLoadResource()
        {
            foreach(string s in fileList)
            {                
                FtpWebRequest ftp = FtpWebRequest.Create(new Uri(s)) as FtpWebRequest;
                ftp.UseBinary = true;
                ftp.KeepAlive = false;
                string localFileName = s.Replace("ftp://changan/newtools/", "");
                localFileName = Path.Combine(Application.StartupPath, localFileName);

                FileInfo fi = new FileInfo(localFileName);

                if(fi.Exists) // 本地文件已经存在
                {
                    ftp.Method = WebRequestMethods.Ftp.GetDateTimestamp;
                    FtpWebResponse response = ftp.GetResponse() as FtpWebResponse;
                    DateTime newEditTime = response.LastModified;
                    response.Close();

                    DateTime localEditTime = fi.LastWriteTime;

                    if(newEditTime > localEditTime) // 需要更新文件
                    {
                        fi.Delete();

                        ftp.Method = WebRequestMethods.Ftp.DownloadFile;
                        response = (FtpWebResponse)ftp.GetResponse();
                        Stream ftpStream = response.GetResponseStream();
                        long contentLength = response.ContentLength;
                        int bufferSize = 2048;
                        int readCount;
                        byte[] buffer = new byte[bufferSize];
                        readCount = ftpStream.Read(buffer, 0, bufferSize);

                        FileStream fileStream = new FileStream(localFileName, FileMode.Create);

                        while (readCount > 0)
                        {
                            fileStream.Write(buffer, 0, readCount);
                            readCount = ftpStream.Read(buffer, 0, bufferSize);
                        }

                        ftpStream.Close();
                        fileStream.Close();
                        response.Close();
                    }
                }
                else
                {
                    ftp.Method = WebRequestMethods.Ftp.DownloadFile;
                    FtpWebResponse response = (FtpWebResponse)ftp.GetResponse();
                    Stream ftpStream = response.GetResponseStream();
                    long contentLength = response.ContentLength;
                    int bufferSize = 2048;
                    int readCount;
                    byte[] buffer = new byte[bufferSize];
                    readCount = ftpStream.Read(buffer, 0, bufferSize);

                    string folderName = Path.GetDirectoryName(localFileName); // 获取目录
                    if (!Directory.Exists(folderName)) // 目录不存在
                    {
                        Directory.CreateDirectory(folderName); // 先创建目录，再创建文件
                    }  
                    FileStream fileStream = new FileStream(localFileName, FileMode.Create);
                    while(readCount > 0)
                    {
                        fileStream.Write(buffer, 0, readCount);
                        readCount = ftpStream.Read(buffer, 0, bufferSize);
                    }
                    
                    ftpStream.Close();
                    fileStream.Close();
                    response.Close();
                }
            }
        }

        /// <summary>
        /// 保存文件
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
                    // sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("GB18030"));
                }
                else
                {
                    // sw = new StreamWriter(fileName, false, Encoding.GetEncoding("gb2312"));
                    sw = new StreamWriter(fileName, false, Encoding.GetEncoding("GB18030"));
                }

                sw.Write(content);
                sw.Close();
            }
            catch (IOException ex)
            {
                logText.Append(string.Format("{0} —— 对文件{1}进行IO操作时产生异常：" + ex.Message + "\r\n", DateTime.Now, fileName));
            }
        }

        /// <summary>
        /// 将数据表转换为字符串内容
        /// </summary>
        /// <param name="tableName">数据表名</param>
        /// <returns>字符串内容</returns>
        private string GenerateContentString(string tableName)
        {
            string sqlString = string.Format("SELECT * FROM {0}", tableName);
            DataTable table = GetDataTable(sqlString);
            StringBuilder content = new StringBuilder();
            StringBuilder line = new StringBuilder();

            // 读取自定义的导出顺序和需要排除的字段
            string customOrderString = "";
            List<string> excludeFieldList = new List<string>();

            DataRow[] dataRows = configTable.Select(string.Format("tablename = '{0}'", tableName));
            if (dataRows.Length > 0)
            {
                customOrderString = dataRows[0]["export_order"].ToString();
                string excludeFieldString = dataRows[0]["export_ignore"].ToString();

                if (excludeFieldString != "")
                {
                    string[] excludeFieldArray = excludeFieldString.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                    excludeFieldList.AddRange(excludeFieldArray);
                }
            }

            if (table.PrimaryKey.Length > 0) // 有主键，需要依主键排序
            {
                string orderString = "";
                foreach (DataColumn c in table.PrimaryKey)
                {
                    orderString += string.Format("{0}, ", c.ColumnName);
                }
                orderString = orderString.TrimEnd(new char[] { ',', ' ' });

                // 自定义的导出顺序优先
                if (customOrderString != "")
                {
                    orderString = customOrderString;
                }

                sqlString = string.Format("SELECT * FROM {0} ORDER BY {1}", tableName, orderString);
                table = GetDataTable(sqlString);
            }

            foreach (DataColumn c in table.Columns)
            {
                if (!excludeFieldList.Contains(c.ColumnName))
                {
                    line.Append(string.Format("{0}\t", c.ColumnName));
                }
            }

            line.Remove(line.Length - 1, 1);
            line.Append("\r\n");
            content.Append(line);

            foreach (DataRow row in table.Rows)
            {
                line = new StringBuilder();

                foreach (DataColumn column in table.Columns)
                {
                    if (!excludeFieldList.Contains(column.ColumnName))
                    {
                        object fieldValue = row[column.ColumnName];

                        if (fieldValue is Boolean) // 布尔值转换为0,1
                        {
                            if ((bool)fieldValue)
                            {
                                line.Append("1\t");
                            }
                            else
                            {
                                line.Append("0\t");
                            }
                        }
                        else
                        {
                            line.Append(string.Format("{0}\t", fieldValue.ToString()));
                        }
                    }
                }

                line.Remove(line.Length - 1, 1);
                line.Append("\r\n");
                content.Append(line);
            }

            return content.ToString();
        }

        /// <summary>
        /// 导出自动导出表
        /// </summary>
        /// <param name="tableName">表名</param>
        private void ExportAutoTable(string tableName)
        {
            logText.Append(string.Format("{0} —— 准备导出数据表{1}...\r\n", DateTime.Now, tableName));

            string content = GenerateContentString(tableName);

            DataRow[] dataRows = configTable.Select(string.Format("tablename = '{0}'", tableName));
            if (dataRows.Length > 0)
            {
                string path = dataRows[0]["filename"].ToString().TrimStart(new char[] { '\\' });

                if (path != "")
                {
                    string fullPath = Path.Combine(rootPath, path);
                    SaveData(fullPath, content);
                    logText.Append(string.Format("{0} —— 数据表{1}导出成功！\r\n", DateTime.Now, tableName));
                }
                else
                {
                    logText.Append(string.Format("{0} —— 数据表{1}导出失败！\r\n", DateTime.Now, tableName));
                }  
            }                                 
        }

        /// <summary>
        /// 导出自定义数据表
        /// </summary>
        /// <param name="tableName">数据表名</param>
        private void ExportCustomTable(string tableName)
        {
            logText.Append(string.Format("{0} —— 准备导出数据表{1}...\r\n", DateTime.Now, tableName));           

            string content = GenerateContentString(tableName);
            string[] lines = content.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
            string[] headLineArray = lines[0].TrimEnd(new char[] { '\r' }).Split(new char[] { '\t' }, StringSplitOptions.RemoveEmptyEntries);
            ArrayList fields = new ArrayList();
            fields.AddRange(headLineArray);
            
            string[] values = new string[lines.Length - 1];
            for (int i = 1; i < lines.Length; i++)
            {
                values[i - 1] = lines[i].TrimEnd(new char[] { '\r' });
            }

            ImportInfoHelper helper = new ImportInfoHelper();
            helper.Fields = fields;
            helper.Values = values;

            bool exportSuccess = true;

            try
            {
                LuaFunction function = exportLua.GetFunction("onexport");
                object[] results = function.Call("", tableName, rootPath, helper);
                if (results != null && results.Length > 0 && !(bool)results[0])
                {
                    ExportAutoTable(tableName);
                }
            }
            catch (Exception ex)
            {
                logText.Append(string.Format("{0} —— 数据表{1}导出失败：{2}\r\n", DateTime.Now, tableName, ex.Message));
                exportSuccess = false;
            }
            
            if (exportSuccess)
            {
                logText.Append(string.Format("{0} —— 数据表{1}导出成功！\r\n", DateTime.Now, tableName));
            }            
        }

        /// <summary>
        /// 导出自定义数据表
        /// </summary>
        /// <param name="tableName">数据表名</param>
        private void ExportCustomPostTable(string tableName)
        {
            logText.Append(string.Format("{0} —— 准备处理数据表{1}的数据...\r\n", DateTime.Now, tableName));

            DataRow[] rows = configTable.Select(string.Format("tablename = '{0}'", tableName));

            if (rows.Length > 0)
            {
                string path = rows[0]["filename"].ToString().TrimStart(new char[] { '\\' });
                string fullPath = Path.Combine(rootPath, path);
                bool exportSuccess = true;

                try
                {
                    LuaFunction function = postExportLua.GetFunction("onpostexport");

                    object[] results = function.Call(tableName, fullPath);
                    if (results != null && results.Length > 0 && !(bool)results[0])
                    {
                        // ExportAutoTable(tableName);
                    }
                }
                catch (System.Exception ex)
                {
                    logText.Append(string.Format("{0} —— 数据表{1}数据处理失败：{2}\r\n", DateTime.Now, tableName, ex.Message));
                    exportSuccess = false;
                }
                
                if (exportSuccess)
                {
                    logText.Append(string.Format("{0} —— 数据表{1}的数据处理成功！\r\n", DateTime.Now, tableName));
                }                
            }
            else
            {
                logText.Append(string.Format("{0} —— 数据表{1}的数据处理失败！\r\n", DateTime.Now, tableName));
            }
        }

        /// <summary>
        /// 导出所有的数据表
        /// </summary>
        public void Export()
        {
            logText.Append(string.Format("{0} —— 准备导出数据表...\r\n", DateTime.Now));
               
            // 导出自动数据表
            foreach(string s in autoTableList)
            {
                ExportAutoTable(s);
            }            

            // 导出自定义数据表
            foreach(string s in customTableList)
            {
                ExportCustomTable(s);
                ExportCustomPostTable(s);
            }                      

            logText.Append(string.Format("{0} —— 数据表导出完成！", DateTime.Now));

            // 导出日志文件
            string logPath = Path.Combine(Application.StartupPath, "ExportTableLog.txt");
            SaveData(logPath, logText.ToString());
            string[] consoleLines = logText.ToString().Split(new char[] { '\n' });
            foreach(string s in consoleLines)
            {
                Console.WriteLine(s);
            }            
            logText = new StringBuilder();
        }

        /// <summary>
        /// 导出辅助类
        /// </summary>
        class ImportInfoHelper
        {
            public ArrayList Fields;
            public string[] Values;
        }
    }
}
