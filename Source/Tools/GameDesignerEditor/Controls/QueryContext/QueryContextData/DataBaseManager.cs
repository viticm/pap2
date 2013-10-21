using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Data;
using System.Collections;
using System.Text;
using System.IO;

namespace DataBaseIO
{
    public class DataBaseManager
    {
        private static DataBaseManager manager = null; // DataBaseManager的实例
        private DataTable lsTable = null; // ls数据表
        private Hashtable lsHashtable = null; // ls hash表
        public SqlConnection conn; // 数据库链接


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
        /// 在数据库创建新的脚本
        /// </summary>
        /// <param name="path">脚本</param> 
        /// <returns>脚本的id</returns>
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

                sqlString = string.Format("SELECT MAX({0}) FROM {1}", "id", "sys_script_script");
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
                foreach (DataRow r in lsTable.Rows)
                {
                    lsHashtable[r["id"].ToString()] = r["content"].ToString();
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
                //connectString = "Server = jx3web; DataBase = s3design; Uid = s3design; Password = ddd12345678;";
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

                string sqlString = string.Format("SELECT * FROM {0} WHERE {1} = \'{2}\'", 
                    "sys_script_lsfile", "classification", classification);
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
        /// 关闭sql连接
        /// </summary>
        private void CloseConnection()
        {
            try
            {
                if (conn != null) // sql连接非空
                {
                    if (conn.State == ConnectionState.Open) // sql连接已经打开
                    {
                        conn.Close();
                    }
                }
                else
                {
                    MessageBox.Show("sql连接尚未初始化!");
                }
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在关闭sql连接时产生异常: " + ex.ToString());
            }
        }

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询语句</param>
        /// <returns>数据表</returns>
        public DataTable GetDataTable(string sqlString)
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
        public DataTable GetDataTable(string sqlString, SqlTransaction transaction)
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