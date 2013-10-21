using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Data;

namespace GameDesingerTools
{
    /*
     *例子：
            TableLock l = new TableLock(m_conn, MainTableName);
            l.Lock();
            l.UnLock();
            l.Clear();
     */
    class TableLock
    {
        public TableLock(SqlConnection conn, string strTableName)
        {
            m_conn = conn;
            m_strTableName = strTableName;

            if (m_conn == null || m_strTableName == null || m_strTableName.Length == 0)
                throw new Exception("无法锁定，请检查 TableLock 是否传入正确的参数。");

            string strsql = string.Format("select * from sys_lock_table where tablename='{0}'", strTableName);
            m_adp = new SqlDataAdapter(strsql, m_conn);
            m_adp.MissingSchemaAction = MissingSchemaAction.AddWithKey;
            m_ds = new DataSet();
            m_cmdBuilder = new SqlCommandBuilder(m_adp);
            m_strIP = Helper.GetIPAddress();
            m_strHostName = Helper.GetHostName();
        }
        public void Clear()
        {
            m_conn = null;
            if (m_adp != null)
                m_adp.Dispose();
            if (m_ds != null)
                m_ds.Dispose();
            if (m_cmdBuilder != null)
                m_cmdBuilder.Dispose();
        }
        public bool Lock()
        {
            DataTable tbl  = GetTable();
            bool bRet = false;
            try
            {
                DataRow row = GetLockRow(tbl);
                string strlockip = row["lockip"] == null ? "" : row["lockip"].ToString();
                if (strlockip == m_strIP || strlockip.Trim().Length == 0)
                {
                    bRet = UpdateRow(tbl, row, Convert.ToInt32(row["lastversion"]), DateTime.Now, DateTime.MinValue, true);
                }
                else
                    return false;
            }
            finally
            {
                tbl.Dispose();
            }
            return bRet;
        }
        public bool UnLock()
        {
            DataTable tbl = GetTable();
            bool bRet = false;
            try
            {
                DataRow row = GetLockRow(tbl);
                string strlockip = row["lockip"] == null ? "" : row["lockip"].ToString();
                if (strlockip == m_strIP || strlockip.Trim().Length == 0)
                {
                    bRet = UpdateRow(tbl, row, Convert.ToInt32(row["lastversion"]) + 1, DateTime.MinValue, DateTime.Now, false);
                }
                else
                    return false;
            }
            finally
            {
                tbl.Dispose();
            }
            return bRet;
        }
        //表示清除任何人加的锁，建议只在管理员帐号中使用。
        public bool ForceClearLock()
        {
            DataTable tbl = GetTable();
            bool bRet = false;
            try
            {
                DataRow row = GetLockRow(tbl);
                bRet = UpdateRow(tbl, row, Convert.ToInt32(row["lastversion"]), DateTime.MinValue, DateTime.MinValue, false);
            }
            finally
            {
                tbl.Dispose();
            }
            return bRet;
        }
        
        //返回-1表示没有取到版本号
        public int GetServerLastVersion()
        {
            DataTable tbl = GetTable();
            int nVer = -1;
            try
            {
                //DataRow row = GetLockRow(tbl);
                DataRow row = tbl.Rows.Find(m_strTableName);
                if (row == null)
                    return 0;
                else
                    nVer = row["lastversion"] == DBNull.Value ? 0 : Convert.ToInt32(row["lastversion"]);
            }
            finally
            {
                tbl.Dispose();
            }
            return nVer;
        }
        //Private:
        
        private DataTable GetTable()
        {
            m_adp.Fill(m_ds);
            return m_ds.Tables[0];
        }
        private DataRow GetLockRow(DataTable tbl)
        {
            DataRow row = tbl.Rows.Find(m_strTableName);
            if (row == null)
            {
                try
                {
                    DataRow newrow = tbl.NewRow();
                    newrow.BeginEdit();

                    newrow["tablename"] = m_strTableName;
                    newrow["lastversion"] = 0;

                    newrow["lockip"] = DBNull.Value;
                    newrow["lockhostname"] = DBNull.Value;
                    newrow["lockusername"] = DBNull.Value;

                    newrow["locktime"] = DBNull.Value;// DateTime.MinValue;
                    newrow["unlocktime"] = DBNull.Value;//DateTime.MinValue;

                    newrow.EndEdit();
                    tbl.Rows.Add(newrow);

                    SqlCommandBuilder cmdBuilder = m_cmdBuilder;// new SqlCommandBuilder(m_adp);
                    m_adp.InsertCommand = cmdBuilder.GetInsertCommand();
                    int val = m_adp.Update(tbl);
                    tbl.AcceptChanges();
                    row = newrow;
                }
                catch (Exception ex)
                {
                    tbl.RejectChanges();
                    Helper.AddLog(ex.Message);
                    throw ex;
                }
            }
            return row;
        }
        private bool UpdateRow(DataTable tbl, DataRow row, int nLastVer, DateTime dtLock, DateTime dtUnLock, bool bLock)
        {
            try
            {
                row.BeginEdit();

                row["tablename"] = m_strTableName;
                row["lastversion"] = nLastVer;

                if (bLock)
                {
                    row["lockip"] = m_strIP;
                    row["lockhostname"] = m_strHostName;
                    row["lockusername"] = m_strHostName;

                    if (dtLock == DateTime.MinValue)
                        row["locktime"] = DBNull.Value;
                    else
                        row["locktime"] = dtLock;

                    if (dtUnLock == DateTime.MinValue)
                        row["unlocktime"] = DBNull.Value;
                    else
                        row["unlocktime"] = dtUnLock;
                }
                else
                {
                    row["lockip"] = DBNull.Value;
                    row["lockhostname"] = DBNull.Value;
                    row["lockusername"] = DBNull.Value;

                    row["locktime"] = DBNull.Value;
                    if (dtUnLock == DateTime.MinValue)
                        row["unlocktime"] = DBNull.Value;
                    else
                        row["unlocktime"] = dtUnLock;
                }

                row.EndEdit();

                SqlCommandBuilder cmdBuilder = m_cmdBuilder;// new SqlCommandBuilder(m_adp);
                m_adp.UpdateCommand = cmdBuilder.GetUpdateCommand();
                int val = m_adp.Update(tbl);
                tbl.AcceptChanges();
                return val > 0;
            }
            catch (Exception ex)
            {
                tbl.RejectChanges();
                Helper.AddLog(ex.Message);
                throw ex;
            }
        }
        
        private SqlConnection m_conn;
        private DataSet m_ds;
        private string m_strTableName, m_strIP, m_strHostName;
        SqlCommandBuilder m_cmdBuilder;
        private SqlDataAdapter m_adp;        
    }
}

