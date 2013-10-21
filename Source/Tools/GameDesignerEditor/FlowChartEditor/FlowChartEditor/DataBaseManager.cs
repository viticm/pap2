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

namespace FlowChartEditor
{
    public class DataBaseManager
    {
        private static DataBaseManager manager = null;  // DataBaseManager的实例
        private SqlConnection conn;                     // 数据库链接
        private static string connectString = ""; // 连接数据库的字符串

        //私有构造函数，不允许在外部初始化
        private DataBaseManager() { }

        //获取DatabaseManager的实例
        public static DataBaseManager GetDataBaseManager()
        {
            if (manager == null) // 对象未初始化
            {
                manager = new DataBaseManager();
                manager.Init(connectString);
            }
            return manager;
        }

        //sql连接属性
        public SqlConnection Connection
        {
            get { return conn; }
        }

        //初始化DataBaseManager对象
        public void Init(string connectString)
        {
            try
            {
                connectString = "Server = cuiwei; DataBase = s3diagram_debug1; Uid = sa; Password = ddd12345678;";
                if(conn != null && conn.ConnectionString == connectString) // 连接字符串没有改变，不需要新建连接
                {

                }
                else
                {
                    conn = new SqlConnection(connectString);
                }
                
            }
            catch (SqlException ex)
            {
                throw (new Exception("在打开sql连接时产生异常: " + ex.ToString()));
            }
        }

    #region 锁相关：加锁、解锁、查锁、清锁
        //加锁
        public bool Diagram_Lock(int nDiagramID)
        {
            string strHostName = System.Net.Dns.GetHostName();
            //string strIp = System.Net.Dns.GetHostEntry(System.Net.Dns.GetHostName()).AddressList[0].ToString();
            //看是否被别人锁了
            DataTable dt = GetDataTable("select * from diagram where id=" + nDiagramID.ToString());
            if(dt.Rows.Count <= 0)
                throw(new Exception("加锁失败：无此记录"));
            string strWhoLocked = dt.Rows[0]["WhoLocked"].ToString();
            if (strWhoLocked != strHostName && strWhoLocked != "")
            {
                MessageBox.Show("加锁失败：已经被" + strWhoLocked + "锁定了");
                return false;
            }
            //开始加锁
            conn.Open();
            int nRet = 0;
            try
            {
                SqlCommand sqlcmd = new SqlCommand("update diagram set wholocked=@hostname where id=" + nDiagramID.ToString(), conn);
                sqlcmd.Parameters.Add(new SqlParameter("@hostname", strHostName));
                nRet = sqlcmd.ExecuteNonQuery();
            }
            catch(Exception ex)
            {
                throw(ex);
            }
            finally
            {
                if (conn.State == ConnectionState.Open) conn.Close();
            }
            return(nRet == 1);
        }

        //解锁
        public bool Diagram_Unlock(int nDiagramID)
        {
            string strHostName = System.Net.Dns.GetHostName();
            //看是不是自己锁的
            DataTable dt = GetDataTable("select * from diagram where id=" + nDiagramID.ToString());
            if (dt.Rows.Count <= 0)
                throw (new Exception("解锁失败：无此记录"));
            string strWhoLocked = dt.Rows[0]["WhoLocked"].ToString();
            if (strWhoLocked == "")
                throw (new Exception("解锁失败：记录并未加锁"));
            if (strWhoLocked != strHostName)
                throw (new Exception("解锁失败：记录是被" + strWhoLocked + "锁定的"));
            //开始解锁
            conn.Open();
            int nRet = 0;
            try
            {
                SqlCommand sqlcmd = new SqlCommand("update diagram set wholocked='' where id=" + nDiagramID.ToString(),conn);
                nRet = sqlcmd.ExecuteNonQuery();
            }
            catch (Exception ex)
            {
                throw (ex);
            }
            finally
            {
                if (conn.State == ConnectionState.Open) conn.Close();
            }
            return(nRet == 1);
        }

        //查锁 为null说明没被锁定，否则返回锁定人的机器名
        public string Diagram_WhoLocked(int nDiagramID)
        {
            DataTable dt = GetDataTable("select * from diagram where (not wholocked is NULL) and wholocked<>'' and id=" + nDiagramID.ToString());
            if (dt.Rows.Count == 0)
                return null;
            else
                return dt.Rows[0]["WhoLocked"].ToString();
        }

        //清空当前用户的所有锁
        public void ClearRecordLocks()
        {
            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                SqlCommand cmd = conn.CreateCommand();
                string sqlString = "Update diagram set wholocked='' where wholocked=" + System.Net.Dns.GetHostName() + "'";
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
                if (conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    conn.Close();
                }
            }
        }
    #endregion


    #region 流程图操作相关，读写流程图、增加、删除、修改流程图
        //加载流程图内容对象
        public object GetDiagramData(string id)
        {
            object content = null; // 流程图内容对象

            try
            {
                string sqlString = string.Format("SELECT data FROM diagram WHERE id = '{0}'", id);
                DataTable scriptTable = GetDataTable(sqlString);
                if (scriptTable.Rows.Count > 0) // 存在对应的脚本记录
                {
                    if (scriptTable.Rows[0]["data"] is DBNull)
                    {
                        content = null;
                    }
                    else
                    {
                        IFormatter formater = new BinaryFormatter();
                        content = formater.Deserialize(new MemoryStream((byte[])scriptTable.Rows[0]["data"]));
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("在获取流程图内容对象时产生异常: " + ex.ToString());
            }

            return content;
        }

        //保存流程图内容对象
        public bool SaveDiagramData(string id, object content)
        {
            bool result = true;

            //序列化对象
            byte[] data = null;
            try
            {
                IFormatter formatter = new BinaryFormatter();
                Stream s = new MemoryStream();
                formatter.Serialize(s, content);
                data = new byte[s.Length];
                s.Position = 0;
                s.Read(data, 0, (int)s.Length);
                s.Close();
            }
            catch(Exception ex)
            {
                throw(ex);
            }

            //写数据库
            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }
                string sqlString = string.Format("UPDATE {0} SET {1} = @data, {2} = \'{3}\', {4} = \'{5}\' WHERE {6} = \'{7}\'",
                    "diagram", "data", "edittime", DateTime.Now, "host", System.Net.Dns.GetHostName(), "id", id);
                SqlCommand cmd = new SqlCommand(sqlString, conn);
                SqlParameter spcode = new SqlParameter("@data", data);
                cmd.Parameters.Add(spcode);
                int iret = cmd.ExecuteNonQuery();
                if (iret != 1) result = false;
            }
            catch (SqlException ex)
            {
                MessageBox.Show("在保存流程图时产生sql异常: " + ex.ToString());
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

        //在数据库创建新的流程图
        public string CreateNewDiagram(string path, bool isFolder)
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
                cmd.Transaction = transaction;

                //查重复
                cmd.CommandText = "select id from Diagram where fullpath='" + path.Replace("'", "''") + "'";
                if (cmd.ExecuteScalar() != null)
                    throw (new Exception("创建流程图时产生异常：有重复的流程图！"));
                
                //插入记录
                string sqlString = string.Format("INSERT INTO Diagram(FullPath,Host,EditTime,IsFolder) VALUES ('{0}','{1}','{2}','{3}')",
                    path, System.Net.Dns.GetHostName(), DateTime.Now, isFolder);
                cmd.CommandText = sqlString;
                int iret = cmd.ExecuteNonQuery();

                //返ID
                sqlString = string.Format("SELECT MAX(id) FROM Diagram");
                cmd.CommandText = sqlString;
                id = cmd.ExecuteScalar().ToString();
                transaction.Commit(); // 提交事务
            }
            catch (Exception ex)
            {
                if (transaction != null) // 回滚事务
                {
                    transaction.Rollback();
                }
                throw (new Exception("在创建新的脚本时产生异常: " + ex.ToString()));
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

        //获取流程图Table，列名：id, path, ClassifyNode
        public DataTable GetDiagramInformation()
        {
            DataTable scriptTable = null;

            try
            {
                string sqlString = "SELECT id, fullpath, isFolder as ClassifyNode from diagram";
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

            return scriptTable;
        }

        //改流程图的path
        public bool UpdateDiagramPath(string id, string newPath)
        {
            bool result = true; // 是否处理成功
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

                cmd.CommandText = "select count(id) from diagram where id='" + id + "'";
                int iret = (int)cmd.ExecuteScalar();
                if (iret == 0) throw (new Exception("无此流程图"));

                cmd.CommandText = "select count(id) from diagram where fullpath='" + newPath + "'";
                iret = (int)cmd.ExecuteScalar();
                if (iret > 0) throw (new Exception("新流程图路径（名称）重复"));

                cmd.CommandText = "update diagram set fullpath='" + newPath + "' where id='" + id + "'";
                iret = cmd.ExecuteNonQuery();
                transaction.Commit();
            }
            catch (SqlException ex)
            {
                MessageBox.Show("更新流程图的路径信息时产生sql异常: " + ex.ToString());
                if (transaction != null) // 事务回滚
                {
                    transaction.Rollback();
                }
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
        /// 删除该路径下的所有流程图
        /// </summary>
        /// <param name="path">当前路径</param>
        /// <returns>是否删除成功</returns>
        public bool DeleteMultiDiagram(string path)
        {
            bool result = true; // 是否执行成功
            SqlTransaction transaction = null; // 数据库事务

            try
            {
                string sqlString = string.Format("SELECT ID FROM {0} WHERE FullPath LIKE '{1}%'", "diagram", path);
                DataTable table = GetDataTable(sqlString);

                if(conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                transaction = conn.BeginTransaction();
                SqlCommand cmd = conn.CreateCommand();
                cmd.Transaction = transaction;
                foreach(DataRow row in table.Rows)
                {
                    string id = row["ID"].ToString();
                    sqlString = string.Format("DELETE FROM {0} WHERE ID = '{1}';", "diagram", id);
                    cmd.CommandText = sqlString;
                    cmd.ExecuteNonQuery();
                }                

                transaction.Commit();
            }
            catch (SqlException ex)
            {
                MessageBox.Show(string.Format("在删除路径{0}下的流程图时产生sql异常: ", path) + ex.ToString());
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

        //删流程图
        public bool DeleteDiagram(string id)
        {
            bool result = true;
            SqlTransaction transaction = null;

            try
            {
                if (conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    conn.Open();
                }

                //transaction = conn.BeginTransaction();
                SqlCommand cmd = conn.CreateCommand();
                //cmd.Transaction = transaction;
                string sqlString = string.Format("UPDATE {0} SET {1} = '{2}', {3} = '{4}' WHERE {5} = '{6}';", "diagram",
                    "host", System.Net.Dns.GetHostName(), "edittime", DateTime.Now, "id", id);
                sqlString += string.Format("DELETE FROM {0} WHERE {1} = '{2}';", "diagram", "id", id);
                cmd.CommandText = sqlString;
                result = (cmd.ExecuteNonQuery() == 1);
                //transaction.Commit();
            }
            catch (SqlException ex)
            {
                MessageBox.Show(string.Format("在删除id为{0}的脚本文件时产生sql异常: ", id) + ex.ToString());
                //transaction.Rollback();
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
    #endregion

        /// <summary>
        /// 获取数据表
        /// </summary>
        /// <param name="sqlString">查询字符串</param>
        /// <returns>数据表</returns>
        private DataTable GetDataTable(string sqlString)
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
            catch(SqlException ex)
            {
                MessageBox.Show("SQL数据库错误："+ ex.Message);
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
