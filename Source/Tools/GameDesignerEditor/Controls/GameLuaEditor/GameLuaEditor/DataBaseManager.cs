using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Data;
using System.Collections;
using System.Text;
using System.IO;
using LuaInterface;

namespace GameLuaEditor
{
    public class DataBaseManager
    {
        private static DataBaseManager manager = null; // DataBaseManager的实例
        private DataTable lsTable = null; // ls数据表
        private Hashtable lsHashtable = null; // ls hash表
        public SqlConnection conn; // 数据库链接
        private Lua m_lua;

        /// <summary>
        /// 私有构造函数，不允许在外部初始化
        /// </summary>
        private DataBaseManager()
        {
            InitLua();
        }

        private void InitLua()
        {
            m_lua = new Lua();
            m_lua.RegisterFunction("GetDataTableRow", this, typeof(DataBaseManager).GetMethod("GetDataTableRow"));
        }

        public bool GetDataTableRow(string tablename, LuaTable keys, ref LuaTable ltReturn)
        {
            string sql = string.Format("SELECT * FROM {0}", tablename);
            DataTable tbl = GetDataTable(sql);
            if (tbl.PrimaryKey.Length != keys.Keys.Count)
                MessageBox.Show(string.Format("{0} —— 函数GetDataTableRow断言：{1}\r\n", DateTime.Now, "输入参数keys长度错误。"));

            object[] KEYS = new object[tbl.PrimaryKey.Length];
            for (int i = 0; i < KEYS.Length; i++)
            {
                KEYS[i] = ((LuaTable)keys)[i];
            }
            DataRow row = tbl.Rows.Find(KEYS);
            if (row == null)
                return false;

            foreach (DataColumn col in tbl.Columns)
            {
                ltReturn[col.ColumnName] = (row[col.ColumnName] == DBNull.Value ? null : row[col.ColumnName]);
            }
            return true;
        }


        /// <summary>
        /// 获取DataBaseManager的实例
        /// </summary>
        /// <returns>DataBaseManager的实例</returns>
        public static DataBaseManager GetDataBaseManager()
        {
            if (manager == null) // 对象未初始化
            {
                manager = new DataBaseManager();
            }

            return manager;
        }

        /// <summary>
        /// sql连接属性
        /// </summary>
        public SqlConnection Connection
        {
            get
            {
                return conn;
            }
        }

        /// <summary>
        /// 初始化DataBaseManager对象
        /// </summary>
        /// <param name="connectString">连接字符串</param>
        public void Init(string connectString)
        {
            InitConnection(connectString);
            InitLsTable();
            InitLsHashTable();
        }

        /// <summary>
        /// 获取用户名
        /// </summary>
        /// <returns>用户名</returns>
        public string GetHostName()
        {
            return System.Net.Dns.GetHostName();
        }

        /// <summary>
        /// 获取数据库名
        /// </summary>
        /// <returns>数据库名</returns>
        public string GetDataBaseName()
        {
            string dataBaseName = "连接未初始化";

            if(conn != null)
            {
                dataBaseName = conn.Database.ToString();
            }

            return dataBaseName; 
        }

        /// <summary>
        /// 获取脚本信息数据表
        /// </summary>
        /// <returns>脚本信息数据表</returns>
        public DataTable GetScriptInformation()
        {
            DataTable scriptTable = null;

            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT {0}, {1} FROM {2}", "id", "path",
                    "sys_script_script");
                scriptTable = GetDataTable(sqlString);
                if (scriptTable.Rows.Count < 0) // 不存在脚本记录
                {
                    scriptTable = null;
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在获取脚本信息数据表时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return scriptTable;
        }

        /// <summary>
        /// 获取ls信息数据表
        /// </summary>
        /// <returns>ls信息数据表</returns>
        public DataTable GetLSInformation()
        {
            DataTable lsTable = null;

            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT * FROM {0}", "sys_script_lsfile");
                lsTable = GetDataTable(sqlString);

                if(lsTable.Rows.Count < 0) // 不存在ls记录
                {
                    lsTable = null;
                }
            }
            catch(SqlException ex)
            {
                MessageBox.Show("在获取ls数据表时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return lsTable;
        }

        /// <summary>
        /// 获取ls的分类信息
        /// </summary>
        /// <returns>ls分类链表</returns>
        public List<string> GetLSClassification()
        {
            List<string> classificationList = new List<string>();

            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT {0} FROM {1} GROUP BY {2}",
                    "classification", "sys_script_lsfile", "classification");
                DataTable lsTable = GetDataTable(sqlString);
                foreach(DataRow r in lsTable.Rows)
                {
                    classificationList.Add(r[0].ToString());
                }
            }
            catch(SqlException ex)
            {
                MessageBox.Show("在获取ls分类信息时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return classificationList;
        }

        /// <summary>
        /// 创建脚本
        /// </summary>
        /// <param name="path">脚本路径</param>
        /// <param name="content">脚本内容</param>
        /// <param name="coverScript">是否覆盖原有脚本</param>
        /// <returns>脚本ID</returns>
        public string CreateScriptData(string path, string content, bool coverScript)
        {
            string id = null;
            SqlTransaction transaction = null;

            string newContent = EncodeScriptContent(path, content);
            byte[] data = EncodeScriptData(newContent);

            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                transaction = conn.BeginTransaction();
                SqlCommand cmd = conn.CreateCommand();
                cmd.Transaction = transaction;

                string sqlString = string.Format("SELECT id FROM sys_script_script WHERE path = '{0}'", path);
                cmd.CommandText = sqlString;
                object executeResult = cmd.ExecuteScalar();

                SqlParameter sqlParameter = new SqlParameter("@data", data);
                cmd.Parameters.Add(sqlParameter);

                if (executeResult != null)
                {
                    if (coverScript)
                    {
                        id = executeResult.ToString();
                        sqlString = string.Format("UPDATE sys_script_script SET data = @data, edittime = '{0}', host = '{1}' WHERE id = '{2}'",
                                                  DateTime.Now, GetHostName(), id);
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();
                    }                    
                }
                else
                {
                    sqlString = string.Format("INSERT INTO sys_script_script (path, data, host, edittime) VALUES ('{0}', @data, '{1}', '{2}')",
                                              path, GetHostName(), DateTime.Now);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("SELECT id FROM sys_script_script WHERE path = '{0}'", path);
                    cmd.CommandText = sqlString;
                    executeResult = cmd.ExecuteScalar();

                    if (executeResult != null)
                    {
                        id = executeResult.ToString();
                    }
                }
                
                transaction.Commit();
            }
            catch (SqlException ex)
            {
                MessageBox.Show(string.Format("在创建脚本时产生sql异常：{0}", ex.Message), "创建脚本",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);

                if (transaction != null)
                {
                    transaction.Rollback();
                }
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            return id;
        }

        /// <summary>
        /// 创建脚本
        /// </summary>
        /// <param name="path">脚本</param> 
        /// <returns>脚本ID</returns>
        public string CreateScriptData(string path)
        {
            string id = null; // 脚本的id
            SqlTransaction transaction = null; // 事务

            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                transaction = conn.BeginTransaction();
                SqlCommand cmd = conn.CreateCommand();
                string sqlString = string.Format("INSERT INTO {0} ({1}, {2}, {3}) VALUES ('{4}', '{5}', '{6}')",
                                                 "sys_script_script", "path", "host", "edittime", path, GetHostName(), DateTime.Now);
                cmd.CommandText = sqlString;
                cmd.Transaction = transaction;
                int iret = cmd.ExecuteNonQuery();

                sqlString = string.Format("SELECT id FROM sys_script_script WHERE path = '{0}'", path);
                cmd.CommandText = sqlString;
                id = cmd.ExecuteScalar().ToString();
                transaction.Commit(); // 提交事务
            }
            catch (Exception ex)
            {
                MessageBox.Show("在创建新的脚本时产生异常: " + ex.ToString());

                if (transaction != null) // 回滚事务
                {
                    transaction.Rollback();
                }
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接诶
                {
                    conn.Close();
                }
            }

            return id;
        }

        /// <summary>
        /// 从本地文件中读取脚本
        /// </summary>
        /// <param name="fileName">文件路径</param>
        /// <returns>脚本内容</returns>
        public string GetScriptDataFromFile(string fileName)
        {
            string content = null;

            if (File.Exists(fileName))
            {
                StreamReader reader = new StreamReader(fileName, Encoding.GetEncoding("gb2312"));
                content = reader.ReadToEnd();
                reader.Close();
            }

            return content;
        }

        /// <summary>
        /// 获取脚本的原始字符串
        /// </summary>
        /// <param name="id">脚本ID</param>
        /// <returns>脚本原始字符串</returns>
        public string GetScriptMetaData(string id)
        {
            string content = null;

            if (conn.State == ConnectionState.Closed)
            {
                conn.Open();
            }

            string sqlString = string.Format("SELECT data, path FROM sys_script_script WHERE id = '{0}'", id);
            DataTable scriptTable = GetDataTable(sqlString);

            if (scriptTable.Rows.Count > 0)
            {                
                if (!(scriptTable.Rows[0]["data"] is DBNull))
                {
                    content = DecodeScriptData(((byte[])scriptTable.Rows[0]["data"]));
                }
            }

            if (conn.State == ConnectionState.Open)
            {
                conn.Close();
            }

            return content;
        }

        /// <summary>
        /// 获取脚本内容字符串
        /// </summary>
        /// <param name="id">脚本id</param>
        /// <returns>脚本内容字符串</returns>
        public string GetScriptData(string id)
        {
            string content = null; // 脚本内容字符串
            string path = null;

            if (conn.State == ConnectionState.Closed) // 打开sql连接
            {
                conn.Open();
            }

            string sqlString = string.Format("SELECT data, path FROM sys_script_script WHERE id = '{0}'", id);
            DataTable scriptTable = GetDataTable(sqlString);
            
            if (scriptTable.Rows.Count > 0) // 存在对应的脚本记录
            {
                content = scriptTable.Rows[0]["data"].ToString();                    
                
                if (content != "")
                {
                    content = DecodeScriptData(((byte[])scriptTable.Rows[0]["data"]));
                    path = scriptTable.Rows[0]["path"].ToString(); 
                    content = DecodeScriptContent(path, content);
                }
            }

            if (conn.State == ConnectionState.Open) // 关闭sql连接
            {
                conn.Close();
            }
         
            content = GetBeforeOpen(content, path);

            return content;
        }

        /// <summary>
        /// 获取脚本的历史记录信息
        /// </summary>
        /// <param name="label">脚本的历史记录标记</param>
        /// <returns>脚本的内容</returns>
        public string GetScriptHistoryData(string label)
        {
            string historyData = null; // 脚本的内容

            if(conn.State == ConnectionState.Closed) // 打开sql连接
            {
                conn.Open();
            }

            string sqlString = string.Format("SELECT data, path FROM sys_script_log WHERE label = '{0}'", label);
            DataTable logTable = GetDataTable(sqlString);

            if (logTable.Rows.Count > 0) // 存在对应的脚本记录
            {
                historyData = logTable.Rows[0]["data"].ToString();

                if (historyData != "")
                {
                    historyData = DecodeScriptData(((byte[])logTable.Rows[0]["data"]));
                    string path = logTable.Rows[0]["path"].ToString();
                    historyData = DecodeScriptContent(path, historyData);
                }
            }

            if(conn.State == ConnectionState.Open) // 关闭sql连接
            {
                conn.Close();
            }

            return historyData;
        }

        /// <summary>
        /// 获取脚本分类
        /// </summary>
        /// <param name="path">脚本路径</param>
        /// <returns>脚本分类</returns>
        private string GetClassification(string path)
        {            
            string[] as_Path = path.Split(new string[] { "\\" }, StringSplitOptions.RemoveEmptyEntries);
            string classification = "未分类";

            if (as_Path.Length >= 3 && as_Path[0] == "scripts" && as_Path[1] != "Map")
            {
                classification = as_Path[1];
            }
            else if (as_Path.Length < 3 || as_Path[0] != "scripts" || as_Path[1] != "Map")
            {
                ;
            }
            else
            {
                classification = as_Path[2];
            }

            return classification;
        }

        /// <summary>
        /// 当前脚本的是否属于场景分类
        /// </summary>
        /// <param name="scriptPath">脚本路径</param>
        /// <returns>是否属于场景分类</returns>
        private bool IsScriptInMap(string scriptPath)
        {
            bool scriptInMap = scriptPath.StartsWith(@"scripts\Map");

            return scriptInMap;
        }    
    
        public List<string> GetFileNameList(List<string> idList)
        {
            List<string> fileNameList = new List<string>();

            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                string sqlString;                

                foreach (string id in idList)
                {
                    sqlString = string.Format("SELECT id, path FROM {0} WHERE id = {1}", "sys_script_lsfile", id);
                    DataTable infoTable = GetDataTable(sqlString);

                    if (infoTable.Rows.Count > 0)
                    {
                        string scriptPath = infoTable.Rows[0]["path"].ToString();
                        fileNameList.Add(infoTable.Rows[0]["path"].ToString());
                    }


                }                                
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在读取脚本数据时产生sql异常：" + ex.Message, "读取脚本数据",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            return fileNameList;
        }

        /// <summary>
        /// 输出脚本和ls文件
        /// </summary>
        /// <param name="idList">脚本id链表</param>
        /// <returns>脚本和ls文件hash表</returns>
        public Hashtable OutputScriptData(List<string> idList, bool exportCondenseString)
        {
            Hashtable fileTable = null;

            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                if (idList.Count > 0) // 导出文件数大于零
                {
                    fileTable = new Hashtable(); // 文件
                    List<string> classificationList = new List<string>(); // 分类链表
                    string sqlString = string.Format("SELECT * FROM {0}", "sys_script_lsfile");
                    DataTable lsTable = GetDataTable(sqlString);

                    // 导出文件
                    foreach (string id in idList) 
                    {
                        sqlString = string.Format("SELECT * FROM {0} WHERE id = {1}",
                                                  "sys_script_script", id);
                        DataTable scriptTable = GetDataTable(sqlString);

                        if (scriptTable.Rows.Count > 0)
                        {
                            string path = scriptTable.Rows[0]["path"].ToString();

                            if (scriptTable.Rows[0]["data"] is DBNull)
                            {
                                fileTable[path] = "";
                                continue;
                            }

                            string data = DecodeScriptData(((byte[])scriptTable.Rows[0]["data"]));
                            string classification = GetClassification(scriptTable.Rows[0]["path"].ToString());

                            // 导出ls文件
                            DataRow[] lsRows = lsTable.Select(string.Format("classification = '{0}'", classification));

                            if (lsRows.Length > 0 && !classificationList.Contains(classification))
                            {
                                string tableName = "SCENE_STRING_TABLE";

                                if (!path.StartsWith(@"scripts\Map"))
                                {
                                    tableName = string.Format("{0}_STRING_TABLE", classification.ToUpper());
                                    fileTable[string.Format(@"scripts\{0}\include\strings.ls", classification)] = CodeConvert.ConvertLsFileContent(lsRows, tableName);
                                }
                                else
                                {
                                    fileTable[string.Format(@"scripts\Map\{0}\include\scenestrings.ls", classification)] = CodeConvert.ConvertLsFileContent(lsRows, tableName);
                                }

                                classificationList.Add(classification);
                            }
                            
                            fileTable[path] = data;
                        }
                    }
                }             
            }
            catch(SqlException ex)
            {
                MessageBox.Show("在导出脚本文件时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return fileTable;
        }

        /// <summary>
        /// 保存ls文件字符串到数据库
        /// </summary>
        /// <param name="content">ls文件字符串内容</param>
        /// <param name="classification">ls文件分类</param>
        /// <returns>id号</returns>
        private string SaveLsFileData(string content, string classification)
        {
            string id = null; // 返回的id
            SqlTransaction transaction = null; // 事务  

            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                content = content.Replace("'", "''"); // 存入数据库前进行替换
                transaction = conn.BeginTransaction(); // 开启事务
                SqlCommand cmd = conn.CreateCommand();
                string sqlString = string.Format("INSERT INTO {0} ({1}, {2}) VALUES (\'{3}\', \'{4}\')",
                    "sys_script_lsfile", "content", "classification", content, classification);
                cmd.CommandText = sqlString;
                cmd.Transaction = transaction;
                cmd.ExecuteNonQuery();

                sqlString = string.Format("SELECT MAX({0}) FROM {1}", "id", "sys_script_lsfile");
                cmd.CommandText = sqlString;
                id = cmd.ExecuteScalar().ToString();
                transaction.Commit(); // 提交事务
            }
            catch (Exception ex)
            {
                MessageBox.Show("在保存ls文件信息时产生异常: " + ex.ToString());
                if (transaction != null) // 回滚事务
                {
                    transaction.Rollback();
                }
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return id;
        }

        /// <summary>
        /// 清空当前用户的所有锁
        /// </summary>
        public void ClearRecordLocks()
        {
            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                SqlCommand cmd = conn.CreateCommand();
                string sqlString = "DELETE FROM sys_lock_record WHERE lockhostname='" + System.Net.Dns.GetHostName() +"'";
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                string err = string.Format("在清除行锁时发生错误。\r\n\r\n错误信息：{0}", ex.Message);
                MessageBox.Show(err);
            }
            catch (Exception ex)
            {
                MessageBox.Show("在处理锁时产生异常: " + ex.ToString());
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }
        }

        public string GetBeforeOpen(string code, string path)
        {
            string content = code;
            string strFileName = Path.Combine(Application.StartupPath, @"Plugins\LuaCheck\filter.lua");

            if (File.Exists(strFileName))
            {
                try
                {
                    // 重置环境变量
                    Environment.CurrentDirectory = Application.StartupPath;
                    m_lua.DoFile(strFileName);
                    LuaFunction fun = m_lua.GetFunction("beforeOpen");

                    if (fun != null)
                    {
                        object[] ret = fun.Call(content, path);

                        if (ret.Length == 1 && ret[0] != null)
                        {
                            content = ret[0].ToString();
                        }
                        else
                        {
                            Helper.RecordLog("beforeOpen的返回值不正确", Helper.LogLevel.Warning);                            
                        }
                    }
                }
                catch (System.Exception ex)
                {
                    Helper.RecordLog(ex.ToString(), Helper.LogLevel.Warning);
                    InitLua();
                }
            }

            return content;
        }

        public string GetBeforeSave(string code, string path)
        {
            string content = code;
            string strFileName = Path.Combine(Application.StartupPath, @"Plugins\LuaCheck\filter.lua");

            if (File.Exists(strFileName))
            {
                try
                {
                    // 重置环境变量
                    Environment.CurrentDirectory = Application.StartupPath;
                    m_lua.DoFile(strFileName);
                    LuaFunction fun = m_lua.GetFunction("beforeSave");

                    if (fun != null)
                    {
                        object[] ret = fun.Call(content, path);

                        if (ret.Length == 1 && ret[0] != null)
                        {
                            content = ret[0].ToString();
                        }
                        else
                        {
                            Helper.RecordLog("beforeSave的返回值不正确", Helper.LogLevel.Warning);
                        }
                    }
                }
                catch (System.Exception ex)
                {
                    Helper.RecordLog(ex.ToString(), Helper.LogLevel.Warning);
                }
            }
            return content;
        }

        /// <summary>
        /// 转换脚本内容
        /// </summary>
        /// <param name="scriptPath">脚本路径</param>
        /// <param name="scriptContent">脚本内容</param>
        /// <returns>转换后的脚本内容</returns>
        private string DecodeScriptContent(string scriptPath, string scriptContent)
        {
            string classification = GetClassification(scriptPath);
            InitLsTable(classification);
            InitLsHashTable();

            string tableName = "SCENE_STRING_TABLE";

            if (!IsScriptInMap(scriptPath) && !scriptContent.Contains("SCENE_STRING_TABLE")) // 兼容旧格式
            {
                tableName = string.Format("{0}_STRING_TABLE", classification.ToUpper());
            }

            string newContent = CodeConvert.Decode(scriptContent, lsHashtable, tableName);

            return newContent;
        }

        /// <summary>
        /// 转换脚本内容
        /// </summary>
        /// <param name="scriptPath">脚本路径</param>
        /// <param name="scriptContent">脚本内容</param>        
        /// <returns>转换后的脚本内容</returns>
        private string EncodeScriptContent(string scriptPath, string scriptContent)
        {
            Hashtable contentHashtable = new Hashtable(); // 记录脚本内容和对应id的hash表
            string classification = GetClassification(scriptPath);
            InitLsTable(classification);
            InitLsHashTable();

            foreach (object o in lsHashtable.Keys)
            {
                contentHashtable[lsHashtable[o]] = o;
            }

            // 将脚本中的中文字符串取出，与ls文件对比，并更新ls文件
            List<string> varList = CodeConvert.GetStringList(scriptContent);
            List<string> contentList = new List<string>();

            foreach (DataRow dataRow in lsTable.Rows)
            {
                contentList.Add(dataRow["content"].ToString());
            }

            foreach (string s in varList)
            {
                if (!contentList.Contains(s))
                {
                    contentHashtable[s] = SaveLsFileData(s, classification);
                }
            }

            string tableName = "SCENE_STRING_TABLE";

            if (!IsScriptInMap(scriptPath))
            {
                tableName = string.Format("{0}_STRING_TABLE", classification.ToUpper());
            }

            string newContent = CodeConvert.Encode(scriptContent, contentHashtable, tableName);

            return newContent;
        }

        /// <summary>
        /// 编码脚本内容
        /// </summary>
        /// <param name="content">脚本内容</param>
        /// <returns>编码内容</returns>
        private byte[] EncodeScriptData(string content)
        {
            byte[] data = Encoding.UTF8.GetBytes(content);

            return data;
        }
        
        /// <summary>
        /// 解码脚本内容
        /// </summary>
        /// <param name="data">编码内容</param>
        /// <returns>脚本内容</returns>
        private string DecodeScriptData(byte[] data)
        {
            string content = Encoding.UTF8.GetString(data);

            return content;
        }

        /// <summary>
        /// 保存脚本内容
        /// </summary>
        /// <param name="id">脚本id</param>
        /// <param name="content">脚本内容</param>
        /// <returns>是否保存成功</returns>
        public bool SaveScriptData(string path, string content, out string viewText)
        {
            bool result = true;

            content = GetBeforeSave(content, path);
            viewText = content;
            string newContent = EncodeScriptContent(path, content);

            if (conn.State == ConnectionState.Closed) // 打开sql连接
            {
                conn.Open();
            }
                        
            byte[] data = EncodeScriptData(newContent);
            string sqlString = string.Format("UPDATE sys_script_script SET data = @data, edittime = '{0}', host = '{1}' WHERE path = '{2}'",
                                             DateTime.Now, System.Net.Dns.GetHostName(), path);
            SqlCommand cmd = new SqlCommand(sqlString, conn);
            SqlParameter spcode = new SqlParameter("@data", data);
            cmd.Parameters.Add(spcode);
            int iret = cmd.ExecuteNonQuery();

            if (iret <= 0)
            {
                result = false;
            }
        
            if (conn.State == ConnectionState.Open) // 关闭sql连接
            {
                conn.Close();
            }

            return result;
        }

        /// <summary>
        /// 删除对应id的ls字符串
        /// </summary>
        /// <param name="id">ls字符串的id</param>
        /// <returns>执行结果</returns>
        public bool DeleteLsData(string id)
        {
            bool result = true; // 执行结果

            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("DELETE FROM {0} WHERE {1} = \'{2}\'", "sys_script_lsfile", "id", id);
                SqlCommand cmd = new SqlCommand(sqlString, conn);
                result = (cmd.ExecuteNonQuery() > 0);
            }
            catch(SqlException ex)
            {
                MessageBox.Show(string.Format("在删除id为{0}的字符串时产生sql异常: ", id) + ex.ToString());
                result = false;
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return result;
        }

        /// <summary>
        /// 删除数据库中的脚本
        /// </summary>
        /// <param name="id">脚本的id</param>
        /// <returns>是否删除成功</returns>
        public bool DeleteScriptData(string id)
        {
            bool result = true;
            SqlTransaction transaction = null;

            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                transaction = conn.BeginTransaction();
                SqlCommand cmd = conn.CreateCommand();
                cmd.Transaction = transaction;
                string sqlString = string.Format("UPDATE {0} SET {1} = '{2}', {3} = '{4}' WHERE {5} = '{6}'", "sys_script_script",
                    "host", GetHostName(), "edittime", DateTime.Now, "id", id);
                cmd.CommandText = sqlString;

                //result = (result && (cmd.ExecuteNonQuery() == 2));
                // AffectedNum should be 2, but if not, it's not yet an error. so ...
                int AffectedNum = cmd.ExecuteNonQuery();

                sqlString = string.Format("DELETE FROM {0} WHERE {1} = {2}", "sys_script_script", "id", id);
                cmd.CommandText = sqlString;
                result = (result && (cmd.ExecuteNonQuery() == 1));
                transaction.Commit();
            }
            catch(SqlException ex)
            {
                MessageBox.Show(string.Format("在删除id为{0}的脚本文件时产生sql异常: ", id) + ex.ToString());
                transaction.Rollback();
                result = false;
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return result;
        }

        /// <summary>
        /// 锁定脚本记录
        /// </summary>
        /// <param name="id">脚本id</param>
        /// <returns>是否锁定成功</returns>
        public bool LockScript(string id)
        {
            object[] keys = new object[] { id }; // 主键
            bool result = true; // 返回值

            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                RecordLockEx recordLock = new RecordLockEx(conn, "sys_script_script", keys, 0);
                recordLock.UpdateLocalVersion(keys);
                if (!recordLock.Lock(keys))
                {
                    MessageBox.Show(string.Format("该脚本已被{0}锁定!", recordLock.GetServerLastLockUser(keys)));
                    result = false;
                }
            }
            catch(SqlException ex)
            {
                MessageBox.Show("在锁定脚本记录时产生sql异常: " + ex.ToString());
                result = false;
            }
            catch (Exception ex)
            {
                MessageBox.Show("在处理锁时产生异常: " + ex.ToString());
                result = false;
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return result;
        }

        /// <summary>
        /// 解锁脚本记录
        /// </summary>
        /// <param name="id">脚本id</param>
        /// <returns>是否解锁成功</returns>
        public bool UnLockScript(string id)
        {
            object[] keys = new object[] { id }; // 主键
            bool result = true; // 返回值

            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                RecordLockEx recordLock = new RecordLockEx(conn, "sys_script_script", keys, 0);
                recordLock.UpdateLocalVersion(keys);
                if (recordLock.UnLock(keys) < 0)
                {
                    result = false;
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在解锁脚本记录时产生sql异常: " + ex.ToString());
                result = false;
            }
            catch (Exception ex)
            {
                MessageBox.Show("在处理锁时产生异常: " + ex.ToString());
                result = false;
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }
            return result;
        }

        /// <summary>
        /// 检查对应id的脚本是否已经被锁
        /// </summary>
        /// <param name="id">脚本id</param>
        /// <returns>脚本是否已经被锁</returns>
        public bool IsScriptLocked(string id)
        {
            bool result = false;

            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT tablename FROM {0} WHERE {1} = '{2}' AND {3} = '{4},' AND lockhostname IS NOT NULL",
                    "sys_lock_record", "tablename", "sys_script_script", "fieldvalues", id);
                DataTable scriptTable = GetDataTable(sqlString);
                result = (scriptTable.Rows.Count > 0);
            }
            catch(SqlException ex)
            {
                MessageBox.Show(string.Format("检查id为{0}的脚本是否被锁时产生sql异常: ", id) + ex.ToString());
                result = false;
            }
            catch (Exception ex)
            {
                MessageBox.Show("在处理锁时产生异常: " + ex.ToString());
                result = false;
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return result;
        }

        /// <summary>
        /// 强制清除所有锁
        /// </summary>
        /// <returns>是否清除成功</returns>
        public bool ClearLockScript(string id)
        {
            object[] keys = new object[] { id }; // 主键

            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                RecordLockEx recordLock = new RecordLockEx(conn, "sys_script_script", keys, 0);
                if (!recordLock.ForceClearLock(keys))
                {
                    return false;
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在清除脚本记录时产生sql异常: " + ex.ToString());
            }
            catch (Exception ex)
            {
                MessageBox.Show("在处理锁时产生异常: " + ex.ToString());
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }
            return true;
        }

        /// <summary>
        /// 更新对应id的脚本的路径信息
        /// </summary>
        /// <param name="id">脚本的原路径</param>
        /// <param name ="path">脚本的新路径</param>
        /// <returns>是否更新成功</returns>
        public bool UpdateScriptPath(string oldPath, string newPath)
        {
            bool result = true; // 是否处理成功
            SqlTransaction transaction = null; // 事务

            try
            {
                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                transaction = conn.BeginTransaction();
                SqlCommand cmd = conn.CreateCommand();
                cmd.Transaction = transaction;

                string sqlString = string.Format("SELECT id, path FROM {0}", "sys_script_script");
                DataTable scriptTable = GetDataTable(sqlString, transaction);
                foreach(DataRow r in scriptTable.Rows)
                {
                    if(!(r["path"] is DBNull))
                    {
                        string tempPath = r["path"].ToString();
                        string id = r["id"].ToString();
                        if(tempPath.StartsWith(oldPath))
                        {
                            tempPath = tempPath.Remove(0, oldPath.Length);
                            tempPath = newPath + tempPath;
                            sqlString = string.Format("UPDATE {0} SET {1} = \'{2}\' WHERE {3} = \'{4}\'", 
                                "sys_script_script", "path", tempPath, "id", id);
                            cmd.CommandText = sqlString;
                            int iAffected = cmd.ExecuteNonQuery();
                            //result = (result && (iAffected == 1));
                        }
                    }
                }

                transaction.Commit();
            }
            catch(SqlException ex)
            {
                MessageBox.Show("更新脚本的路径信息时产生sql异常: " + ex.ToString());
                if(transaction != null) // 事务回滚
                {
                    transaction.Rollback();
                }
                result = false;
            }
            finally
            {
                if(conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return result;
        }

        /// <summary>
        /// 初始化ls文件hash表
        /// </summary>
        private void InitLsHashTable()
        {
            lsHashtable = new Hashtable();

            if (lsTable.Rows.Count > 0) // 存在字符串记录
            {
                foreach (DataRow dataRow in lsTable.Rows)
                {
                    lsHashtable[dataRow["id"].ToString()] = dataRow["content"].ToString();
                }
            }
        }

        /// <summary>
        /// 初始化sql连接
        /// </summary>
        /// <param name="connectString">连接字符串</param>
        private void InitConnection(string connectString)
        {
            try
            {
                // string connectString = "Server = jx3web; DataBase = s3design; Uid = s3design; Password = ddd12345678;";
                conn = new SqlConnection(connectString);
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在打开sql连接时产生异常: " + ex.ToString());
            }
        }

        /// <summary>
        /// 初始化ls数据表
        /// </summary>
        private void InitLsTable()
        {
            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT * FROM {0}", "sys_script_lsfile");
                lsTable = GetDataTable(sqlString);
                if (lsTable.Rows.Count < 0) // 不存在字符串记录
                {
                    lsTable = null;
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在获取ls文件内容时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }
        }

        /// <summary>
        /// 初始化ls数据表
        /// </summary>
        /// <param name="classification">分类</param>
        private void InitLsTable(string classification)
        {
            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("SELECT * FROM sys_script_lsfile WHERE classification = '{0}'", classification);
                lsTable = GetDataTable(sqlString);

                if (lsTable.Rows.Count < 0) // 不存在字符串记录
                {
                    lsTable = null;
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在获取ls文件内容时产生sql异常: " + ex.ToString());
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询语句</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sqlString)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sqlString, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            DataTable tb = ds.Tables[0];
            return tb;
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询语句</param>
        /// <param name="transaction">事务对象</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sqlString, SqlTransaction transaction)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sqlString, conn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            adp.SelectCommand.Transaction = transaction;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            DataTable tb = ds.Tables[0];
            return tb;
        }
    }
}