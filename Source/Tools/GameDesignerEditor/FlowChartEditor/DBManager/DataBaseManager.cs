using System;
using System.Collections.Generic;
using System.Data.SqlClient;
using System.Data;
using System.Collections;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace DBManager
{
    public class DataBaseManager
    {
        private static DataBaseManager dataBaseManager = null; // DataBaseManager的实例

        private const string diagramTable = "Diagram"; // 绘图数据表
        private const string diagramBackupTable = "DiagramBackup"; // 绘图备份数据表

        private const string designerToolsConfigFileName = "GameDesingerTools_Public.ini";
        private const string flowChartEditorConfigFileName = "Diagram_Public.ini";

        private SqlConnection conn; // 数据库链接
        private static string connectString = ""; // 连接数据库的字符串
        private SqlConnection conn_jx3web; // jx3web数据库链接        

        private int maxBackupCount = 20; // 绘图备份记录上限

        /// <summary>
        /// 私有构造函数，不允许在外部初始化
        /// </summary>
        private DataBaseManager() 
        {
 
        }

        /// <summary>
        /// 获取DataBaseManager的实例
        /// </summary>
        /// <returns>DataBaseManager的实例</returns>
        public static DataBaseManager GetDataBaseManager()
        {
            if (dataBaseManager == null) // 对象未初始化
            {
                dataBaseManager = new DataBaseManager();
                dataBaseManager.Init();

                string fileName = Path.Combine(Application.StartupPath, designerToolsConfigFileName);
                string connectString = GetConnectString(fileName, "General", "ConnString");

                if (!string.IsNullOrEmpty(connectString))
                {
                    dataBaseManager.conn_jx3web = new SqlConnection(connectString);
                }                                
            }

            return dataBaseManager;
        }

        /// <summary>
        /// 绘图数据表名称
        /// </summary>
        public string DiagramTableName
        {
            get
            {
                return diagramTable;
            }
        }

        /// <summary>
        /// sql连接
        /// </summary>
        public SqlConnection Connection
        {
            get 
            { 
                return conn; 
            }
        }

        /// <summary>
        /// sql连接
        /// </summary>
        public SqlConnection Connection_Jx3web
        {
            get 
            { 
                return conn_jx3web; 
            }
        }        

        /// <summary>
        /// 读取连接字符串
        /// </summary>
        /// <param name="fileName">文件名</param>
        /// <param name="sectionName">分段名</param>
        /// <param name="keyName">键名</param>
        /// <returns>连接字符串</returns>
        private static string GetConnectString(string fileName, string sectionName, string keyName)
        {            
            string content = FileFolderHelper.FileToString(fileName);
            SymmetricMethod symmetricMethod = new SymmetricMethod();
            content = symmetricMethod.Decrypto(content);

            IniStructure iniStructure = new IniStructure();
            iniStructure = IniStructure.ReadIniWithContent(content);
            connectString = iniStructure.GetValue(sectionName, keyName);

            return connectString;
        }

        /// <summary>
        /// 初始化DataBaseManager对象
        /// </summary>        
        private void Init()
        {
            try
            {
                string fileName = Path.Combine(Application.StartupPath, flowChartEditorConfigFileName);
                string connectString = GetConnectString(fileName, "General", "ConnString");

                if (!string.IsNullOrEmpty(connectString))
                {
                    conn = new SqlConnection(connectString);
                }           
            }
            catch (SqlException ex)
            {
                PrintExceptionInformation(ex);                
            }
        }
     
        /// <summary>
        /// 绘图加锁
        /// </summary>
        /// <param name="diagramID">绘图ID</param>
        /// <returns>是否加锁成功</returns>
        public bool Diagram_Lock(int diagramID)
        {
            bool lockSuccess = true;
            string hostName = System.Net.Dns.GetHostName();
            
            // 看是否被别人锁了
            string sqlString = string.Format("SELECT WhoLocked FROM {0} WHERE ID = {1}", diagramTable, diagramID);
            DataTable infoTable = GetDataTable(sqlString);

            if (infoTable.Rows.Count > 0)
            {
                string strWhoLocked = infoTable.Rows[0]["WhoLocked"].ToString();

                // 已经有人加锁了，通过异常返回加锁人的信息（不推荐这样的写法）
                if (strWhoLocked != hostName && strWhoLocked != "")
                {
                    throw new Exception(strWhoLocked);               
                }

                sqlString = string.Format("UPDATE {0} SET WhoLocked = '{1}' WHERE ID = {2}", diagramTable, hostName, diagramID);
                lockSuccess = ExecuteSqlCommand(sqlString);
            }
            else 
            {
                lockSuccess = false;
            }            

            return lockSuccess;
        }
        
        /// <summary>
        /// 绘图解锁
        /// </summary>
        /// <param name="diagramID">绘图ID</param>
        /// <returns>是否解锁成功</returns>
        public bool Diagram_Unlock(int diagramID)
        {
            bool unlockSuccess = true;
            string hostName = System.Net.Dns.GetHostName();
            string sqlString = string.Format("SELECT WhoLocked FROM {0} WHERE ID = {1}", diagramTable, diagramID);
            DataTable infoTable = GetDataTable(sqlString);

            if (infoTable.Rows.Count > 0)
            {
                string lockHostName = infoTable.Rows[0]["WhoLocked"].ToString();

                if (lockHostName == hostName)
                {
                    sqlString = string.Format("UPDATE {0} SET WhoLocked = NULL WHERE ID = {1}", diagramTable, diagramID);
                    ExecuteSqlCommand(sqlString);
                }                
            }
            else
            {
                unlockSuccess = false;
            }            

            return unlockSuccess;
        }

        /// <summary>
        /// 检查是否有人锁定绘图
        /// </summary>
        /// <param name="diagramID">绘图ID</param>
        /// <returns>锁定人的机器名</returns>
        public string Diagram_WhoLocked(int diagramID)
        {
            string lockHostName = null;
            string sqlString = string.Format("SELECT WhoLocked FROM {0} WHERE ID = {1}", diagramTable, diagramID);
            DataTable infoTable = GetDataTable(sqlString);

            if (infoTable.Rows.Count > 0)
            {
                if (!string.IsNullOrEmpty(infoTable.Rows[0]["WhoLocked"].ToString()))
                {
                    lockHostName = infoTable.Rows[0]["WhoLocked"].ToString();
                }
            }            

            return lockHostName;
        }

        /// <summary>
        /// 清空当前用户的所有锁
        /// </summary>
        public void ClearRecordLocks()
        {
            string hostName = System.Net.Dns.GetHostName();
            string sqlString = string.Format("UPDATE {0} SET WhoLocked = NULL WHERE WhoLocked = '{1}'", diagramTable, hostName);

            ExecuteSqlCommand(sqlString);
        }

        /// <summary>
        /// 检查用户授权
        /// </summary>
        /// <param name="userName">用户名</param>
        /// <param name="ipAddress">用户IP</param>
        /// <returns>是否拥有授权</returns>
        public bool CheckAuthority(string userName, string ipAddress)
        {
            bool hasAuthority = false;

            try
            {
                if (conn_jx3web.State == ConnectionState.Closed)
                {
                    conn_jx3web.Open();
                }

                SqlCommand cmd = conn_jx3web.CreateCommand();
                string sqlString = string.Format("SELECT _index FROM sys_useracl WHERE userid = '{0}' AND ip = '{1}'",
                                                 userName, ipAddress);
                cmd.CommandText = sqlString;
                object executeResult = cmd.ExecuteScalar();

                if (executeResult != null && !(executeResult is DBNull))
                {
                    hasAuthority = true;
                }
            }
            catch (SqlException ex)
            {
                PrintExceptionInformation(ex);
            }
            finally
            {
                if (conn_jx3web.State == ConnectionState.Open)
                {
                    conn_jx3web.Close();
                }
            }

            return hasAuthority;
        }

        /// <summary>
        /// 反序列化数据
        /// </summary>
        /// <param name="data">输入</param>
        /// <returns>输出</returns>
        private object DeserializeData(byte[] data)
        {
            object content = null;

            try
            {
                IFormatter formater = new BinaryFormatter();
                content = formater.Deserialize(new MemoryStream(data));
            }
            catch (System.Exception ex)
            {
                PrintExceptionInformation(ex);
            }            

            return content;
        }
        
        /// <summary>
        /// 获取绘图数据
        /// </summary>
        /// <param name="id">绘图ID</param>
        /// <returns>是否获取成功</returns>
        public object GetDiagramData(string id)
        {
            string sqlString = string.Format("SELECT Data FROM {0} WHERE ID = {1}", diagramTable, id);
            object executeResult = ExecuteSqlSelectCommand(sqlString);
            object content = null;

            if (executeResult != null && !(executeResult is DBNull))
            {
                content = DeserializeData((byte[])executeResult);
            }

            return content;
        }

        /// <summary>
        /// 获取绘图数据
        /// </summary>
        /// <param name="path">绘图路径</param>
        /// <param name="diagramClass">绘图类型</param>
        /// <returns>是否获取成功</returns>
        public object GetDiagramDataByPath(string path, string diagramClass)
        {

            string sqlString = string.Format("SELECT Data FROM {0} WHERE FullPath = '{1}' AND Class = '{2}'",
                                             diagramTable, path, diagramClass);
            object executeResult = ExecuteSqlSelectCommand(sqlString);
            object content = null; // 绘图内容对象

            if (executeResult != null && !(executeResult is DBNull))
            {
                content = DeserializeData((byte[])executeResult);
            }            

            return content;
        }

        /// <summary>
        /// 加载绘图备份数据
        /// </summary>
        /// <param name="backupID">绘图备份ID</param>
        /// <returns>是否加载成功</returns>
        public object GetBackupDiagramData(string backupID)
        {
            string sqlString = string.Format("SELECT Data FROM {0} WHERE ID = {1}", diagramBackupTable, backupID);
            object executeResult = ExecuteSqlSelectCommand(sqlString);
            object content = null; // 流程图内容对象

            if (executeResult != null && !(executeResult is DBNull))
            {
                content = DeserializeData((byte[])executeResult);
            }

            return content;
        }

        /// <summary>
        /// 复制绘图的数据
        /// </summary>
        /// <param name="originID">源绘图的ID</param>
        /// <param name="destinID">目标绘图的ID</param>
        /// <returns>是否复制成功</returns>
        public bool CopyDiagramData(string originID, string destinID)
        {
            string sqlString = string.Format("UPDATE {0} SET Data = (SELECT Data FROM {0} WHERE ID = '{1}') WHERE ID = '{2}'",
                                             diagramTable, destinID, originID);
            bool executeSuccess = ExecuteSqlCommand(sqlString);

            return executeSuccess;
        }

        /// <summary>
        /// 恢复绘图的数据
        /// </summary>
        /// <param name="originID">原绘图的ID</param>
        /// <param name="backupID">备份绘图的ID</param>
        /// <returns>是否恢复成功</returns>
        public bool RestoreDiagramData(string originID, string backupID)
        {
            bool result = true;
            SqlTransaction transaction = null;
            string hostName = System.Net.Dns.GetHostName();

            // 写数据库
            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                transaction = conn.BeginTransaction();
                SqlCommand cmd = conn.CreateCommand();
                cmd.Transaction = transaction;

                // 获取版本号
                string sqlString = string.Format("SELECT Version FROM {0} WHERE ID = {1}", diagramTable, originID);
                cmd.CommandText = sqlString;
                int currentVersion = 1;
                object version = cmd.ExecuteScalar();

                if (version != null && !(version is DBNull))
                {
                    currentVersion = (int)version + 1;
                }

                // 更新绘图信息
                sqlString = string.Format("UPDATE {0} SET Data = (SELECT Data FROM {1} WHERE ID = {2}), Version = {3}, Host = '{4}', EditTime = getdate() WHERE ID = {5}",
                                          diagramTable, diagramBackupTable, backupID, currentVersion, hostName, originID);
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();

                transaction.Commit();
            }
            catch (SqlException ex)
            {
                PrintExceptionInformation(ex);
                transaction.Rollback();
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
        /// 序列化数据
        /// </summary>
        /// <param name="content">输入</param>
        /// <returns>输出</returns>
        private byte[] SerializeData(object content)
        {
            byte[] data = null;

            try
            {
                IFormatter formatter = new BinaryFormatter();
                Stream memoryStream = new MemoryStream();
                formatter.Serialize(memoryStream, content);
                data = new byte[memoryStream.Length];
                memoryStream.Position = 0;
                memoryStream.Read(data, 0, (int)memoryStream.Length);
                memoryStream.Close();
            }
            catch (SerializationException ex)
            {
                PrintExceptionInformation(ex);              
            }

            return data;
        }

        /// <summary>
        /// 将绘图数据保存到文件
        /// </summary>
        /// <param name="fileName">文件路径</param>
        /// <param name="content">绘图内容</param>
        /// <returns>是否保存成功</returns>
        public bool SaveDiagramDataToFile(string fileName, object content)
        {
            bool saveSuccess = false;
            byte[] data = SerializeData(content);

            if (data != null)
            {
                string folderName = Path.GetDirectoryName(fileName); // 获取目录

                if (!string.IsNullOrEmpty(folderName) && !Directory.Exists(folderName)) // 目录不存在
                {
                    Directory.CreateDirectory(folderName); // 先创建目录，再创建文件
                }

                File.WriteAllBytes(fileName, data);
                saveSuccess = true;
            }

            return saveSuccess;
        }

        /// <summary>
        /// 从文件中加载绘图数据
        /// </summary>
        /// <param name="fileName">文件名</param>
        /// <returns>绘图数据</returns>
        public object LoadDiagramDataFromFile(string fileName)
        {
            object data = null;

            if (File.Exists(fileName))
            {
                try
                {
                    byte[] buffer = File.ReadAllBytes(fileName);
                    data = DeserializeData(buffer);
                }
                catch (Exception ex)
                {
                    PrintExceptionInformation(ex);
                }                
            }

            return data;
        }

        /// <summary>
        /// 保存绘图
        /// </summary>
        /// <param name="path">绘图id</param>
        /// <param name="diagramClass">绘图分类</param>
        /// <param name="content">绘图数据</param>
        /// <returns>是否保存成功</returns>
        public bool SaveDiagramData(string path, string diagramClass, object content)
        {
            bool saveSuccess = true;
            byte[] data = SerializeData(content);

            if (data != null)
            {
                // 写数据库
                try
                {
                    if (conn.State == ConnectionState.Closed) // 打开sql连接
                    {
                        conn.Open();
                    }

                    // 更新绘图信息
                    string sqlString = string.Format("UPDATE {0} SET Data = @data WHERE FullPath = '{1}' AND Class = '{2}'",
                                                     diagramTable, path, diagramClass);
                    SqlCommand cmd = conn.CreateCommand();
                    cmd.CommandText = sqlString;
                    SqlParameter spcode = new SqlParameter("@data", data);
                    cmd.Parameters.Add(spcode);
                    cmd.ExecuteNonQuery();
                }
                catch (SqlException ex)
                {
                    PrintExceptionInformation(ex);
                    saveSuccess = false;
                }
                finally
                {
                    if (conn.State == ConnectionState.Open) // 关闭sql连接
                    {
                        conn.Close();
                    }
                }
            }
            else
            {
                saveSuccess = false;
            }

            return saveSuccess;
        }

        /// <summary>
        /// 保存绘图
        /// </summary>
        /// <param name="id">绘图id</param>
        /// <param name="content">绘图数据</param>
        /// <returns>是否保存成功</returns>
        public bool SaveDiagramData(string id, object content)
        {
            bool saveSuccess = true;
            string hostName = System.Net.Dns.GetHostName(); 
            SqlTransaction transaction = null;
            byte[] data = SerializeData(content);
            
            if (data != null)
            {
                // 写数据库
                try
                {
                    if (conn.State == ConnectionState.Closed) // 打开sql连接
                    {
                        conn.Open();
                    }

                    transaction = conn.BeginTransaction();
                    SqlCommand cmd = conn.CreateCommand();
                    cmd.Transaction = transaction;

                    // 获取版本号
                    string sqlString = string.Format("SELECT Version FROM {0} WHERE ID = {1}", diagramTable, id);
                    cmd.CommandText = sqlString;
                    int currentVersion = 1;
                    object version = cmd.ExecuteScalar();

                    if(version != null && !(version is DBNull))
                    {
                        currentVersion = (int)version + 1;
                    }

                    sqlString = string.Format("SELECT FullPath FROM {0} WHERE ID = {1}", diagramTable, id);
                    cmd.CommandText = sqlString;
                    string fullPath = cmd.ExecuteScalar() as string;

                    sqlString = string.Format("SELECT Class FROM {0} WHERE ID = {1}", diagramTable, id);
                    cmd.CommandText = sqlString;
                    string diagramClass = cmd.ExecuteScalar() as string;

                    // 更新绘图信息
                    sqlString = string.Format("UPDATE {0} SET Data = @data, EditTime = getdate(), Host = '{1}', Version = {2} WHERE ID = {3}",
                                              diagramTable, hostName, currentVersion, id);
                    cmd.CommandText = sqlString;
                    SqlParameter spcode = new SqlParameter("@data", data);
                    cmd.Parameters.Add(spcode);
                    cmd.ExecuteNonQuery();

                    // 备份绘图信息
                    sqlString = string.Format("INSERT INTO {0} (OriginID, EditTime, Data, Class, Host, Version, FullPath) VALUES ('{1}', getdate(), @data, '{2}', '{3}', '{4}', '{5}')",
                                              diagramBackupTable, id, diagramClass, hostName, currentVersion, fullPath);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    // 检查备份信息是否需要清理
                    sqlString = string.Format("SELECT COUNT(ID) FROM {0} WHERE OriginID = {1}", diagramBackupTable, id);
                    cmd.CommandText = sqlString;
                    object executeResult = cmd.ExecuteScalar();

                    // 需要清理备份信息
                    if ((int)executeResult > maxBackupCount * 2)
                    {
                        sqlString = string.Format("DELETE FROM {0} WHERE OriginID = {2} AND ID NOT IN (SELECT TOP {1} ID FROM {0} WHERE OriginID = {2} ORDER BY EditTime DESC)",
                                                  diagramBackupTable, maxBackupCount, id);
                        cmd.CommandText = sqlString;
                        cmd.ExecuteNonQuery();
                    }

                    transaction.Commit();
                }
                catch (SqlException ex)
                {
                    PrintExceptionInformation(ex);
                    transaction.Rollback();
                    saveSuccess = false;
                }
                finally
                {
                    if (conn.State == ConnectionState.Open) // 关闭sql连接
                    {
                        conn.Close();
                    }
                }
            }
            else
            {
                saveSuccess = false;
            }            

            return saveSuccess;
        }

        /// <summary>
        /// 创建新的流程图
        /// </summary>
        /// <param name="path">流程图路径</param>
        /// <param name="isFolder">是否是文件夹</param>
        /// <param name="diagramClass">流程图分类</param>
        /// <returns>流程图的id</returns>
        public string CreateNewDiagram(string path, bool isFolder, string diagramClass)
        {
            string id = null; // 流程图的id
            SqlTransaction transaction = null; // 事务

            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                transaction = conn.BeginTransaction();
                SqlCommand cmd = conn.CreateCommand();
                cmd.Transaction = transaction;

                //查重复
                cmd.CommandText = string.Format("SELECT ID FROM {0} WHERE FullPath = '{1}' AND Class = '{2}' AND Hide <> 1",
                                                diagramTable, path.Replace("'", "''"), diagramClass);
                
                if (cmd.ExecuteScalar() != null)
                {
                    MessageBox.Show("该名称的绘图已经存在！", "新建绘图", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    string hostName = System.Net.Dns.GetHostName();

                    // 插入记录
                    string sqlString = string.Format("INSERT INTO {0} (FullPath, Host, EditTime, IsFolder, Class) VALUES ('{1}', '{2}', getdate(), '{3}', '{4}')",
                                                     diagramTable, path, hostName, isFolder, diagramClass);
                    cmd.CommandText = sqlString;
                    int iret = cmd.ExecuteNonQuery();

                    // 返ID
                    sqlString = string.Format("SELECT MAX(ID) FROM {0}", diagramTable);
                    cmd.CommandText = sqlString;
                    id = cmd.ExecuteScalar().ToString();
                    transaction.Commit(); // 提交事务
                }                                
            }
            catch (SqlException ex)
            {
                PrintExceptionInformation(ex);

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
        /// 获取绘图信息
        /// </summary>
        /// <param name="chartClass">绘图分类</param>
        /// <returns>绘图信息</returns>
        public DataTable GetDiagramInformation(string chartClass)
        {
            DataTable scriptTable = null;

            try
            {
                string sqlString = string.Format("SELECT ID, FullPath, IsFolder, Class, EditTime, Host, Version From {0} WHERE Class = '{1}' AND (Hide IS NULL OR Hide = 0)", 
                                                 diagramTable, chartClass);

                if (string.IsNullOrEmpty(chartClass))
                {
                    sqlString = string.Format("SELECT ID, FullPath, IsFolder, Class, EditTime, Host, Version From {0} WHERE Hide IS NULL OR Hide = 0",
                                              diagramTable);
                }

                scriptTable = GetDataTable(sqlString);

                if (scriptTable.Rows.Count == 0) // 不存在脚本记录
                {
                    scriptTable = null;
                }
            }
            catch (SqlException ex)
            {
                PrintExceptionInformation(ex);
            }

            return scriptTable;
        }
        
        /// <summary>
        /// 修改绘图的路径
        /// </summary>
        /// <param name="id">绘图的ID</param>
        /// <param name="newPath">新的绘图路径</param>
        /// <returns>是否修改成功</returns>
        public bool UpdateDiagramPath(string id, string newPath)
        {
            string sqlString = string.Format("UPDATE {0} SET FullPath = '{1}' WHERE ID = {2}",
                                             diagramTable, newPath, id);
            bool executeSuccess = ExecuteSqlCommand(sqlString);

            return executeSuccess;
        }

        /// <summary>
        /// 删除绘图
        /// </summary>
        /// <param name="id">绘图的ID</param>
        /// <returns>是否删除成功</returns>
        public bool DeleteDiagram(string id)
        {            
            string sqlString = string.Format("DELETE FROM {0} WHERE ID = {1}", diagramTable, id);
            // string sqlString = string.Format("UPDATE {0} SET Hide = 1 WHERE ID = {1}", diagramTable, id);

            bool executeSuccess = ExecuteSqlCommand(sqlString);

            return executeSuccess;
        }

        /// <summary>
        /// 删除该路径下的所有绘图
        /// </summary>
        /// <param name="path">当前路径</param>
        /// <returns>是否删除成功</returns>
        public bool DeleteMultiDiagram(string path)
        {
            bool result = true; // 是否执行成功
            SqlTransaction transaction = null; // 数据库事务

            try
            {
                string sqlString = string.Format("SELECT ID FROM {0} WHERE FullPath LIKE '{1}%'", diagramTable, path);
                DataTable table = GetDataTable(sqlString);

                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                transaction = conn.BeginTransaction();
                SqlCommand cmd = conn.CreateCommand();
                cmd.Transaction = transaction;

                foreach (DataRow row in table.Rows)
                {
                    string id = row["ID"].ToString();
                    sqlString = string.Format("DELETE FROM {0} WHERE ID = {1}", diagramTable, id);
                    // sqlString = string.Format("UPDATE {0} SET Hide = 1 WHERE ID = {1}", diagramTable, id);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                }

                transaction.Commit();
            }
            catch (SqlException ex)
            {
                PrintExceptionInformation(ex);
                transaction.Rollback();
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
        /// 获取绘图的编辑时间
        /// </summary>
        /// <param name="id">绘图的ID</param>
        /// <returns>绘图的编辑时间</returns>
        public DateTime GetDiagramEditTime(string id)
        {
            DateTime editTime = DateTime.MinValue;
            string sqlString = string.Format("SELECT EditTime FROM {0} WHERE ID = {1}", diagramTable, id);
            object executeResult = ExecuteSqlSelectCommand(sqlString);

            if (executeResult != null && !(executeResult is DBNull))
            {
                editTime = (DateTime)executeResult;
            }

            return editTime;
        }

        /// <summary>
        /// 获取绘图备份的编辑时间
        /// </summary>
        /// <param name="backupID">绘图备份的ID</param>
        /// <returns>绘图备份的编辑时间</returns>
        public DateTime GetBackupDiagramEditTime(string backupID)
        {
            DateTime editTime = DateTime.MinValue;
            string sqlString = string.Format("SELECT EditTime FROM {0} WHERE ID = {1}", diagramBackupTable, backupID);
            object executeResult = ExecuteSqlSelectCommand(sqlString);

            if (executeResult != null && !(executeResult is DBNull))
            {
                editTime = (DateTime)executeResult;
            }

            return editTime;
        }

        /// <summary>
        /// 获取绘图的编辑版本
        /// </summary>
        /// <param name="id">绘图的ID</param>
        /// <returns>绘图的编辑版本</returns>
        public int GetDiagramVersion(string id)
        {
            int version = 0;
            string sqlString = string.Format("SELECT Version FROM {0} WHERE ID = {1}", diagramTable, id);
            object executeResult = ExecuteSqlSelectCommand(sqlString);

            if (executeResult != null && !(executeResult is DBNull))
            {                
                version = (int)executeResult;               
            }

            return version;
        }

        /// <summary>
        /// 获取绘图备份的编辑版本
        /// </summary>
        /// <param name="backupID">绘图的ID</param>
        /// <returns>绘图的编辑版本</returns>
        public int GetBackupDiagramVersion(string backupID)
        {
            int version = 0;
            string sqlString = string.Format("SELECT Version FROM {0} WHERE ID = {1}", diagramBackupTable, backupID);
            object executeResult = ExecuteSqlSelectCommand(sqlString);

            if (executeResult != null && !(executeResult is DBNull))
            {
                version = (int)executeResult;
            }

            return version;
        }

        /// <summary>
        /// 获取流程图的编辑者
        /// </summary>
        /// <param name="id">流程图的id</param>
        /// <returns>流程图的编辑者</returns>
        public string GetDiagramEditor(string id)
        {
            string userName = "";
            string sqlString = string.Format("SELECT Host FROM {0} WHERE ID = {1}", diagramTable, id);
            object executeResult = ExecuteSqlSelectCommand(sqlString);

            if (executeResult != null && !(executeResult is DBNull))
            {
                userName = executeResult as string;
            }

            return userName;
        }

        /// <summary>
        /// 获取所有的地图链表
        /// </summary>
        /// <returns>所有的地图链表</returns>
        public List<string> GetMapList()
        {
            List<string> list = new List<string>();

            SqlConnection j3web_conn = GetJx3webConnection();
            string sqlString = "SELECT Name FROM MapList";
            DataTable table = GetDataTable(sqlString, j3web_conn);

            foreach(DataRow dataRow in table.Rows)
            {
                list.Add(dataRow[0].ToString());
            }

            return list;
        }

        /// <summary>
        /// 获取用户的配置信息
        /// </summary>
        /// <param name="ip">用户的IP</param>
        /// <returns>用户的配置信息</returns>
        public object GetUserConfigInfo(string ip)
        {
            object info = null;
            string sqlString = string.Format("SELECT Data FROM UserConfigInfo WHERE IP = '{0}'", ip);
            object executeResult = ExecuteSqlSelectCommand(sqlString);

            if (executeResult != null && !(executeResult is DBNull))
            {
                info = DeserializeData((byte[])executeResult);                
            }

            return info;
        }

        /// <summary>
        /// 检查用户信息是否存在
        /// </summary>
        /// <returns>用户的IP</returns>
        public bool CheckUserInfoExist(string ip)
        {
            string sqlString = string.Format("SELECT HostName FROM UserConfigInfo WHERE IP = '{0}'", ip);
            DataTable table = GetDataTable(sqlString);
            bool exist = table.Rows.Count > 0;

            return exist;
        }

        /// <summary>
        /// 添加用户信息
        /// </summary>
        /// <param name="ip">用户的IP</param>
        /// <param name="hostName">用户的域名</param>
        /// <param name="info">配置信息</param>
        /// <returns>是否添加成功</returns>
        public bool AddUserConfigInfo(string ip, string hostName, object info)
        {
            bool result = true;

            //序列化对象
            byte[] data = null;
            try
            {
                IFormatter formatter = new BinaryFormatter();
                Stream s = new MemoryStream();
                formatter.Serialize(s, info);
                data = new byte[s.Length];
                s.Position = 0;
                s.Read(data, 0, (int)s.Length);
                s.Close();
            }
            catch (SerializationException ex)
            {
                MessageBox.Show("在序列化用户配置信息时产生异常: " + ex.ToString());
            }

            //写数据库
            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("INSERT INTO UserConfigInfo (IP, HostName, Data) VALUES ('{0}', '{1}', @data)",
                                                 ip, hostName);
                SqlCommand cmd = new SqlCommand(sqlString, conn);
                SqlParameter spcode = new SqlParameter("@data", data);
                cmd.Parameters.Add(spcode);
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                PrintExceptionInformation(ex);
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
        /// 设置用户的配置信息
        /// </summary>
        /// <param name="ip">用户的IP</param>
        /// <param name="info">用户的配置信息</param>
        /// <returns>是否设置成功</returns>
        public bool SetUserConfigInfo(string ip, object info)
        {
            bool result = true;
            byte[] data = SerializeData(info);

            //写数据库
            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                string sqlString = string.Format("UPDATE UserConfigInfo SET Data = @data WHERE IP = '{0}'",
                                                 ip);
                SqlCommand cmd = new SqlCommand(sqlString, conn);
                SqlParameter spcode = new SqlParameter("@data", data);
                cmd.Parameters.Add(spcode);
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                PrintExceptionInformation(ex);
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

    #region 大类、事件、API、本库常数
        //查询大类
        public object[] GetTypeDefine(int id)
        {
            DataTable dt = GetDataTable("select * from typedefine where id=" + id.ToString());
            if (dt.Rows.Count == 0)
                return null;
            else
                return new object[] 
                {
                    dt.Rows[0]["Description"],
                    dt.Rows[0]["IsDuplicate"],
                    dt.Rows[0]["IsEnum"],
                    dt.Rows[0]["QueryScript"],
                    dt.Rows[0]["GenAsString"]
                };
        }

        //查询事件信息 返回名字、描述
        public object[] GetEventDefine(int id)
        {
            DataTable dt = GetDataTable("select * from eventdefine where id=" + id.ToString());
            if (dt.Rows.Count == 0)
                return null;
            else
                return new object[] 
                {
                    dt.Rows[0]["Name"],
                    dt.Rows[0]["Description"],
                    dt.Rows[0]["MainClass"]
                };
        }

        //查询事件参数
        public DataTable GetEventDefineArg(int id)
        {
            string strSQL = @"
declare @xml xml
select @xml=arglist from eventdefine where id = {0}
select 
c.value('@id','nvarchar(MAX)') as 'id' ,
c.value('@Type','nvarchar(MAX)') as 'type',
c.value('@Text','nvarchar(MAX)') as 'name',
c.value('@Name','nvarchar(MAX)') as 'default'
from @xml.nodes('/ArgList/Arg') as n(c)
";
            strSQL = string.Format(strSQL, id.ToString());
            DataTable dt = GetDataTable(strSQL);
            if (dt.Rows.Count == 0)
                return null;
            else
                return dt;
        }

        //查询API信息 返回名字、描述、
        public object[] GetAPIDefine(int id)      
        {
            DataTable dt = GetDataTable("select (class + '--' + text) as text,Description,rettype,name,isasyn,tip from ApiDefine where id <> 1 and id=" + id.ToString());
            if (dt.Rows.Count == 0)
                return null;
            else
                return new object[] 
                {
                    dt.Rows[0]["Text"],
                    dt.Rows[0]["Description"],
                    dt.Rows[0]["rettype"],
                    dt.Rows[0]["name"],
                    dt.Rows[0]["isasyn"],
                    dt.Rows[0]["tip"]
                };
        }

        //查询API参数
        public DataTable GetAPIDefineArg(int id)
        {
            string strSQL = @"
declare @xml xml
select @xml=arglist from ApiDefine where id <> 1 and id = {0}
select 
c.value('@id','nvarchar(MAX)') as 'id' ,
c.value('@Type','nvarchar(MAX)') as 'type',
c.value('@Text','nvarchar(MAX)') as 'name',
c.value('@Name','nvarchar(MAX)') as 'default'
from @xml.nodes('/ArgList/Arg') as n(c)
";
            strSQL = string.Format(strSQL, id.ToString());
            DataTable dt = GetDataTable(strSQL);
            if (dt.Rows.Count == 0)
                return null;
            else
                return dt;
        }

        //根据返回值查API们的ID
        public int[] GetAPIsByRetType(int typeid)
        {
            DataTable dt = GetDataTable("select id from ApiDefine where id <> 1 and rettype=" + typeid.ToString());
            if (dt.Rows.Count == 0)
                return null;
            else
            {
                int[] ret = new int[dt.Rows.Count];
                for(int i = 0; i < ret.Length; i++)
                {
                    ret[i] = Convert.ToInt32(dt.Rows[i]["id"].ToString());
                }
                return ret;
            }
        }

        //查所有API们的ID
        public int[] GetAllAPIs()
        {
            DataTable dt = GetDataTable("select id from ApiDefine where id <> 1");
            if (dt.Rows.Count == 0)
                return null;
            else
            {
                int[] ret = new int[dt.Rows.Count];
                for(int i = 0; i < ret.Length; i++)
                {
                    ret[i] = Convert.ToInt32(dt.Rows[i]["id"].ToString());
                }
                return ret;
            }
        }


        //查API们的ID，返回值不是制定类型的。
        public int[] GetAPIs_Except_RetType(int typeid)
        {
            DataTable dt = GetDataTable("select id from ApiDefine where id <> 1 and (not rettype='" + typeid.ToString() + "')");
            if (dt.Rows.Count == 0)
                return null;
            else
            {
                int[] ret = new int[dt.Rows.Count];
                for (int i = 0; i < ret.Length; i++)
                {
                    ret[i] = Convert.ToInt32(dt.Rows[i]["id"].ToString());
                }
                return ret;
            }
        }

        //返回事件列表
        public int[] GetEvents()
        {
            DataTable dt = GetDataTable("select id from eventdefine");
            if (dt.Rows.Count == 0)
                return null;
            else
            {
                int[] ret = new int[dt.Rows.Count];
                for (int i = 0; i < ret.Length; i++)
                {
                    ret[i] = Convert.ToInt32(dt.Rows[i]["id"].ToString());
                }
                return ret;                
            }
        }

        //根据返回值查Const们的DBValue
        public string[] GetConstDBValuesByRetType(int typeid)
        {
            DataTable dt = GetDataTable("select value from constant where attachedtype=" + typeid.ToString());
            if (dt.Rows.Count == 0)
                return null;
            else
            {
                string[] ret = new string[dt.Rows.Count];
                for(int i = 0; i < ret.Length; i++)
                {
                    ret[i] = dt.Rows[i]["Value"].ToString();
                }
                return ret;
            }
        }

        //从本库查询常数的屏显
        public string GetConstText(int nTypeID, string dbValue)
        {
            DataTable dt = GetDataTable("select text from constant where attachedType='" + nTypeID.ToString() + "' and value='" + dbValue + "'");
            if (dt.Rows.Count == 0)
                return nTypeID.ToString() + "<找不到中文屏显>";
            else
                return dt.Rows[0]["text"].ToString();
            
        }

    #endregion


    #region 给JX3编译器使用，导出系统文件
        //获取API文件内容
        public string GetAPIFileContext()
        {
            string sql = @"declare @ret varchar(MAX)
set @ret = ''
select @ret=@ret+
case when script is NUll then ''
else
    script
end + '
'
from apidefine
select @ret";
            string strTemp = @"Include('scripts/flowlib/lib.lua')
Include('scripts/flowlib/event_filter.lua')
using('comgr')
declare('API', {})
";
            return strTemp + GetConstTextFromSQL(sql, this.conn);
        }

        //获取事件派发器文件内容
        public string GetEventDispatchFileContext()
        {
            string sql = @"declare @ret varchar(MAX)
set @ret = ''
declare cur CURSOR
STATIC
FOR select id, Name from baseevent
open cur
declare @rows int, @i int
declare @name varchar(max), @id int
set @i = 0
set @rows = @@cursor_rows
while @i<@rows
begin
	fetch next from cur into @id, @name
	set @ret=@ret + 'EventDispatch.A' + @name + ' = function(key, ...)
    local bret = false
'
	select @ret = @ret + w from
	(
	select '	bret = bret or EventDispatch.AOnAllEvent(' + cast(id as varchar(max)) + ',  key, ...)
'  w from eventdefine where attachedbaseevent = @id
	) t
	set @ret = @ret + '    return bret
end

'
	set @i = @i + 1
end
close cur
deallocate cur
select @ret";
            string strTemp = @"Include('scripts/flowlib/api.lua')
Include('scripts/flowlib/event_filter.lua')
Include('scripts/flowlib/regist_file.lua')
declare('EventDispatch', {})
using('Event')
EventDispatch.AOnAllEvent = function(eventid, key, ...)
    local bret = false
	env = Event[eventid]
    if not env then
        trace('not found event implement: ' .. eventid)
        return bret
    end
    if env.FunList[key] ~= nil then
        trace('got key:', key)
        for k, v in pairs(env.FunList[key]) do
            local ta = {}
            for i = 1, #arg do
                ta[i] = arg[i]
            end
            for i = 1, #v do
                ta[i +#arg] = v[i]
            end
            if env.Condition(unpack(ta)) then
                trace(' do event: ', eventid, key)
                bret = true
                event_dispatch(k, env.Args(unpack(ta)))
            else
                trace(' not do event: ', eventid, key)
            end
        end
    else
	    trace('not got key', key)
    end
    return bret
end

";
            return strTemp + GetConstTextFromSQL(sql, this.conn);
        }

        //获取事件派发器文件内容
        public string GetEventFilterFileContext()
        {
            string sql = @"declare @ret varchar(MAX)
set @ret = ''
select @ret=@ret+
case when script is NUll then ''
else
    script
end + '
'
from eventdefine
select @ret";
            string strTemp = @"Include('scripts/flowlib/lib.lua')
using('comgr')
declare('Event', {})

";
            return strTemp + GetConstTextFromSQL(sql, this.conn);
        }

        public SqlConnection GetJx3webConnection()
        {
            return this.conn_jx3web;
        }

        //Lua用函数
        public SqlConnection NewCustomConnection(string strConnStr)
        {
            SqlConnection conn = new SqlConnection(strConnStr); 
            return conn;
        }

        //跨库查询常数的屏显
        public string GetConstTextFromSQL(string strSQL,SqlConnection conn)
        {
            DataTable dt = GetDataTable(strSQL, conn);
            if (dt.Rows.Count == 0)
                return null;
            else
                return dt.Rows[0][0].ToString();
        }

    #endregion


    #region AI图相关
        /// <summary>
        /// 查询AI事件定义全表
        /// </summary>
        /// <returns>AI事件定义全表</returns>
        public DataTable GetTable_AI_Event_Define()
        {
            string sql = "select * from AI_Event_Define";
            DataTable dt = GetDataTable(sql, conn_jx3web);

            return dt;
        }

        /// <summary>
        /// 查询AI Action定义全表
        /// </summary>
        /// <returns>AI Action定义全表</returns>
        public DataTable GetTable_AI_Action_Define()
        {
            string sql = "SELECT id, actionid, name, ename, info, class, parms, rettype FROM AI_Action_Define ORDER BY class";
            DataTable dt = GetDataTable(sql, conn_jx3web);

            return dt;
        }

        /// <summary>
        /// 查询AI枚举定义全表
        /// </summary>
        /// <returns>AI枚举定义全表</returns>
        public DataTable GetTable_AI_Enum_Define()
        {
            string sql = "select * from AI_Enum_Define";
            DataTable dt = GetDataTable(sql, conn_jx3web);

            return dt;
        }        
       
        /// <summary>
        /// 获取AI自定义动作的内容
        /// </summary>
        /// <param name="conn">sql连接</param>
        /// <returns>自定义动作的内容</returns>
        public string GetUserActionContext(SqlConnection conn)
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

                for(int i = 1; i <= strPList.Length; i++)
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
        /// 获取AI自定义动作的内容
        /// </summary>
        /// <returns>AI自定义动作的内容</returns>
        public string GetUserActionContext()
        {
            return GetUserActionContext(conn_jx3web);
        }       

        /// <summary>
        /// 获取流程图LS标准对应字符串的索引
        /// </summary>
        /// <param name="content">字符串</param>
        /// <param name="mapName">场景名</param>
        /// <returns>索引</returns>
        public string InsertIntoFlowChartLS(string content, string mapName)
        {
            string id = null; // 返回的id
            SqlTransaction transaction = null; // 事务  

            try
            {
                if (conn_jx3web.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn_jx3web.Open();
                }

                content = content.Replace("'", "''"); // 存入数据库前进行替换
                transaction = conn_jx3web.BeginTransaction(); // 开启事务
                SqlCommand cmd = conn_jx3web.CreateCommand();
                cmd.Transaction = transaction;

                string sqlString = string.Format("SELECT id FROM sys_script_lsfile WHERE content = '{0}' AND classification = '{1}'",
                                                 content, mapName);
                cmd.CommandText = sqlString;
                object executeResult = cmd.ExecuteScalar();

                if (executeResult != null && !(executeResult is DBNull)) // 表中已经存在
                {
                    id = executeResult.ToString();
                }
                else // 表中不存在，需要添加
                {
                    sqlString = string.Format("INSERT INTO {0} ({1}, {2}) VALUES ('{3}', '{4}')",
                                              "sys_script_lsfile", "content", "classification", content, mapName);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();

                    sqlString = string.Format("SELECT MAX({0}) FROM {1}", "id", "sys_script_lsfile");
                    cmd.CommandText = sqlString;
                    id = cmd.ExecuteScalar().ToString();
                }
                
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
                if (conn_jx3web.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn_jx3web.Close();
                }
            }

            return id;
        }

        /// <summary>
        /// 查找字符串在ls表中对应的ID
        /// </summary>
        /// <param name="str">字符串</param>
        /// <returns>字符串对应的ID</returns>
        public string InsertIntoLS(string str)
        {
            string strSQL = @"
declare @ret int
declare @str varchar(1000)
set @str = '{0}'
set @ret = (select id from ai_ls where content = @str)
if @ret is not null
	select @ret
else
	begin
	insert into ai_ls(content) values (@str)
	select @@identity
	end";
            strSQL = string.Format(strSQL, str);
            DataTable dt = GetDataTable(strSQL, conn_jx3web);

            return dt.Rows[0][0].ToString();
        }
    #endregion

        /// <summary>
        /// 执行sql命令
        /// </summary>
        /// <param name="sqlString">sql命令</param>
        /// <return>是否否执行成功</return>
        public bool ExecuteSqlCommand(string sqlString)
        {
            bool executeSuccess = true;

            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = sqlString;
                cmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                PrintExceptionInformation(ex);
                executeSuccess = false;
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            return executeSuccess;
        }

        /// <summary>
        /// 执行sql查询命令
        /// </summary>
        /// <param name="sqlString">sql命令</param>
        /// <returns>查询结果</returns>
        public object ExecuteSqlSelectCommand(string sqlString)
        {
            object executeResult = null;

            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }

                SqlCommand cmd = conn.CreateCommand();
                cmd.CommandText = sqlString;
                executeResult = cmd.ExecuteScalar();
            }
            catch (SqlException ex)
            {
                PrintExceptionInformation(ex);
            }
            finally
            {
                if (conn.State == ConnectionState.Open)
                {
                    conn.Close();
                }
            }

            return executeResult;
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询字符串</param>
        /// <returns>数据表</returns>
        public DataTable GetDataTable(string sqlString)
        {
            return GetDataTable(sqlString, this.conn);
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询字符串</param>
        /// <returns>数据表</returns>
        public DataTable GetDataTable(string sqlString, SqlConnection conn)
        {                
            DataTable table = null;

            try
            {
                if (conn.State == ConnectionState.Closed)
                {
                    conn.Open();
                }                

                SqlDataAdapter adapter = new SqlDataAdapter(sqlString, conn);
                adapter.MissingSchemaAction = MissingSchemaAction.AddWithKey;
                DataSet dataSet = new DataSet();
                adapter.Fill(dataSet);
                table = dataSet.Tables[0];
            }
            catch (SqlException ex)
            {
                PrintExceptionInformation(ex);
            }
            finally
            {
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }

            return table;
        }

        /// <summary>
        /// 打印出sql异常信息
        /// </summary>
        /// <param name="ex">sql异常</param>
        private void PrintExceptionInformation(SqlException ex)
        {
            MessageBox.Show(string.Format("执行当前操作产生sql异常：{0}", ex.Message), "数据库操作",
                            MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private void PrintExceptionInformation(Exception ex)
        {
            MessageBox.Show(string.Format("执行当前操作产生异常：{0}", ex.Message), "数据操作",
                            MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
    }
}
