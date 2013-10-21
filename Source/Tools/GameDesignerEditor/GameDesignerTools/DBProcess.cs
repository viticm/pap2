using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Data.Sql;
using System.Data;

namespace GameDesingerTools
{
    public static class DBProcess
    {
        private static SqlConnection m_conn = null;
        
        public static void SetConnection(SqlConnection conn)
        {
            m_conn = conn;

        }

        /// <summary>
        /// 保存脚本到数据库
        /// </summary>
        /// <param name="fieldid"></param>
        /// <param name="tabid"></param>
        /// <param name="strcode"></param>
        /// <returns></returns>
        public static bool SaveScriptByID(string strTableName, string fieldid, string tabid, string strcode)
        {
            if ( m_conn == null )
            {
                return false;
            }
            //先看看是否存在记录再说
            SqlDb sqlDB = new SqlDb();
            sqlDB.SetConnection(m_conn);
            string sql = string.Format("select count(*) as c from sys_scriptstore where fieldid={0} and modtabid={1}", fieldid, tabid);
            
            /* remoting
            DataSet ds = sqlDB.ExeSqlDs(sql, "sys_scriptstore");
            */

            //DataSet ds = new DataSet();
            DataTable tbl_script = Helper.GetDataTableWithSqlProxy("sys_scriptstore", sql, m_conn);
            //ds.Tables.Add(tbl_script);

            if (tbl_script.Rows[0]["c"].ToString() == "0")
            {
                //添加记录一条
                string table_name = "";
                string field_name = "";
                if (fieldid == "0" || fieldid == "-1") // -1 表示所有tab页总的lua代码
                {
                    
                    sql = string.Format("select tablename from sys_modl_tab_def where modtabid='{0}'", tabid);
                    /* remoting
                    DataRowCollection drc = sqlDB.ExeSqlDs(sql, "test").Tables[0].Rows;
                     */

                    DataRowCollection drc = Helper.GetDataTableWithSqlProxy("sys_modl_tab_def", sql, m_conn).Rows;
                    if (drc.Count == 0) return false;
                    table_name = drc[0]["tablename"].ToString();
                    field_name = "#";
                }
                else
                {
                    sql = string.Format("select rtrim(tablename) as table_name, rtrim(fieldname) as field_name from sys_meta_info where fieldid={0}", fieldid);
                    
                    /* remoting
                    DataRowCollection drc = sqlDB.ExeSqlDs(sql, "test").Tables[0].Rows;
                    */

                    DataRowCollection drc = Helper.GetDataTableWithSqlProxy("sys_modl_tab_def", sql, m_conn).Rows;
 

                    if (drc.Count == 0) return false;
                    table_name = strTableName;// drc[0]["table_name"].ToString();
                    field_name = drc[0]["field_name"].ToString();
                }
                sql = @"insert into sys_scriptstore(fieldid, modtabid, tablename, fieldname, scriptlanguage)
                        values ('{0}','{1}','{2}','{3}','LUA')";
                sql = string.Format(sql, fieldid, tabid, table_name, field_name);
                //int ret = sqlDB.ExeNoQuery(sql);
                int ret = Helper.ExecuteCommand("sys_scriptstore", sql, m_conn);
                if (ret == -1)
                {
                    return false;
                }
            }
            /* remoting
            Byte[] ab = Encoding.UTF8.GetBytes(strcode);
            sql = string.Format("update sys_scriptstore set scriptcode=(@strcode) where fieldid={0} and modtabid={1}", fieldid, tabid);
            SqlCommand cmd = new SqlCommand(sql, sqlDB.getConn());
            SqlParameter spcode = new SqlParameter("@strcode", ab);
            cmd.Parameters.Add(spcode);
            cmd.Connection.Open();
            int t = cmd.ExecuteNonQuery();
             */
            Byte[] ab = Encoding.UTF8.GetBytes(strcode);
            string filter = string.Format("fieldid={0} and modtabid={1}", fieldid, tabid);
            DataTable tbl = Helper.GetDataTableProxy("sys_scriptstore", filter, null, m_conn);
            int t = 0;
            foreach (DataRow row in tbl.Rows)
            {
                row["scriptcode"] = ab;
                t++;
            }
            string sql_script = string.Format("select * from sys_scriptstore where {0}", filter);
            Helper.SaveTable(tbl, sql_script, m_conn);
            //cmd.Connection.Close();
            if (t == -1)               //成功的时候，是2
            {
                return false;
            }
            return true;
        }
    }

#region 一个数据库类SqlDb

    public class SqlDb
    {

        //构造函数
        public SqlDb()
        {
            m_sqlCnn = null;
        }


        /// <summary>
        /// Connection参数
        /// </summary>
        private string m_strCnn;   // = System.Configuration.ConfigurationSettings.AppSettings["ConnectionString"];
        private SqlConnection m_sqlCnn = null;    //new SqlConnection(m_strCnn);


        public SqlConnection getConn()
        {
            return m_sqlCnn;
        }

        /// 执行delete，insert，update等操作，返回值为影响的行数
        public int ExeNoQuery(string strSql)
        {
            int nRet = -1;
            m_strError = "";
            SqlCommand sqlCmd = new SqlCommand(strSql, m_sqlCnn);
            try
            {
                OpenCnn();
                nRet = sqlCmd.ExecuteNonQuery();
                
            }
            catch (SqlException ex)
            {
                nRet = -1;
                m_strError = ex.ToString();
            }
            finally
            {
                if (m_sqlCnn.State == ConnectionState.Open)
                {
                    sqlCmd.Dispose();
                    m_sqlCnn.Close();
                }
            }
            return nRet;
        }

        /// 执行delete，update,insert操作，返回值为受影响的行数
        public int ExeNoQuery(SqlCommand sqlCmd)
        {
            int nRet = -1;
            m_strError = "";
            sqlCmd.Connection = m_sqlCnn;
            try
            {
                OpenCnn();
                nRet = sqlCmd.ExecuteNonQuery();
            }
            catch (SqlException ex)
            {
                nRet = -1;
                m_strError = ex.ToString();
            }
            finally
            {
                if (m_sqlCnn.State == ConnectionState.Open)
                {
                    m_sqlCnn.Close();
                }
            }
            return nRet;
        }

        //判断是否存在
        public int ExeIsExist(string strSql)
        {
            int nExist = -1;
            m_strError = "";
            SqlCommand sqlCmd = new SqlCommand(strSql, m_sqlCnn);
            try
            {
                OpenCnn();
                SqlDataReader sqlReader = sqlCmd.ExecuteReader();
                if (sqlReader.Read())
                {
                    nExist = 1;
                }
                else
                {
                    nExist = 0;
                }
                sqlReader.Close();

            }
            catch (SqlException ex)
            {
                nExist = -1;
                m_strError = ex.ToString();
            }
            finally
            {
                if (m_sqlCnn.State == ConnectionState.Open)
                {
                    sqlCmd.Dispose();
                    m_sqlCnn.Close();
                }
            }
            return nExist;
        }

        //判断是否存在
        public int ExeIsExist(SqlCommand sqlCmd)
        {
            int nExist = -1;
            m_strError = "";
            sqlCmd.Connection = m_sqlCnn;
            try
            {
                OpenCnn();
                SqlDataReader sqlReader = sqlCmd.ExecuteReader();
                if (sqlReader.Read())
                {
                    nExist = 1;
                }
                else
                {
                    nExist = 0;
                }
                sqlReader.Close();
            }
            catch (SqlException ex)
            {
                nExist = -1;
                m_strError = ex.ToString();
            }
            finally
            {
                if (m_sqlCnn.State == ConnectionState.Open)
                {
                    m_sqlCnn.Close();
                }
            }
            return nExist;
        }


        //返回DataSet
        public DataSet ExeSqlDs(string strSql, string strTable)
        {
            m_strError = "";
            SqlDataAdapter sqlAdapter = new SqlDataAdapter(strSql, m_sqlCnn);
            DataSet dbSet = new DataSet();
            try
            {
                sqlAdapter.Fill(dbSet, strTable);
            }
            catch (SqlException ex)
            {
                dbSet = null;
                m_strError = ex.ToString();
            }

            return dbSet;
        }


        //返回DataSet
        public DataSet ExeSqlDs(string strSql, int nStart, int nCount, string strTable)
        {
            m_strError = "";
            SqlDataAdapter sqlAdapter = new SqlDataAdapter(strSql, m_sqlCnn);
            DataSet dbSet = new DataSet();
            try
            {
                sqlAdapter.Fill(dbSet, nStart, nCount, strTable);
            }
            catch (SqlException ex)
            {
                dbSet = null;
                m_strError = ex.ToString();
            }
            return dbSet;
        }


        //返回DataReader
        public SqlDataReader ExeSqlDr(string strSql)
        {
            m_strError = "";
            SqlDataReader dr = null;
            SqlCommand sqlCmd = new SqlCommand(strSql, m_sqlCnn);
            try
            {
                OpenCnn();
                dr = sqlCmd.ExecuteReader();
            }
            catch (SqlException ex)
            {
                dr = null;
                m_strError = ex.ToString();
            }
            finally
            {
                if (m_sqlCnn.State == ConnectionState.Open)
                {
                    sqlCmd.Dispose();
                }
            }
            return dr;
        }

        //返回DataTable
        public DataTable GetDataTable(string sql)
        {
            SqlDataAdapter adp = new SqlDataAdapter(sql, m_sqlCnn);
            adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            DataSet ds = new DataSet();
            adp.Fill(ds);
            DataTable tbl = ds.Tables[0];
            return tbl;
        }

        //执行ExecuteScalar
        public int ExeScalar(string strSql)
        {
            int nCount = -1;
            m_strError = "";
            SqlCommand sqlCmd = new SqlCommand(strSql, m_sqlCnn);
            try
            {
                OpenCnn();
                nCount = (int)sqlCmd.ExecuteScalar();
            }
            catch (SqlException ex)
            {
                nCount = -1;
                m_strError = ex.ToString();
            }
            finally
            {
                if (m_sqlCnn.State == ConnectionState.Open)
                {
                    sqlCmd.Dispose();
                    m_sqlCnn.Close();
                }
            }
            return nCount;
        }

        //打开Connection
        private void OpenCnn()
        {
            if (m_sqlCnn.State == ConnectionState.Open)
            {
                m_sqlCnn.Close();
            }
            m_sqlCnn.Open();
        }

        //关闭Connection
        public void CloseCnn()
        {
            if (m_sqlCnn.State == ConnectionState.Open)
            {
                m_sqlCnn.Close();
            }
        }


        //返回DataSet
        public DataSet ExeSqlDs(SqlCommand sqlCmd, string strTable)
        {
            m_strError = "";
            DataSet ds = new DataSet();
            sqlCmd.Connection = m_sqlCnn;
            SqlDataAdapter sqlAdapter = new SqlDataAdapter();
            sqlAdapter.SelectCommand = sqlCmd;
            try
            {
                sqlAdapter.Fill(ds, strTable);
            }
            catch (SqlException ex)
            {
                ds = null;
                m_strError = ex.ToString();
            }
            return ds;
        }


        //设定Connection 字符串，new Connection
        public void SetConnectionString(string strSource)
        {
            m_strCnn = strSource;
            m_sqlCnn = null;
            m_sqlCnn = new SqlConnection(m_strCnn);
        }

        //设定SqlConnection
        public void SetConnection(SqlConnection conn)
        {
            m_sqlCnn = conn;
        }

        /// 错误信息
        private string m_strError;


        /// 错误信息
        public string StrErrorInfo
        {
            get
            {
                return m_strError;
            }
        }
    }
#endregion

}
